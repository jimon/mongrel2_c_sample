// mongrel2 usage sample
// Dmitry Ivanov, 2013

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <zmq.h>
#include <jsmn.h>

// place your sub and pub ip's here
const char * sub_adress = "tcp://xxx.xxx.xxx.xxx:9999";
const char * pub_adress = "tcp://xxx.xxx.xxx.xxx:9998";

// place your handler uuid here (can be just some random number)
const char * handler_uuid = "550e8400-e29b-41d4-a716-446655440000";

typedef enum
{
	http_req_get,
	http_req_put,
	http_req_post,
	http_req_head,
	http_req_delete
} mongrel2_msg_type_t;

typedef struct
{
	char * uuid;
	char * id;
	char * path;
	char * json;
	size_t json_size;
	char * payload;
	size_t payload_size;
	char * query;

	mongrel2_msg_type_t type;
} mongrel2_msg_t;

int8_t parse_msg(mongrel2_msg_t * msg, char * buffer);
size_t print_msg_dbg(char * buffer, mongrel2_msg_t * msg);
void send_response(void * socket, mongrel2_msg_t * msg, uint16_t http_code, const char * payload, size_t payload_size);

int main(void)
{
	printf("mongrel2 sample\n");

	void * context = zmq_init(1);
	void * sock_request = zmq_socket(context, ZMQ_UPSTREAM);
	void * sock_response = zmq_socket(context, ZMQ_PUB);

	zmq_connect(sock_request, sub_adress);
	zmq_connect(sock_response, pub_adress);
	zmq_setsockopt(sock_response, ZMQ_IDENTITY, handler_uuid, strlen(handler_uuid));

	printf("init ok\n");

	while(true)
	{
		printf("waiting\n");

		zmq_msg_t msg;
		zmq_msg_init(&msg);
		zmq_recv(sock_request, &msg, 0);

		mongrel2_msg_t request_msg;
		if(parse_msg(&request_msg, (char*)zmq_msg_data(&msg)) < 0)
		{
			printf("invalid client\n");
			continue;
		}

		char temp[2 * 1024];
		print_msg_dbg(temp, &request_msg);

		printf("%s", temp);

		send_response(sock_response, &request_msg, 200, temp, strlen(temp));
	}

	zmq_close(sock_request);
	zmq_close(sock_response);
	zmq_term(context);

	_getch();

	return 0;
}

int8_t parse_msg(mongrel2_msg_t * msg, char * buffer)
{
	// examples what you can receive from mongrel2
	//char raw[] = "54c6755b-9628-40a4-9a2d-cc82a816345e 1 /handler 614:{\"PATH\":\"/handler\",\"x-forwarded-for\":\"93.73.33.73\",\"loool\":\"100\",\"origin\":\"chrome-extension://hgmloofddffdnphfgcellkdfbfbjeloo\",\"content-type\":\"application/x-www-form-urlencoded\",\"accept-language\":\"en-US,en;q=0.8,ru;q=0.6\",\"accept-encoding\":\"gzip,deflate,sdch\",\"connection\":\"keep-alive\",\"content-length\":\"9\",\"accept\":\"\",\"user-agent\":\"Mozilla/5.0 (Windows NT 6.2; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/31.0.1650.63 Safari/537.36\",\"host\":\"anlcs0.dgs.tatemgames.com:6767\",\"cookie\":\"PHPSESSID=34faa8a913042a21249e0fbce60eaa7a\",\"METHOD\":\"POST\",\"VERSION\":\"HTTP/1.1\",\"URI\":\"/handler\",\"PATTERN\":\"/handler/\"},9:asdzxccvb,";
	//char raw[] = "54c6755b-9628-40a4-9a2d-cc82a816345e 1 /handler 563:{\"PATH\":\"/handler\", \"x-forwarded-for\":\"93.73.33.73\", \"cache-control\":\"max-age=0\", \"accept-language\":\"en-US,en;q=0.8,ru;q=0.6\", \"accept-encoding\":\"gzip,deflate,sdch\", \"connection\":\"keep-alive\", \"accept\":\"text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,;q=0.8\", \"user-agent\":\"Mozilla/5.0 (Windows NT 6.2; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/31.0.1650.63 Safari/537.36\", \"host\":\"anlcs0.dgs.tatemgames.com:6767\", \"cookie\":\"PHPSESSID=34faa8a913042a21249e0fbce60eaa7a\", \"METHOD\":\"GET\", \"VERSION\":\"HTTP/1.1\", \"URI\":\"/handler\", \"PATTERN\":\"/handler/\"},0:,";

	// first we parse uuid, id, path
	char * cursor = buffer;

	msg->uuid = cursor;
	char * find = strstr(cursor, " ");
	*find = '\0';
	cursor = find + 1;

	msg->id = cursor;
	find = strstr(cursor, " ");
	*find = '\0';
	cursor = find + 1;

	msg->path = cursor;
	find = strstr(cursor, " ");
	*find = '\0';
	cursor = find + 1;

	find = strstr(cursor, ":");
	*find = '\0';

	// then findout json and payload with netstrings
	msg->json = find + 1;
	msg->json_size = strtoull(cursor, NULL, 0);

	*(msg->json + msg->json_size) = '\0';
	cursor = msg->json + msg->json_size + 1;
	find = strstr(cursor, ":");
	*find = '\0';

	msg->payload = find + 1;
	msg->payload_size = strtoull(cursor, NULL, 0);

	*(msg->payload + msg->payload_size) = '\0';

	msg->query = NULL;

	// then check if we got "client disconnected" payload
	const char * disconnect_body = "{\"type\":\"disconnect\"}";
	size_t disconnect_body_length = strlen(disconnect_body);

	if((msg->payload_size == disconnect_body_length) && (strncmp(msg->payload, disconnect_body, disconnect_body_length) == 0))
		return -1;

	// then parse json
	jsmn_parser parser;
	jsmn_init(&parser);

	jsmntok_t tokens[512];
	if(jsmn_parse(&parser, msg->json, tokens, 512) != JSMN_SUCCESS)
		return -2;

	if(tokens[0].type == JSMN_OBJECT)
	{
		const size_t child_tokens = tokens[0].size;

		if(child_tokens % 2 != 0)
			return -3; // invalid json format

		const char * str_method = "METHOD";
		const char * str_query = "QUERY";
		const char * str_get = "GET";
		const char * str_put = "PUT";
		const char * str_post = "POST";
		const char * str_head = "HEAD";
		const char * str_delete = "DELETE";

		size_t str_method_length = strlen(str_method);
		size_t str_query_length = strlen(str_query);
		size_t str_get_length = strlen(str_get);
		size_t str_put_length = strlen(str_put);
		size_t str_post_length = strlen(str_post);
		size_t str_head_length = strlen(str_head);
		size_t str_delete_length = strlen(str_delete);

		size_t i = 1;
		while(i < child_tokens + 1)
		{
			if(tokens[i].type != JSMN_STRING)
				return -4; // invalid json format

			size_t key_length = tokens[i].end - tokens[i].start;
			char * key = msg->json + tokens[i].start;

			if((key_length == str_method_length) && (strncmp(key, str_method, str_method_length) == 0))
			{
				key_length = tokens[i + 1].end - tokens[i + 1].start;
				key = msg->json + tokens[i + 1].start;

				if((key_length == str_get_length) && (strncmp(key, str_get, str_get_length) == 0))
					msg->type = http_req_get;
				else if((key_length == str_put_length) && (strncmp(key, str_put, str_put_length) == 0))
					msg->type = http_req_put;
				else if((key_length == str_post_length) && (strncmp(key, str_post, str_post_length) == 0))
					msg->type = http_req_post;
				else if((key_length == str_head_length) && (strncmp(key, str_head, str_head_length) == 0))
					msg->type = http_req_head;
				else if((key_length == str_delete_length) && (strncmp(key, str_delete, str_delete_length) == 0))
					msg->type = http_req_delete;
				else
					return -5; // invalid method
			}
			else if((key_length == str_query_length) && (strncmp(key, str_query, str_query_length) == 0))
			{
				key_length = tokens[i + 1].end - tokens[i + 1].start;
				key = msg->json + tokens[i + 1].start;

				msg->query = key;
				*(msg->query + key_length) = '\0';
			}

			i += 2;
		}
	}

	return 0;
}

size_t print_msg_dbg(char * buffer, mongrel2_msg_t * msg)
{
	size_t printed = sprintf(buffer, "recv :\n");
	printed += sprintf(buffer + printed, "uuid: %s\nid: %s\npath: %s\n", msg->uuid, msg->id, msg->path);
	printed += sprintf(buffer + printed, "json_size: %u\njson: %s\n", msg->json_size, msg->json);
	printed += sprintf(buffer + printed, "payload_size: %u\npayload: %s\n", msg->payload_size, msg->payload);

	printed += sprintf(buffer + printed, "method: ");
	switch(msg->type)
	{
	case http_req_get: printed += sprintf(buffer + printed, "get"); break;
	case http_req_put: printed += sprintf(buffer + printed, "put"); break;
	case http_req_post: printed += sprintf(buffer + printed, "post"); break;
	case http_req_head: printed += sprintf(buffer + printed, "head"); break;
	case http_req_delete: printed += sprintf(buffer + printed, "delete"); break;
	default: printed += sprintf(buffer + printed, "invalid"); break;
	}
	printed += sprintf(buffer + printed, "\n");

	printed += sprintf(buffer + printed, "query: %s\n", msg->query);
	return printed;
}

char response_buffer[256 * 1024];

void send_response(void * socket, mongrel2_msg_t * msg, uint16_t http_code, const char * payload, size_t payload_size)
{
	size_t printed_size = sprintf(response_buffer, "%s %u:%s, HTTP/1.1 %u\r\nContent-Length: %u\r\n\r\n",
			msg->uuid,
			strlen(msg->id),
			msg->id,
			http_code,
			payload_size);

	if(payload_size)
		memcpy(response_buffer + printed_size, payload, payload_size);

	*(response_buffer + printed_size + payload_size) = '\0';

	zmq_msg_t msg_send;
	zmq_msg_init_data(&msg_send, response_buffer, printed_size + payload_size + 1, NULL, NULL);
	zmq_send(socket, &msg_send, 0);
}
