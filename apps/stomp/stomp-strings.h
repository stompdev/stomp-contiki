#ifndef STOMP_STRINGS_H
#define	STOMP_STRINGS_H

#define STOMP_NULL 0x00
#define STOMP_NEW_LINE 0x0a
#define STOMP_COLON 0x3a

extern const char stomp_command_abort[6];
extern const char stomp_command_ack[4];
extern const char stomp_command_begin[6];
extern const char stomp_command_commit[7];
extern const char stomp_command_connect[8];
extern const char stomp_command_connected[10];
extern const char stomp_command_disconnect[11];
extern const char stomp_command_error[6];
extern const char stomp_command_message[8];
extern const char stomp_command_nack[5];
extern const char stomp_command_receipt[8];
extern const char stomp_command_send[5];
extern const char stomp_command_stomp[6];
extern const char stomp_command_subscribe[10];
extern const char stomp_command_unsubscribe[12];

extern const char stomp_header_accept_version[15];
extern const char stomp_header_ack[4];
extern const char stomp_header_amq_msg_type[13];
extern const char stomp_header_auto[5];
extern const char stomp_header_client[7];
extern const char stomp_header_client_id[10];
extern const char stomp_header_content_length[15];
extern const char stomp_header_content_type[13];
extern const char stomp_header_correlation_id[15];
extern const char stomp_header_destination[12];
extern const char stomp_header_expires[8];
extern const char stomp_header_host[5];
extern const char stomp_header_id[3];
extern const char stomp_header_client_individual[18];
extern const char stomp_header_login[6];
extern const char stomp_header_message_id[11];
extern const char stomp_header_message[8];
extern const char stomp_header_original_destination[21];
extern const char stomp_header_passcode[9];
extern const char stomp_header_persistent[11];
extern const char stomp_header_priority[9];
extern const char stomp_header_receipt_id[11];
extern const char stomp_header_receipt[8];
extern const char stomp_header_redelivered[12];
extern const char stomp_header_reply_to[9];
extern const char stomp_header_request_id[11];
extern const char stomp_header_response_id[12];
extern const char stomp_header_selector[9];
extern const char stomp_header_session[8];
extern const char stomp_header_subscription[13];
extern const char stomp_header_timestamp[10];
extern const char stomp_header_transaction[12];
extern const char stomp_header_transformation_error[21];
extern const char stomp_header_transformation[15];
extern const char stomp_header_type[5];
extern const char stomp_header_jmsxuserid[11];

#endif	/* STOMP_STRINGS_H */