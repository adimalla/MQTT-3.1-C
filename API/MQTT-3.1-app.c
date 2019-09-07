/**
 ******************************************************************************
 * @file    pub_mqtt.c, file name will change
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

#include <mqtt_client.h>


/* @brief MACRO defines */
#define DEBUG 	  1
#define DEBUG_ALL 0

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
	char *my_client_name   = "pub|1234-adityamall";
	char *my_client_topic  = "temperature/device1";
	char user_name[]       = "device1.sensor";
	char pass_word[]       = "1234";
	char *pub_message;

	uint8_t mqtt_message_state;

	uint8_t loop_state = 0;

	int retval = 0;

	/* client socket */
	if( (client_sfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
	{
		fprintf(stderr,"Error: socket failed \n");
		return -1;
	}

	server_addr.sin_family      = AF_INET;
	server_addr.sin_port        = htons(PORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	/* Connect to server */
	if( ( connect(client_sfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) ) < 0)
	{
		fprintf(stderr,"Error: connect failed \n");
		return -1;
	}


	loop_state = FSM_RUN;

	mqtt_message_state = IDLE_STATE;

	/*@ MQTT Finite state machine */
	while(loop_state)
	{

		switch(mqtt_message_state)
		{

		case mqtt_idle_state:

			printf("FSM Idle Start \n");

			loop_state = FSM_RUN;

			mqtt_message_state = MQTT_CONNECT_MESSAGE;

			break;


		case mqtt_read_state:

			/* @brief Read Message type from the socket read buffer */
			read( client_sfd , read_buffer, 1500);

			publisher.message = (void*)read_buffer;

			/* Update State */
			if(publisher.message->message_type == MQTT_CONNACK_MESSAGE)
			{
				mqtt_message_state = publisher.message->message_type;
			}
			else if(publisher.message->message_type == MQTT_PUBACK_MESSAGE)
			{
				mqtt_message_state = publisher.message->message_type;
			}
			else
			{
				mqtt_message_state = MQTT_DISCONNECT_MESSAGE;
			}

			/* Clear read buffer after reading */
			memset(read_buffer, '\0', sizeof(message));

			break;


		case mqtt_connect_state:

			/* Fill mqtt CONNECT message structure */
			publisher.connect_msg = (void *)&message;
			publisher.connect_msg->connect_flags.clean_session = ENABLE;

			/* Setup User name password (optional) */
			retval = mqtt_client_username_passwd(&publisher, user_name, pass_word);
			if(retval == -1)
			{
				printf("Bad value of user name or password\n");

				mqtt_message_state = EXIT_STATE;

				break;
			}

			/* Setup mqtt CONNECT Message  */
			message_length = mqtt_connect(&publisher, my_client_name, 60);

			/* Send mqtt CONNECT  (through socket API) */
			write(client_sfd, (char*)publisher.connect_msg, message_length);

			/* Print debug message */
			printf("%s :Sending CONNECT\n", my_client_name);

			/* Update state */
			mqtt_message_state = READ_STATE;

			break;


		case mqtt_connack_state:

			/* @brief print debug message */
			printf("%s :Received CONNACK\n", my_client_name);

			/* Check return code of CONNACK message */
			publisher.connack_msg = (void *)read_buffer;

			if(publisher.connack_msg->return_code == MQTT_CONNECTION_ACCEPTED)
			{

				/* Update State */
				mqtt_message_state = MQTT_PUBLISH_MESSAGE;

			}
			else if(publisher.connack_msg->return_code == MQTT_CONNECTION_REFUSED)
			{

				/* TODO Update State for connack connection refused */

			}
			break;


		case mqtt_publish_state:

			pub_message = "Test Message from Client!!";

			/* Fill mqtt PUBLISH message strcuture */
			memset(message, '\0', sizeof(message));

			publisher.publish_msg = (void *)message;

			/*Configure publish options */
			message_status = mqtt_publish_options(&publisher, MQTT_MESSAGE_NO_RETAIN, QOS_ATLEAST_ONCE);

			/* Configure publish message */
			message_length = mqtt_publish(&publisher, my_client_topic, pub_message);

			/* @brief send publish message (Socket API) */
			write(client_sfd, (char*)publisher.publish_msg, message_length);

			/* @brief print debug message */
			printf("%s :Sending PUBLISH(\"%s\",...(%ld bytes))\n", my_client_name, publisher.publish_msg->topic_name, strlen(pub_message));

			/* Update State */
			if(message_status == QOS_ATLEAST_ONCE || message_status == QOS_EXACTLY_ONCE)
			{
				mqtt_message_state = READ_STATE;
			}
			else
			{
				mqtt_message_state = MQTT_DISCONNECT_MESSAGE;
			}

			break;


		case mqtt_puback_state:

			printf("%s :Received PUBACK\n",my_client_name);

			mqtt_message_state = MQTT_DISCONNECT_MESSAGE;

			break;


		case mqtt_disconnect_state:

			/* @brief Fill DISCONNECT structure */
			memset(message,'\0',sizeof(message));

			publisher.disconnect_msg = (void*)message;

			message_length = mqtt_disconnect(&publisher);

			/* Send Disconnect Message */
			write(client_sfd, message, message_length);

			/* @brief print debug message */
			printf("%s :Sending DISCONNECT\n",my_client_name);

			/* Update State */
			mqtt_message_state = EXIT_STATE;

			break;


		case mqtt_exit_state:

			printf("FSM Exit state \n");

			/* Close socket */
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
