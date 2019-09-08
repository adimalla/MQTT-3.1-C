/**
 ******************************************************************************
 * @file    mqtt_client.h
 * @author  Aditya Mall,
 * @brief   MQTT publish client API Header File for mosquitto MQTT Broker
 *
 *  Info
 *          MQTT API Header File
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

#ifndef MQQT_CLIENT_H_
#define MQQT_CLIENT_H_

/*
 * Standard Header and API Header files
 */
#include <stdint.h>
#include "mqtt_configs.h"


/******************************************************************************/
/*                                                                            */
/*                       Macros and Defines                                   */
/*                                                                            */
/******************************************************************************/


/* Avoid Structure padding */
#pragma pack(1)


/* State Machine defines */
#define FSM_RUN                   1               /*!< Value of Run state of finite state machine     */
#define FSM_SUSPEND               0               /*!< Value of Suspend state of finite state machine */
#define READ_STATE                16              /*!< Value of Read state of finite state machine    */
#define IDLE_STATE                17              /*!< Value of Idle state of finite state machine    */
#define EXIT_STATE                18              /*!< Value of Exit state of finite state machine    */


/* MQTT Header Options */
#define MQTT_MESSAGE_RETAIN       1               /*!< Retain mqtt message at server/broker       */
#define MQTT_MESSAGE_NO_RETAIN    0               /*!< Do not retain message at server/broker     */
#define QOS_FIRE_FORGET           0               /*!< Quality of service: at most once delivery  */
#define QOS_ATLEAST_ONCE          1               /*!< Quality of service: at least once delivery */
#define QOS_EXACTLY_ONCE          2               /*!< Quality of service: exactly once delivery  */
#define QOS_RESERVED              3               /*!< Reserved bit, for future additions         */


/* @brief Defines for CONNECT Message */
#define MQTT_CONNECT_MESSAGE      1               /*!< MQTT Connect message bit identifier value */
/* TODO add defines for connect options */


/* @brief Defines for CONNACK Message */
#define MQTT_CONNACK_MESSAGE      2               /*!< MQTT Connack message bit identifier value */
#define MQTT_CONNECTION_ACCEPTED  0               /*!< Connack message accepted identifier value */
#define MQTT_CONNECTION_REFUSED   2               /*!< Connack message refused identifier value  */


/* @brief Defines for PUBLISH Message */
#define MQTT_PUBLISH_MESSAGE      3               /*!< MQTT Publish message bit identifier value             */
#define MQTT_TOPIC_LENGTH         TOPIC_LENGTH    /*!< Publish message topic length, mqtt_configs.h          */
#define PUBLISH_PAYLOAD_LENGTH    MESSAGE_LENGTH  /*!< Publish message payload message length,mqtt_configs.h */
#define MQTT_MESSAGE_ID_OFFSET    2               /*!< Publish message, message ID length offset value       */


/* Defines for PUBACK Message */
#define MQTT_PUBACK_MESSAGE       4               /*!< MQTT Puback message bit identifier value */


/* @brief Defines for Disconnect */
#define MQTT_DISCONNECT_MESSAGE   14              /*!< MQTT Publish message bit identifier value */




/******************************************************************************/
/*                                                                            */
/*                  Data Structures for MQTT Control Messages                 */
/*                                                                            */
/******************************************************************************/



/* @brief MQTT Fixed Header, common to all control messages */
typedef struct mqtt_header
{
	uint8_t retain_flag     : 1;  /*!< Message Retain Flag, (LSB)                                              */
	uint8_t qos_level       : 2;  /*!< Quality of service Level, (LSB)                                         */
	uint8_t dup_flag        : 1;  /*!< Duplicate Flag, (LSB)                                                   */
	uint8_t message_type    : 4;  /*!< MQTT Control Packet / Message Type, (MSB)                               */
	uint8_t message_length;       /*!< Length of MQTT Message (Remaining Length, excludes fixed header length) */

}mqtt_header_t;



/* @brief MQTT CONNECT structures */

/* Connect Flags bit fields */
typedef struct mqtt_connect_flags
{
	uint8_t reserved        : 1;  /*!< Reserved not set                                 */
	uint8_t clean_session   : 1;  /*!< Clean / New session, previous history is deleted */
	uint8_t will_flag       : 1;  /*!< Flag for enabling the qos and retain options     */
	uint8_t will_qos        : 2;  /*!< Enable qos (quality of service) option           */
	uint8_t will_retain     : 1;  /*!< Enable retain option                             */
	uint8_t password_flag   : 1;  /*!< Enable password option                           */
	uint8_t user_name_flag  : 1;  /*!< Enable user name option                          */

}mqtt_connect_flags_t;


/* TODO test username password with broker*/

/* Main MQTT Connect Structure */
typedef struct mqtt_connect
{
	mqtt_header_t        fixed_header;                         /*!< MQTT Fixed Header                                 */
	uint16_t             protocol_name_length;                 /*!< MQTT Protocol Name Length                         */
	char                 protocol_name[PROTOCOL_NAME_LENGTH];  /*!< MQTT Protocol Name,length defined in configs.h    */
	uint8_t              protocol_version;                     /*!< MQTT Protocol Version                             */
	mqtt_connect_flags_t connect_flags;                        /*!< Connect Message Flags                             */
	uint16_t             keep_alive_value;                     /*!< Client Keep Alive Value                           */
	uint16_t             client_id_length;                     /*!< Client ID Length                                  */
	char                 client_id[CLIENT_ID_LENGTH];          /*!< Client ID, length defined in mqtt_config.h        */
	uint16_t             user_name_length;                     /*!< Client User Name Length                           */
	char                 user_name[USER_NAME_LENGTH];          /*!< Client User Name, length defined in mqtt_config.h */
	uint16_t             password_length;                      /*!< Client Password Length                            */
	char                 password[PASSWORD_LENGTH];            /*!< Client Password, length defined in mqtt_config.h  */

}mqtt_connect_t;



/* @brief MQTT CONNACK structure */
typedef struct mqtt_connack
{
	mqtt_header_t fixed_header;  /*!< MQTT Fixed Header            */
	uint8_t       Reserved;      /*!< Reserved                     */
	uint8_t       return_code;   /*!< Connack Message return codes */

}mqtt_connack_t;



/* @brief MQTT disconnect structure */
typedef struct mqtt_disconnect
{
	mqtt_header_t fixed_header;  /*!< MQTT Fixed Header */

}mqtt_disconnect_t;



/* @brief MQTT PUBLISH structure */
typedef struct mqtt_publish
{
	mqtt_header_t fixed_header;              /*!< MQTT Fixed Header            */
	uint16_t      topic_length;              /*!< Publish message topic length */
	char          topic_name[TOPIC_LENGTH];  /*!< Publish message topic name   */
	char          payload[MESSAGE_LENGTH];   /*!< publish message payload      */

}mqtt_publish_t;



/* @brief MQTT client handle structure */
typedef struct mqtt_client_handle
{
	mqtt_header_t     *message;         /*!< Pointer to the fixed header structure       */
	mqtt_connect_t    *connect_msg;     /*!< Pointer to the connect message structure    */
	mqtt_connack_t    *connack_msg;     /*!< Pointer to the connack message structure    */
	mqtt_publish_t    *publish_msg;     /*!< Pointer to the publish message structure    */
	mqtt_disconnect_t *disconnect_msg;  /*!< Pointer to the disconnect message structure */

}mqtt_client_t;



/* @brief MQTT message types for State Machine */
enum mqtt_message_states
{
	mqtt_idle_state       = IDLE_STATE,               /*!< State machine Idle State               */
	mqtt_read_state       = READ_STATE,               /*!< State machine Read State               */
	mqtt_connect_state    = MQTT_CONNECT_MESSAGE,     /*!< Connect message send state             */
	mqtt_connack_state    = MQTT_CONNACK_MESSAGE,     /*!< Connack message return code read state */
	mqtt_publish_state    = MQTT_PUBLISH_MESSAGE,     /*!< Publish message send state             */
	mqtt_puback_state     = MQTT_PUBACK_MESSAGE,      /*!< Puback message read state              */
	mqtt_disconnect_state = MQTT_DISCONNECT_MESSAGE,  /*!< Diconect message send state            */
	mqtt_exit_state       = EXIT_STATE,               /*!< State machine exit state               */
};




/******************************************************************************/
/*                                                                            */
/*                       API Function Prototypes                              */
/*                                                                            */
/******************************************************************************/



/*
 * @brief  Configures mqtt client user name and password.
 * @param  *client   : pointer to mqtt client structure (mqtt_client_t).
 * @param  user_name : mqtt client user name
 * @param  password  : mqtt client password
 * @retval int8_t    : 1 = Success, -1 = Error
 */
int8_t mqtt_client_username_passwd(mqtt_client_t *client, char *user_name, char *password);



/*
 * @brief  Configures mqtt CONNECT message structure.
 * @param  *client         : pointer to mqtt client structure (mqtt_client_t).
 * @param  client_name     : Name of the mqtt client given by user.
 * @param  keep_alive_time : Keep Alive time for the client.
 * @retval size_t          : Length of connect message.
 */
size_t mqtt_connect(mqtt_client_t *client, char *client_name, uint16_t keep_alive_time);



/*
 * @brief  Configures mqtt PUBLISH message options.
 * @param  *client        : pointer to mqtt client structure (mqtt_client_t).
 * @param  message_retain : Enable retain for message retention at broker
 * @param  message_qos    : Quality of service value (1:At-least once, 2:Exactly once)
 * @retval int8_t         : 1 = Success, -1 = Error
 */
int8_t mqtt_publish_options(mqtt_client_t *client, uint8_t message_retain, uint8_t message_qos);



/*
 * @brief  Configures mqtt PUBLISH message structure.
 * @param  *client          : pointer to mqtt client structure (mqtt_client_t).
 * @param  *publish_topic   : publish topic name
 * @param  *publish_message : message to be published
 * @retval size_t           : length of PUBLISH control packet.
 */
size_t mqtt_publish(mqtt_client_t *client, char *publish_topic, char *publish_message);



/*
 * @brief  Configures mqtt DISCONNECT message structure.
 * @param  *client : pointer to mqtt client structure (mqtt_client_t).
 * @retval size_t  : Length of disconnect message.
 */
size_t mqtt_disconnect(mqtt_client_t *client);



#endif /* MQQT_CLIENT_H_ */
