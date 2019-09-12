/**
 ******************************************************************************
 * @file    MQTT-3,1-app.c
 * @author  Aditya Mall,
 * @brief   Example MQTT publish client, for mosquitto MQTT Broker
 *
 *  Info
 *          Only for testing, (POSIX compatible only.)
 *
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2019 Aditya Mall </center></h2>
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */


/* @brief Header files */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdint.h>

#include "mqtt_client.h"


/* @brief MACRO defines */

#define LOOPBACK 0
#define IOT_LAB  0
#define WLAN     1

#if WLAN

#if IOT_LAB
#define RASP_IP_ADDR "192.168.1.186"
#else
#define RASP_IP_ADDR "192.168.1.12"
#endif

#else
#define RASP_IP_ADDR "10.42.0.217"
#endif

#define PORT 1883


int main()
{
	int client_sfd;
	struct sockaddr_in server_addr;

	size_t message_length;
	int8_t message_status;

	mqtt_client_t publisher;

	char message[2000]     = {0};
	char read_buffer[1500] = {0};
	char *my_client_name   = "pub|1990-adityamall";
	char *my_client_topic  = "temperature/device1";
	char user_name[]       = "device1.sensor";
	char pass_word[]       = "4321";
	char *pub_message;

	uint8_t mqtt_message_state;

	uint8_t loop_state = 0;
	int retval = 0;

	/* client socket */
	if( (client_sfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
	{
		printf("Error: socket failed \n");
		return -1;
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port   = htons(PORT);

#if LOOPBACK
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
#else
	server_addr.sin_addr.s_addr = inet_addr(RASP_IP_ADDR);
#endif

	/* Connect to server */
	if( ( connect(client_sfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) ) < 0)
	{
		printf("Error: connect failed \n");
		return -1;
	}


	/* State machine initializations */
	loop_state = FSM_RUN;

	mqtt_message_state = mqtt_idle_state;

	/* MQTT Finite state machine */
	while(loop_state)
	{
		switch(mqtt_message_state)
		{

		case mqtt_idle_state:

			fprintf(stdout,"FSM Idle Start");

			/* Update state to connect to send connect message */
			mqtt_message_state = mqtt_connect_state;

			break;


		case mqtt_read_state:

			/* @brief Read Message type from the socket read buffer */
			read(client_sfd , read_buffer, 1500);

			publisher.message = (void*)read_buffer;

			/* get MQTT message type and update state */
			mqtt_message_state = get_mqtt_message_type(&publisher);
			if(!mqtt_message_state)
			{
				mqtt_message_state = mqtt_disconnect_state;
			}

			break;



		case mqtt_connect_state:

			/* Fill mqtt CONNECT message structure */

			memset(message, '\0', sizeof(message));

			publisher.connect_msg = (void *)&message;

			/* Setup User name password (optional) */
			retval = mqtt_client_username_passwd(&publisher, user_name, pass_word);
			if(retval == -1)
			{
				fprintf(stdout,"Bad value of user name or password\n");

				mqtt_message_state = mqtt_exit_state;

				break;
			}


			/* Set connect options */
			retval = mqtt_connect_options(&publisher, MQTT_CLEAN_SESSION, QOS_EXACTLY_ONCE, MQTT_MESSAGE_NO_RETAIN);
			if(retval == -1)
			{
				fprintf(stdout,"Bad value of connect options params\n");

				mqtt_message_state = mqtt_exit_state;

				break;
			}


			/* Setup mqtt CONNECT Message  */
			message_length = mqtt_connect(&publisher, my_client_name, 60);

			/* Send mqtt CONNECT  (through socket API) */
			retval = write(client_sfd, (char*)publisher.connect_msg, message_length);
			if(retval == -1)
			{
				printf("Connect write error, Socket closed by server \n");

				mqtt_message_state = mqtt_exit_state;

				break;
			}

			/* Print debug message */
			fprintf(stdout, "%s :Sending CONNECT\n", my_client_name);

			/* Update state */
			mqtt_message_state = mqtt_read_state;

			break;



		case mqtt_connack_state:

			/* @brief print debug message */
			fprintf(stdout,"%s :Received CONNACK\n", my_client_name);

			/* Check return code of CONNACK message */
			publisher.connack_msg = (void *)read_buffer;

			mqtt_message_state = get_connack_status(&publisher);

			break;



		case mqtt_publish_state:

			pub_message = "Test Message from client PC";

			/* Fill mqtt PUBLISH message strcuture */
			memset(message, '\0', sizeof(message));

			publisher.publish_msg = (void *)message;

			/*Configure publish options */
			message_status = mqtt_publish_options(&publisher, MQTT_MESSAGE_NO_RETAIN, QOS_EXACTLY_ONCE);
			if(message_status == -1)
			{
				fprintf(stdout, "publish options param error\n");

				mqtt_message_state = mqtt_disconnect_state;

				break;
			}

			/* Configure publish message */
			message_length = mqtt_publish(&publisher, my_client_topic, pub_message);
			if(message_length == 0)
			{
				fprintf(stdout,"publish message param error\n");

				mqtt_message_state = mqtt_disconnect_state;

				break;
			}

			/* @brief send publish message (Socket API) */
			write(client_sfd, (char*)publisher.publish_msg, message_length);

			/* @brief print debug message */
			fprintf(stdout, "%s :Sending PUBLISH(\"%s\",...(%ld bytes))\n", my_client_name, my_client_topic, strlen(pub_message));

			/* Update State according to quality of service */
			if(message_status == QOS_ATLEAST_ONCE || message_status == QOS_EXACTLY_ONCE)
			{
				mqtt_message_state = READ_STATE;
			}
			else
			{
				mqtt_message_state =  mqtt_disconnect_state;
			}

			break;



		case mqtt_puback_state:

			printf("%s :Received PUBACK\n",my_client_name);

			mqtt_message_state =  mqtt_disconnect_state;
			//mqtt_message_state =  mqtt_publish_state;

			break;



		case mqtt_pubrec_state:

			printf("%s :Received PUBREL\n",my_client_name);

			mqtt_message_state = mqtt_pubrel_state;

			break;



		case mqtt_pubrel_state:

			/* Fill mqtt PUBREL message strcuture */
			memset(message, '\0', sizeof(message));

			publisher.pubrel_msg = (void *)message;

			message_length = mqtt_publish_release(&publisher);

			/* @brief send pubrel message (Socket API) */
			write(client_sfd, (char*)publisher.pubrel_msg, message_length);

			fprintf(stdout,"%s :Sending PUBREL\n",my_client_name);

			mqtt_message_state = mqtt_read_state;

			break;



		case mqtt_pubcomp_state:

			printf("%s :Received PUBCOMP\n",my_client_name);

			mqtt_message_state =  mqtt_disconnect_state;

			break;



		case mqtt_disconnect_state:

			/* @brief Fill DISCONNECT structure */
			memset(message,'\0',sizeof(message));

			publisher.disconnect_msg = (void*)message;

			message_length = mqtt_disconnect(&publisher);

			/* Send Disconnect Message */
			write(client_sfd, (char*)publisher.disconnect_msg, message_length);

			/* @brief print debug message */
			printf("%s :Sending DISCONNECT\n",my_client_name);

			/* Update State */
			mqtt_message_state = mqtt_exit_state;

			break;



		case mqtt_exit_state:

			printf("FSM Exit state \n");

			/* Close socket */
			shutdown(client_sfd, SHUT_RD);
			close(client_sfd);

			/* Suspend while loop */
			loop_state = FSM_SUSPEND;

			break;

		default:
			break;

		}

	}

	printf("Exit from FSM \n");

	return 0;
}
