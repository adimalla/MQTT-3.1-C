/**
 ******************************************************************************
 * @file    mqtt_pub.c, file name will change
 * @author  Aditya Mall,
 * @brief   Example MQTT publish client, for mosquitto MQTT Broker
 *
 *  Info
 *          API
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


/*
 * Standard Header and API Header files
 */
#include <stdint.h>
#include <string.h>
#include "mqtt_configs.h"
#include "mqtt_client.h"



/******************************************************************************/
/*                                                                            */
/*                  Data Structures and Defines                               */
/*                                                                            */
/******************************************************************************/


/* @brief Defines for CONNECT Message */
#define CONNECT_PROTOCOL_LENGTH_SIZE   2
#define CONNECT_PROTOCOL_NAME_SIZE     PROTOCOL_NAME_LENGTH
#define CONNECT_PROTOCOL_VERSION_SIZE  1
#define CONNECT_FLAGS_SIZE             1
#define CONNECT_KEEP_ALIVE_TIME_SIZE   2
#define CONNECT_CLIENT_ID_LENGTH_SIZE  2
#define CONNECT_USER_NAME_LENGTH_SIZE  2
#define CONNECT_PASSWORD_LENGTH_SIZE   2

/* return codes for mqtt api functions */
enum function_return_codes
{
	func_opts_error      = -1, /*!< */
	func_opts_success    = 1,  /*!< */
	func_param_len_error = 0,  /*!< */
};



/******************************************************************************/
/*                                                                            */
/*                              API Functions                                 */
/*                                                                            */
/******************************************************************************/



/*
 * @brief  static function to convert to network byte order for mqtt packets
 * @param  value    : value in host byte order format
 * @retval uint16_t : value network byte order format
 */
static uint16_t mqtt_htons(uint16_t value)
{
	value  = ((value & 0xFF00) >> 8) + ((value & 0x00FF) << 8);

	return value;
}



/*
 * @brief  Configures mqtt client user name and password.
 * @param  *client   : pointer to mqtt client structure (mqtt_client_t).
 * @param  user_name : mqtt client user name
 * @param  password  : mqtt client password
 * @retval int8_t    : 1 = Success, -1 = Error
 */
int8_t mqtt_client_username_passwd(mqtt_client_t *client, char *user_name, char *password)
{
	uint8_t user_name_length = 0;
	uint8_t password_length  = 0;

	user_name_length = strlen(user_name);
	password_length = strlen(password);

	/* check if user name is not null */
	if( (user_name_length == 0) || (password_length == 0))
	{
		return func_opts_error;
	}

	/* check if user name and password doesn't exceed defined length, if yes return 0 */
	if( user_name_length > USER_NAME_LENGTH || password_length > PASSWORD_LENGTH)
	{
		return func_opts_error;
	}
	else
	{
		client->connect_msg->connect_flags.user_name_flag = ENABLE;
		client->connect_msg->connect_flags.password_flag  = ENABLE;


		//client->connect_msg->message_payload[0] = 0;

		client->connect_msg->payload_options.user_name_length = mqtt_htons(user_name_length);
		strcpy(client->connect_msg->payload_options.user_name, user_name);

		client->connect_msg->payload_options.password_length  = mqtt_htons(password_length);
		strcpy(client->connect_msg->payload_options.password, password);

	}

	return func_opts_success;
}



/*
 * @brief  Configures mqtt CONNECT message structure.
 * @param  *client         : pointer to mqtt client structure (mqtt_client_t).
 * @param  client_name     : Name of the mqtt client given by user.
 * @param  keep_alive_time : Keep Alive time for the client.
 * @retval size_t          : Length of connect message.
 */
size_t mqtt_connect(mqtt_client_t *client, char *client_name, uint16_t keep_alive_time)
{
	size_t  message_length     = 0;
	uint8_t client_name_length = 0;
	uint8_t user_name_length   = 0;
	char    *user_name;
	uint8_t password_length    = 0;
	char    *password;

	/* Check for client id length and truncate if greater than configuration specifed length */
	client_name_length = strlen(client_name);
	if(client_name_length > CLIENT_ID_LENGTH)
	{
		client_name_length = CLIENT_ID_LENGTH;
	}

	/* Fill mqtt  connect structure */
	client->connect_msg->fixed_header.message_type  = MQTT_CONNECT_MESSAGE;

	client->connect_msg->protocol_name_length = mqtt_htons(PROTOCOL_NAME_LENGTH);
	strcpy(client->connect_msg->protocol_name, PROTOCOL_NAME);

	client->connect_msg->protocol_version = PROTOCOL_VERSION;

	client->connect_msg->keep_alive_value = mqtt_htons(keep_alive_time);

	uint8_t new_payload_index;

	/* Populate payload message fields as per available payload options */
	if(client->connect_msg->connect_flags.user_name_flag)
	{
		/* Configure client ID and length */
		client->connect_msg->message_payload[0] = 0;
		client->connect_msg->message_payload[1] = client_name_length;
		strcpy(client->connect_msg->message_payload + CONNECT_CLIENT_ID_LENGTH_SIZE, client_name);

		user_name_length = strlen(client->connect_msg->payload_options.user_name);
		password_length = strlen(client->connect_msg->payload_options.password);


		/* Update index for user name and length details */
		new_payload_index = CONNECT_CLIENT_ID_LENGTH_SIZE + client_name_length;

		client->connect_msg->message_payload[new_payload_index] = 0;
		client->connect_msg->message_payload[new_payload_index + 1] = user_name_length;
		strcpy(client->connect_msg->message_payload + new_payload_index + CONNECT_USER_NAME_LENGTH_SIZE, client->connect_msg->payload_options.user_name);


		/* Update index for password and length details */
		new_payload_index += CONNECT_USER_NAME_LENGTH_SIZE + user_name_length;

		client->connect_msg->message_payload[new_payload_index] = 0;
		client->connect_msg->message_payload[new_payload_index + 1] = password_length;
		strcpy(client->connect_msg->message_payload + new_payload_index + 2, client->connect_msg->payload_options.password);

		/* Configure message length */
		message_length = (size_t)(FIXED_HEADER_LENGTH + CONNECT_PROTOCOL_LENGTH_SIZE + CONNECT_PROTOCOL_NAME_SIZE + CONNECT_PROTOCOL_VERSION_SIZE + CONNECT_FLAGS_SIZE + \
				CONNECT_KEEP_ALIVE_TIME_SIZE + CONNECT_CLIENT_ID_LENGTH_SIZE + client_name_length + CONNECT_USER_NAME_LENGTH_SIZE + CONNECT_PASSWORD_LENGTH_SIZE + \
				user_name_length + password_length);
	}
	else
	{
		/* Configure client ID and length */
		client->connect_msg->message_payload[0] = 0;
		client->connect_msg->message_payload[1] = client_name_length;
		strcpy(client->connect_msg->message_payload + CONNECT_CLIENT_ID_LENGTH_SIZE, client_name);

		/* Configure message length */
		message_length = (size_t)(FIXED_HEADER_LENGTH + CONNECT_PROTOCOL_LENGTH_SIZE + CONNECT_PROTOCOL_NAME_SIZE + CONNECT_PROTOCOL_VERSION_SIZE + CONNECT_FLAGS_SIZE + \
				CONNECT_KEEP_ALIVE_TIME_SIZE + CONNECT_CLIENT_ID_LENGTH_SIZE + client_name_length);
	}

	client->connect_msg->fixed_header.message_length = message_length - FIXED_HEADER_LENGTH;

    return message_length;
}




/*
 * @brief  Configures mqtt PUBLISH message options.
 * @param  *client        : pointer to mqtt client structure (mqtt_client_t).
 * @param  message_retain : Enable retain for message retention at broker
 * @param  message_qos    : Quality of service value (1:At-least once, 2:Exactly once)
 * @retval int8_t         : 1 = Success, -1 = Error
 */
int8_t mqtt_publish_options(mqtt_client_t *client, uint8_t message_retain, uint8_t message_qos)
{
	if(message_retain)
	{
		client->publish_msg->fixed_header.retain_flag = ENABLE;

	}

	/* Check if Quality of service value (qos) is less than reserved value (val:3) */
	if(message_qos < QOS_RESERVED)
	{
		client->publish_msg->fixed_header.qos_level = message_qos;
	}
	else
	{
		return func_opts_error;
	}

	return message_qos;
}




/*
 * @brief  Configures mqtt PUBLISH message structure.
 * @param  *client          : pointer to mqtt client structure (mqtt_client_t).
 * @param  *publish_topic   : publish topic name
 * @param  *publish_message : message to be published
 * @retval size_t           : length of PUBLISH control packet, retval = 0, if fail.
 */
size_t mqtt_publish(mqtt_client_t *client, char *publish_topic, char *publish_message)
{

	uint8_t message_length         = 0;
	uint8_t publish_topic_length   = 0;
	uint8_t payload_message_length = 0;

	publish_topic_length   = strlen(publish_topic);

	/*Check if quality of service is > 0 and accordingly adjust the length of publish message */
	if(client->publish_msg->fixed_header.qos_level > 0)
	{
		payload_message_length = strlen(publish_message) + MQTT_MESSAGE_ID_OFFSET;
	}
	else
	{
		payload_message_length = strlen(publish_message);
	}

	/* Check for overflow condition, if topic and message length is not greater than specified length */
	if(publish_topic_length > MQTT_TOPIC_LENGTH || payload_message_length > PUBLISH_PAYLOAD_LENGTH)
	{
		return func_param_len_error;
	}

	/* Fill main publish structure */
	client->publish_msg->fixed_header.message_type = MQTT_PUBLISH_MESSAGE;

	client->publish_msg->topic_length = mqtt_htons(MQTT_TOPIC_LENGTH);

	/* Copy topic to publish topic member */
	strncpy(client->publish_msg->topic_name, publish_topic, publish_topic_length);

	/* Copy message to publish pay load, insert message ID if quality of service > 0 */
	if(client->publish_msg->fixed_header.qos_level > 0)
	{

		client->publish_msg->payload[0] = 0;
		client->publish_msg->payload[1] = 1;

		strcpy(client->publish_msg->payload + MQTT_MESSAGE_ID_OFFSET, publish_message);
	}
	else
	{
		strncpy(client->publish_msg->payload, publish_message, payload_message_length);
	}

	/* TODO can be improved*/
	client->publish_msg->fixed_header.message_length = payload_message_length + TOPIC_LENGTH + sizeof(client->publish_msg->topic_length);

	message_length = client->publish_msg->fixed_header.message_length + FIXED_HEADER_LENGTH;

	return message_length;
}






/*
 * @brief  Configures mqtt DISCONNECT message structure.
 * @param  *client         : pointer to mqtt client structure (mqtt_client_t).
 * @retval size_t          : Length of disconnect message.
 */
size_t mqtt_disconnect(mqtt_client_t *client)
{

	size_t message_length = 0;

	client->disconnect_msg->fixed_header.message_type = MQTT_DISCONNECT_MESSAGE;

	message_length = sizeof(mqtt_disconnect_t);

	client->disconnect_msg->fixed_header.message_length = (uint8_t)(message_length - FIXED_HEADER_LENGTH);

	return message_length;
}





#if 0
static char *reverse_string(const char *string_value)
{
	int8_t i = 0, j = 0;           /*!< Loop Variables        */

	uint8_t     string_length;     /*!< Length of string      */
	static char *string_reversed;  /*!< Reversed string value */

	/*@brief check if string is not NULL, if yes, return NULL and exit */
	if(string_value == NULL)
	{
		return NULL;
	}

	/*@brief Calculate String Length and specify size of fixed memory allocator array */
	string_length = strlen(string_value);

	char fixed_mem_alloc[string_length];

	string_reversed = fixed_mem_alloc;

	memset(string_reversed, 0, string_length);

	/*@brief reverse string */
	for(i = string_length - 1; i >= 0; i--)
	{
		string_reversed[j++] = string_value[i];
	}


	return string_reversed;

}
#endif


