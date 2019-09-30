/**
 ******************************************************************************
 * @file    mqtt_client.c
 * @author  Aditya Mall,
 * @brief   Example MQTT publish client, for mosquitto MQTT Broker
 *
 *  Info
 *          MQTT API Source File
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
#include "mqtt_client.h"



/******************************************************************************/
/*                                                                            */
/*                  Data Structures and Defines                               */
/*                                                                            */
/******************************************************************************/


/* @brief Defines for MQTT control packet Variable sizes */
#define CONNECT_PROTOCOL_LENGTH_SIZE   2                       /*!< */
#define CONNECT_PROTOCOL_NAME_SIZE     PROTOCOL_NAME_LENGTH    /*!< */
#define CONNECT_PROTOCOL_VERSION_SIZE  1                       /*!< */
#define CONNECT_FLAGS_SIZE             1                       /*!< */
#define CONNECT_KEEP_ALIVE_TIME_SIZE   2                       /*!< */
#define CONNECT_CLIENT_ID_LENGTH_SIZE  2                       /*!< */
#define CONNECT_USER_NAME_LENGTH_SIZE  2                       /*!< */
#define CONNECT_PASSWORD_LENGTH_SIZE   2                       /*!< */
#define PUBLISH_TOPIC_LENGTH_SIZE      2                       /*!< */


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

		client->connect_msg->payload_options.user_name_length = mqtt_htons(user_name_length);
		strcpy(client->connect_msg->payload_options.user_name, user_name);

		client->connect_msg->payload_options.password_length  = mqtt_htons(password_length);
		strcpy(client->connect_msg->payload_options.password, password);

	}

	return func_opts_success;
}



/*
 * @brief  Configures mqtt connect options. (qos and retain don't have any effect on control packets currently)
 * @param  *client : pointer to mqtt client structure (mqtt_client_t).
 * @param  session : configure session type
 * @param  qos     : configure quality of service
 * @param  retain  : configure mention retention at broker.
 * @retval int8_t  : 1 = Success, -1 = Error
 */
int8_t mqtt_connect_options(mqtt_client_t *client, uint8_t session, uint8_t qos, uint8_t retain)
{
	/* Check for correct values of session, retain and qos(quality of service) */
	if(session > 1 || retain > 1 || qos > QOS_RESERVED)
	{
		return func_opts_error;
	}

	client->connect_msg->connect_flags.clean_session = session;

	/* Enable will flag and if qos and retain are enabled by user */
	if(qos || retain)
	{
		client->connect_msg->connect_flags.will_qos    = qos;
		client->connect_msg->connect_flags.will_retain = retain;
		client->connect_msg->connect_flags.will_flag   = ENABLE;
	}
	else
	{
		client->connect_msg->connect_flags.will_qos    = qos;
		client->connect_msg->connect_flags.will_retain = retain;
		client->connect_msg->connect_flags.will_flag   = DISABLE;
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
size_t mqtt_connect(mqtt_client_t *client, char *client_name, int16_t keep_alive_time)
{
	size_t  message_length     = 0;
	uint8_t client_name_length = 0;
	uint8_t user_name_length   = 0;
	uint8_t password_length    = 0;
	uint8_t payload_index      = 0;

	size_t func_retval;

	/* TODO Implement error checks in mqtt connect */
	if(keep_alive_time < 0 || client_name == NULL || client == NULL)
	{
		func_retval = (size_t)func_param_len_error;
	}
	else
	{
		/* Check for client id length and truncate if greater than configuration specified length */
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


		/*
		 * @brief Populate payload message fields as per available payload options,
		 *        and append the index of payload array as per options.
		 */
		if(client->connect_msg->connect_flags.user_name_flag)
		{
			/* Configure client ID and length */
			client->connect_msg->message_payload[0] = 0;
			client->connect_msg->message_payload[1] = client_name_length;
			strncpy(client->connect_msg->message_payload + CONNECT_CLIENT_ID_LENGTH_SIZE, client_name, client_name_length);

			user_name_length = strlen(client->connect_msg->payload_options.user_name);
			password_length = strlen(client->connect_msg->payload_options.password);

			/* Update index for user name and length details */
			payload_index = CONNECT_CLIENT_ID_LENGTH_SIZE + client_name_length;

			client->connect_msg->message_payload[payload_index]     = 0;
			client->connect_msg->message_payload[payload_index + 1] = user_name_length;
			strncpy(client->connect_msg->message_payload + payload_index + CONNECT_USER_NAME_LENGTH_SIZE, client->connect_msg->payload_options.user_name, user_name_length);


			/* Update index for password and length details */
			payload_index += CONNECT_USER_NAME_LENGTH_SIZE + user_name_length;

			client->connect_msg->message_payload[payload_index]     = 0;
			client->connect_msg->message_payload[payload_index + 1] = password_length;
			strncpy(client->connect_msg->message_payload + payload_index + 2, client->connect_msg->payload_options.password, password_length);

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
			strncpy(client->connect_msg->message_payload + CONNECT_CLIENT_ID_LENGTH_SIZE, client_name, client_name_length);

			/* Configure message length */
			message_length = (size_t)(FIXED_HEADER_LENGTH + CONNECT_PROTOCOL_LENGTH_SIZE + CONNECT_PROTOCOL_NAME_SIZE + CONNECT_PROTOCOL_VERSION_SIZE + CONNECT_FLAGS_SIZE + \
					CONNECT_KEEP_ALIVE_TIME_SIZE + CONNECT_CLIENT_ID_LENGTH_SIZE + client_name_length);

		}

		client->connect_msg->fixed_header.message_length = message_length - FIXED_HEADER_LENGTH;

		func_retval = message_length;
	}

	return func_retval;
}



/*
 * @brief  Returns the value of message type from input buffer.
 * @param  *client  : pointer to mqtt client structure (mqtt_client_t).
 * @retval  uint8_t : value of message type.
 */
uint8_t get_mqtt_message_type(mqtt_client_t *client)
{
	return client->message->message_type;
}



/*
 * @brief  Returns the value of connack message status from input buffer.
 * @param  *client  : pointer to mqtt client structure (mqtt_client_t).
 * @retval  uint8_t : connack message return code.
 */
uint8_t get_connack_status(mqtt_client_t *client)
{
	uint8_t message_state;

	/* If connection accepted then publish message */
	if(client->connack_msg->return_code == MQTT_CONNECTION_ACCEPTED)
	{
		message_state = MQTT_PUBLISH_MESSAGE;
	}
	else
	{
		message_state = MQTT_DISCONNECT_MESSAGE;
	}

	return message_state;
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
 * @retval size_t           : length of publish control packet, fail = 0;
 */
size_t mqtt_publish(mqtt_client_t *client, char *publish_topic, char *publish_message)
{

	uint8_t message_length         = 0;
	uint8_t publish_topic_length   = 0;
	uint8_t publish_message_length = 0;
	uint8_t payload_index          = 0;


	publish_topic_length = strlen(publish_topic);


	/*Check if quality of service is > 0 and accordingly adjust the length of publish message */
	if(client->publish_msg->fixed_header.qos_level > 0)
	{
		publish_message_length = strlen(publish_message) + MQTT_MESSAGE_ID_OFFSET;
	}
	else
	{
		publish_message_length = strlen(publish_message);
	}


	/* Check for overflow condition, if topic and message length is not greater than specified length */
	if(publish_topic_length > MQTT_TOPIC_LENGTH || publish_message_length > PUBLISH_PAYLOAD_LENGTH)
	{
		return func_param_len_error;
	}

	/* Fill main publish structure */
	client->publish_msg->fixed_header.message_type = MQTT_PUBLISH_MESSAGE;

	client->publish_msg->topic_length = mqtt_htons(publish_topic_length);


	/* Copy message to publish pay load, insert message ID if quality of service > 0 */
	if(client->publish_msg->fixed_header.qos_level > 0)
	{

		/* Copy topic to publish topic member */
		strncpy(client->publish_msg->payload, publish_topic, publish_topic_length);

		/* Update pay load index*/
		payload_index = publish_topic_length;

		/* Configure topic length */
		client->publish_msg->payload[payload_index]     = 0;
		client->publish_msg->payload[payload_index + 1] = 1;

		/* Copy pay-load message */
		strcpy(client->publish_msg->payload + payload_index + MQTT_MESSAGE_ID_OFFSET, publish_message);
	}
	else
	{
		/* Copy topic to publish topic member */
		strncpy(client->publish_msg->payload, publish_topic, publish_topic_length);

		strncpy(client->publish_msg->payload + publish_topic_length, publish_message, publish_message_length);
	}

	/* Configure Message Length */
	message_length = publish_message_length + publish_topic_length + PUBLISH_TOPIC_LENGTH_SIZE + FIXED_HEADER_LENGTH;

	client->publish_msg->fixed_header.message_length = message_length - FIXED_HEADER_LENGTH;

	return message_length;
}



/*
 * @brief  Configures mqtt PUBREL message structure.
 * @param  *client         : pointer to mqtt client structure (mqtt_client_t).
 * @retval size_t          : Length of publish release message.
 */
size_t mqtt_publish_release(mqtt_client_t *client)
{

	size_t message_length = 0;

	client->pubrel_msg->fixed_header.qos_level   = QOS_ATLEAST_ONCE;
	client->pubrel_msg->fixed_header.message_type = MQTT_PUBREL_MESSAGE;

	client->pubrel_msg->message_id = mqtt_htons(1);

	message_length = sizeof(mqtt_pubrel_t);

	client->pubrel_msg->fixed_header.message_length = (uint8_t)(message_length - FIXED_HEADER_LENGTH);

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



size_t mqtt_subscribe(mqtt_client_t *client, char *subscribe_topic, uint8_t qos)
{
	size_t message_length = 0;

	uint8_t subscribe_topic_length = 0;

	subscribe_topic_length = (uint8_t)strlen(subscribe_topic);

	if(client == NULL)
	{
		return 0;
	}

	client->subscribe_msg->fixed_header.message_type = MQTT_SUBSCRIBE_MESSAGE;
	client->subscribe_msg->fixed_header.qos_level = qos;

	client->subscribe_msg->message_identifier = mqtt_htons(0x01);

	client->subscribe_msg->topic_length = mqtt_htons(subscribe_topic_length);

	strncpy(client->subscribe_msg->payload, subscribe_topic, subscribe_topic_length);

	client->subscribe_msg->payload[subscribe_topic_length] = qos;

	message_length = FIXED_HEADER_LENGTH + 2 + 2 + subscribe_topic_length + 1;

	client->subscribe_msg->fixed_header.message_length = message_length - FIXED_HEADER_LENGTH;

	return message_length;
}


