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
/*                              API Functions                                 */
/*                                                                            */
/******************************************************************************/



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
 * @retval uint8_t   : 1 = Success, 0 = Failure
 */
uint8_t mqtt_client_username_passwd(mqtt_client_t *client, char *user_name, char *password)
{
	/* check if user name is not null */
	if( strlen(user_name) == 0 )
	{
		return 0;
	}

	/* check if user name and passsword doesn't exceed defined length, if yes return 0 */
	if( strlen(user_name) > USER_NAME_LENGTH || strlen(password) > PASSWORD_LENGTH)
	{
		return 0;
	}
	else
	{
		client->connect_msg->connect_flags.user_name_flag = ENABLE;
		client->connect_msg->connect_flags.password_flag  = ENABLE;

		strcpy(client->connect_msg->user_name, user_name);
		strcpy(client->connect_msg->password, password);

		client->connect_msg->user_name_length = mqtt_htons(USER_NAME_LENGTH);
		client->connect_msg->password_length = mqtt_htons(PASSWORD_LENGTH);
	}

	return 1;
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
	size_t message_length;

	client->connect_msg->fixed_header.message_type  = MQTT_CONNECT_MESSAGE;

	client->connect_msg->protocol_name_length 	    = mqtt_htons(PROTOCOL_NAME_LENGTH);

	strcpy(client->connect_msg->protocol_name, PROTOCOL_NAME);

	client->connect_msg->protocol_version = PROTOCOL_VERSION;
	client->connect_msg->keep_alive_value = mqtt_htons(keep_alive_time);
	client->connect_msg->client_id_length = mqtt_htons(CLIENT_ID_LENGTH);

	strcpy(client->connect_msg->client_id, client_name);

	/* @brief calculate message length, fixed (Can be a constant) */
	message_length = sizeof(mqtt_connect_t);

	client->connect_msg->fixed_header.message_length = (uint8_t)( (message_length - FIXED_HEADER_LENGTH) );

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


