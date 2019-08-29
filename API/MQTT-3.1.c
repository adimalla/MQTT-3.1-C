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


/* @brief MACRO defines */
#define DEBUG 	  1
#define DEBUG_ALL 0

#define PORT 1883

#define FIXED_HEADER_LENGTH   2
#define PROTOCOL_NAME_LENGTH  6
#define CLIENT_NAME_LENGTH    23
#define TOPIC_LENGTH          40

#pragma pack(1)


/* @brief Fixed Header */
typedef struct mqtt_fixed_header
{
	//uint8_t message_flag : 4; /*!< LSB */
    uint8_t retain_flag  : 1;
    uint8_t qos_level    : 2;
    uint8_t dup_flag     : 1;

	uint8_t message_type : 4; /*!< MSB */

}mqtt_fixed_header_t;


/* @brief MQTT CONNECT structure */
typedef struct mqtt_connect
{
	mqtt_fixed_header_t control_packet;
	uint8_t  message_length;
	uint16_t protocol_name_length;
	char  protocol_name[6];
	uint8_t  protocol_version;
	uint8_t  connect_flags;
	uint16_t keep_alive_value;
	uint16_t client_id_length;
	char  client_name[23];

}mqtt_connect_t;


/* @brief MQTT CONNACK structure */
typedef struct mqtt_connack
{
	mqtt_fixed_header_t control_packet;
	uint8_t message_length;
	uint8_t connect_ack_flags;
	uint8_t return_code;

}mqtt_connack_t;


/* @brief MQTT PUBLISH structure */
typedef struct mqtt_publish
{
	mqtt_fixed_header_t control_packet;
	uint8_t  message_length;
	uint16_t topic_length;
	char  topic_name[40];
	uint8_t  payload[100];

}mqtt_publish_t;


/* TODO Test QOS
// with qos
typedef struct mqtt_publish_qos
{
	mqtt_fixed_header_t control_packet;
	uint8_t  message_length;
	uint16_t topic_length;
	uint8_t  topic_name[40];
	uint16_t message_id;
	uint8_t  payload[100];

}mqtt_publish_qos_t;
*/


typedef struct mqtt_disconnect
{
	mqtt_fixed_header_t control_packet;
	uint8_t message_length;

}mqtt_disconnect_t;




int main()
{

	int client_sfd;
	struct sockaddr_in server_addr;

	mqtt_connect_t    *connect_msg;
	mqtt_connack_t    *connack_msg;
	mqtt_publish_t    *publish_msg;
	mqtt_disconnect_t *disconnect_msg;

	char message[2000]     = {0};
	char read_buffer[1500] = {0};
	char *my_client_name   = "mosqpub|1234-adityamall";
	char *my_client_topic  = "temperature/device1";
	char *pub_message;
	uint8_t pub_message_length;

	/* Fill mqtt CONNECT message structure */
	connect_msg = (void *)&message;

    printf("Address of message:%p vs of Adress of &message:%p\n", message, &message);

	connect_msg->control_packet.message_type = 0x01;
	connect_msg->protocol_name_length 	 = htons(6);

	strncpy(connect_msg->protocol_name,"MQIsdp", 6);

	connect_msg->protocol_version = 3;
	connect_msg->connect_flags    = 0x02;
	connect_msg->keep_alive_value = htons(60);

	connect_msg->client_id_length = htons(23);
	strcpy(connect_msg->client_name, my_client_name);

	connect_msg->message_length = 37;

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

	/* Send mqtt packet */
	write(client_sfd, message, connect_msg->message_length + FIXED_HEADER_LENGTH);

#if DEBUG
	printf("%s :Sending CONNECT\n", my_client_name);
#endif

	/* Fill mqtt PUBLISH message strcuture */
	memset(message, '\0', sizeof(message));

    publish_msg = (void *)&message;
	publish_msg->control_packet.message_type = 0x03;
	publish_msg->topic_length = htons(40);

	strcpy(publish_msg->topic_name, my_client_topic);

	pub_message = "65 F";
	strncpy(publish_msg->payload, pub_message, strlen(pub_message));
	pub_message_length = strlen(pub_message);

	publish_msg->message_length = pub_message_length + TOPIC_LENGTH + 2;

#if DEBUG_ALL
	printf("topic:%s, len:%ld\n", publish_msg->topic_name, strlen(publish_msg->topic_name));
	printf("message:%s, len:%ld\n", publish_msg->payload, strlen(publish_msg->payload));
	printf("Msg Len %d\n",publish_msg->message_length);
#endif

	/* @brief check CONNACK message */
	connack_msg = (void *)read_buffer;

	/* TODO implement FSM */
	while( (read( client_sfd , read_buffer, 1500)) > 0)
	{

#if DEBUG || DEBUG_ALL
		if(connack_msg->control_packet.message_type == 2)
		{
			printf("%s :Received CONNACK\n", my_client_name);
		}
#endif

		if(connack_msg->return_code == 0)
		{

#if DEBUG || DEBUG_ALL
			printf("%s :Connection Accepted\n", my_client_name);
#endif
			/* @brief send publish message */
			write(client_sfd, message, publish_msg->message_length + FIXED_HEADER_LENGTH);

#if DEBUG || DEBUG_ALL
			printf("%s :Sending PUBLISH(\"%s\",...(%d bytes))\n", my_client_name, publish_msg->topic_name, pub_message_length);
#endif

			/* @brief Fill DISCONNECT structure */
			memset(message,'\0',sizeof(message));

            disconnect_msg = (void*)message;
			disconnect_msg->control_packet.message_type = 14;
			disconnect_msg->message_length = 0;

			write(client_sfd, message, disconnect_msg->message_length + FIXED_HEADER_LENGTH);

#if DEBUG || DEBUG_ALL
			printf("%s :Sending DISCONNECT\n",my_client_name);
#endif

			break;
		}
		else if(connack_msg->return_code == 2)
		{

#if DEBUG || DEBUG_ALL
			printf("%s :Connection Refused\n", my_client_name);
#endif
			break;
		}
	}

	close(client_sfd);

	return 0;
}
