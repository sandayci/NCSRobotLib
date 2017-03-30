/*
 * webAEX libwebsockets-server -- webinterface for event-based hardware
 *
 * Author: Jonathan Binas
 *         <jbinas@gmail.com>
 *
 * Adapted from implementation by Andy Green <andy@warmcat.com>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation:
 *  version 2.1 of the License.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *  MA  02110-1301  USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <signal.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <errno.h>
#include <sys/types.h>
#include <stdbool.h>
#include <stdint.h>
#include <syslog.h>
#include <sys/time.h>
#include <unistd.h>
#include <dirent.h>

#include <libwebsockets.h>
#include <paex.h>
#include "lib/cjson/cJSON.h"
#include "lib/aerctl/aerctl.h"

static int close_testing;
int max_poll_elements;

struct pollfd *pollfds;
int *fd_lookup;
int count_pollfds;
static volatile int force_exit = 0;
static struct libwebsocket_context *context;


//specify different protocols supported by this server
enum protocols_enum {
	/* always first */
	PROTOCOL_HTTP = 0,

	PROTOCOL_DUMB_AER,

	/* always last */
	DEMO_PROTOCOL_COUNT
};


char *resource_path = LOCAL_RESOURCE_PATH;
char *build_path = BUILD_PATH;

/*
 * We take a strict whitelist approach to stop ../ attacks
 */

struct serveable {
	const char *urlpath;
	const char *mimetype;
}; 

struct per_session_data__http {
	int fd;
	DIR *dir;
	struct dirent *ent;
};

/*
 * this is just an example of parsing handshake headers, you don't need this
 * in your code unless you will filter allowing connections by the header
 * content
 */

static void
dump_handshake_info(struct libwebsocket *wsi)
{
	int n = 0;
	char buf[256];
	const unsigned char *c;

	do {
		c = lws_token_to_string(n);
		if (!c) {
			n++;
			continue;
		}

		if (!lws_hdr_total_length(wsi, n)) {
			n++;
			continue;
		}

		lws_hdr_copy(wsi, buf, sizeof buf, n);

		fprintf(stderr, "    %s = %s\n", (char *)c, buf);
		n++;
	} while (c);
}

const char * get_mimetype(const char *file)
{
	int n = strlen(file);

	if (n < 5)
		return NULL;

	if (!strcmp(&file[n - 4], ".ico"))
		return "image/x-icon";

	if (!strcmp(&file[n - 4], ".png"))
		return "image/png";

	if (!strcmp(&file[n - 5], ".html"))
		return "text/html";

	if (!strcmp(&file[n - 3], ".js"))
		return "text/javascript";

	if (!strcmp(&file[n - 5], ".json"))
		return "application/json";

	if (!strcmp(&file[n - 4], ".css"))
		return "text/css";

	return NULL;
}

/* this protocol server (always the first one) just knows how to do HTTP */

static int callback_http(struct libwebsocket_context *context,
		struct libwebsocket *wsi,
		enum libwebsocket_callback_reasons reason, void *user,
							   void *in, size_t len)
{
#if 0
	char client_name[128];
	char client_ip[128];
#endif
	char buf[256];
	char exp_path[256];
	char b64[64];
	struct timeval tv;
	int n, m;
	char *other_headers;
	static unsigned char buffer[4096];
	unsigned char *pw = &buffer[LWS_SEND_BUFFER_PRE_PADDING];
	unsigned char *p;
	struct stat stat_buf;
	struct per_session_data__http *pss =
			(struct per_session_data__http *)user;
	const char *mimetype;
	unsigned char *end;
	struct dirent *ent;
	switch (reason) {
	case LWS_CALLBACK_HTTP:

		dump_handshake_info(wsi);

		if (len < 1) {
			libwebsockets_return_http_status(context, wsi,
						HTTP_STATUS_BAD_REQUEST, NULL);
			return -1;
		}

		/* this example server has no concept of directories */
		if (strchr((const char *)in + 1, '/')) {
			libwebsockets_return_http_status(context, wsi,
						HTTP_STATUS_FORBIDDEN, NULL);
			return -1;
		}

		/* if a legal POST URL, let it continue and accept data */
		if (lws_hdr_total_length(wsi, WSI_TOKEN_POST_URI)) {
			//char *post_data[2048];
			//lws_body_copy(wsi, post_data, sizeof post_data);
			//printf("POST data: %s\n", post_data);
			return 0;
		}

		/* if list of bias files is requested, send raw data */
		if (!strcmp((const char *)in, "/_biases.json")) {
			if (strlen(resource_path) > sizeof(exp_path) - 12)
				return -1;
			sprintf(exp_path, "%s/biases",resource_path);

			p = buffer + LWS_SEND_BUFFER_PRE_PADDING;
			end = p + sizeof(buffer) - LWS_SEND_BUFFER_PRE_PADDING;
			pss->dir = opendir(exp_path);

			if (pss->dir == NULL) {
				printf("ERROR could not open bias directory\n");
				return -1;
			}

			if (lws_add_http_header_status(context, wsi, 200, &p, end))
				return 1;
			if (lws_add_http_header_by_token(context, wsi, WSI_TOKEN_HTTP_SERVER, (unsigned char *)"libwebsockets", 13, &p, end))
				return 1;
			if (lws_add_http_header_by_token(context, wsi, WSI_TOKEN_HTTP_CONTENT_TYPE, (unsigned char *)"application/json", 16, &p, end))
				return 1;
			//if (lws_add_http_header_content_length(context, wsi,stat_buf.st_size, &p, end))
			//	return 1;
			if (lws_finalize_http_header(context, wsi, &p, end))
				return 1;

			/*
			 * send the http headers...
			 * this won't block since it's the first payload sent
			 * on the connection since it was established
			 * (too small for partial)
			 * 
			 * Notice they are sent using LWS_WRITE_HTTP_HEADERS
			 * which also means you can't send body too in one step,
			 * this is mandated by changes in HTTP2
			 */

			n = libwebsocket_write(wsi,
			       buffer + LWS_SEND_BUFFER_PRE_PADDING,
			       p - (buffer + LWS_SEND_BUFFER_PRE_PADDING),
			       LWS_WRITE_HTTP_HEADERS);

			if (n < 0) {
				closedir(pss->dir);
				return -1;
			}
			/* book us a LWS_CALLBACK_HTTP_WRITEABLE callback */
			libwebsocket_callback_on_writable(context, wsi);
			break;
		}

		/* if list of experiment files is requested, send raw data */
		if (!strcmp((const char *)in, "/_experiments.json")) {
			if (strlen(resource_path) > sizeof(exp_path) - 17)
				return -1;
			sprintf(exp_path, "%s/experiments",resource_path);

			p = buffer + LWS_SEND_BUFFER_PRE_PADDING;
			end = p + sizeof(buffer) - LWS_SEND_BUFFER_PRE_PADDING;
			pss->dir = opendir(exp_path);

			if (pss->dir == NULL) {
				printf("ERROR could not open experiments directory\n");
				return -1;
			}

			if (lws_add_http_header_status(context, wsi, 200, &p, end))
				return 1;
			if (lws_add_http_header_by_token(context, wsi, WSI_TOKEN_HTTP_SERVER, (unsigned char *)"libwebsockets", 13, &p, end))
				return 1;
			if (lws_add_http_header_by_token(context, wsi, WSI_TOKEN_HTTP_CONTENT_TYPE, (unsigned char *)"application/json", 16, &p, end))
				return 1;
			//if (lws_add_http_header_content_length(context, wsi,stat_buf.st_size, &p, end))
			//	return 1;
			if (lws_finalize_http_header(context, wsi, &p, end))
				return 1;

			/*
			 * send the http headers...
			 * this won't block since it's the first payload sent
			 * on the connection since it was established
			 * (too small for partial)
			 * 
			 * Notice they are sent using LWS_WRITE_HTTP_HEADERS
			 * which also means you can't send body too in one step,
			 * this is mandated by changes in HTTP2
			 */

			n = libwebsocket_write(wsi,
			       buffer + LWS_SEND_BUFFER_PRE_PADDING,
			       p - (buffer + LWS_SEND_BUFFER_PRE_PADDING),
			       LWS_WRITE_HTTP_HEADERS);

			if (n < 0) {
				closedir(pss->dir);
				return -1;
			}
			/* book us a LWS_CALLBACK_HTTP_WRITEABLE callback */
			libwebsocket_callback_on_writable(context, wsi);
			break;
		}

		/* if bias file is requested, rewrite URL */
		strcpy(buf, resource_path);
		if (strstr((const char *)in + 1, "_biases_") == (const char *)in + 1) {
			strcat(buf, "/biases/");
			if (sizeof in < 1) {
			    printf("ERROR invalid bias file name\n");
			    return -1; }
			else
			    strncat(buf, (const char *)in + 9, sizeof(buf)-strlen(resource_path));
		}

		/* if not, send a file the easy way */
		else if (strcmp(in, "/")) {
			strcat(buf, "/static");
			if (*((const char *)in) != '/')
				strcat(buf, "/");
			strncat(buf, in, sizeof(buf) - strlen(resource_path));
		} else /* default file to serve */
			strcat(buf, "/static/index.html");
		buf[sizeof(buf) - 1] = '\0';
		printf("rewritten url: %s\n", (const char *)buf);

                /* refuse to serve files we don't understand */
                mimetype = get_mimetype(buf);
                if (!mimetype) {
                        lwsl_err("Unknown mimetype for %s\n", buf);
                        libwebsockets_return_http_status(context, wsi,
                                      HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE, NULL);
                        return -1;
                }

                other_headers = NULL;
                n = 0;

		if (libwebsockets_serve_http_file(context, wsi, buf,
						mimetype, other_headers, n))
			/* through completion or error, close the socket */
			return -1; 

		/*
		 * notice that the sending of the file completes asynchronously,
		 * we'll get a LWS_CALLBACK_HTTP_FILE_COMPLETION callback when
		 * it's done
		 */

		break;

	case LWS_CALLBACK_HTTP_BODY:
		strncpy(buf, in, 20);
		buf[20] = '\0';
		if (len < 20)
			buf[len] = '\0';

		lwsl_notice("LWS_CALLBACK_HTTP_BODY: %s... len %d\n",
				(const char *)buf, (int)len);

		break;

	case LWS_CALLBACK_HTTP_BODY_COMPLETION:
		lwsl_notice("LWS_CALLBACK_HTTP_BODY_COMPLETION\n");
		/* the whole of the sent body arried, close the connection */
		libwebsockets_return_http_status(context, wsi,
						HTTP_STATUS_OK, NULL);

		return -1;

	case LWS_CALLBACK_HTTP_FILE_COMPLETION:
//		lwsl_info("LWS_CALLBACK_HTTP_FILE_COMPLETION seen\n");
		/* kill the connection after we sent one file */
		return -1;

	case LWS_CALLBACK_HTTP_WRITEABLE:
		/*
		 * we can send more of whatever it is we were sending
		 */
		if (pss->dir != NULL) {
		    //obtain list of files
		    cJSON *exp_files_root, *exp_files;
		    exp_files_root = cJSON_CreateObject();
		    cJSON_AddItemToObject(exp_files_root, "files", exp_files=cJSON_CreateArray());
		    while ((pss->ent = readdir(pss->dir)) != NULL) {
		    	if (strcmp(pss->ent->d_name, ".") &&
			    strcmp(pss->ent->d_name, "..")) {
				cJSON_AddItemToArray(
					exp_files,
					cJSON_CreateString(pss->ent->d_name)
				);
			}
		    }
		    char * flist_rendered = cJSON_Print(exp_files_root);
		    closedir(pss->dir);
		    p = buffer + LWS_SEND_BUFFER_PRE_PADDING;
		    if (sizeof flist_rendered > sizeof buffer - LWS_SEND_BUFFER_PRE_PADDING) {
		        printf("ERROR list of files too long");
		        return -1;
		    }
		    n = sprintf((char *)p, (const char *)flist_rendered);
		    if (n > 0) {
		    	m = libwebsocket_write(wsi, p, n, LWS_WRITE_HTTP);
		    	if (m < n)
		    	    lwsl_err("ERROR %d writing to di socket\n", n);
		    }
		    return -1; //close conn in any case
		}

		/* send normal file */
		do {
		    n = read(pss->fd,buffer+LWS_SEND_BUFFER_PRE_PADDING,
			sizeof (buffer)-LWS_SEND_BUFFER_PRE_PADDING);
		    /* problem reading, close conn */
		    if (n < 0)
		    	goto bail;
		    /* sent it all, close conn */
		    if (n == 0)
		    	goto flush_bail;
		    /* To support HTTP2, must take care of preamble space */
		    m = libwebsocket_write(wsi,
		    	       buffer + LWS_SEND_BUFFER_PRE_PADDING,
		    	       n, LWS_WRITE_HTTP);
		    if (m < 0)
		    	/* write failed, close conn */
		    	goto bail;
		    if (m != n)
		    	/* partial write, adjust */
		    	/* currently, this case is not handled if list of files is sent */
		    	lseek(pss->fd, m - n, SEEK_CUR);

		    if (m) /* while still active, extend timeout */
		    	libwebsocket_set_timeout(wsi,
		    		PENDING_TIMEOUT_HTTP_CONTENT, 5);
		    
		    /* if he has indigestion, let him clear it before eating more */
		    if (lws_partial_buffered(wsi))
		    	break;

		} while (!lws_send_pipe_choked(wsi));
		libwebsocket_callback_on_writable(context, wsi);
		break;
flush_bail:
		/* true if still partial pending */
		if (lws_partial_buffered(wsi)) {
			libwebsocket_callback_on_writable(context, wsi);
			break;
		}
bail:
		close(pss->fd);
		return -1;

	/*
	 * callback for confirming to continue with client IP appear in
	 * protocol 0 callback since no websocket protocol has been agreed
	 * yet.  You can just ignore this if you won't filter on client IP
	 * since the default uhandled callback return is 0 meaning let the
	 * connection continue.
	 */

	case LWS_CALLBACK_FILTER_NETWORK_CONNECTION:
#if 0
		libwebsockets_get_peer_addresses(context, wsi, (int)(long)in, client_name,
			     sizeof(client_name), client_ip, sizeof(client_ip));

		fprintf(stderr, "Received network connect from %s (%s)\n",
							client_name, client_ip);
#endif
		/* if we returned non-zero from here, we kill the connection */
		break;

	case LWS_CALLBACK_GET_THREAD_ID:
		/*
		 * if you will call "libwebsocket_callback_on_writable"
		 * from a different thread, return the caller thread ID
		 * here so lws can use this information to work out if it
		 * should signal the poll() loop to exit and restart early
		 */

		/* return pthread_getthreadid_np(); */

		break;

	default:
		break;
	}

	return 0;
}


/* dumb_aer protocol */

/*
 * one of these is auto-created for each connection and a pointer to the
 * appropriate instance is passed to the callback in the user parameter
 *
 * for this example protocol we use it to individualize the count for each
 * connection.
 */

struct per_session_data__dumb_aer {
	int poll_no;
	int active;
	int mute;
	uint32_t rd_addr;
	uint32_t ev_addr;
	uint32_t t_prev;
	uint32_t binsize;
	uint32_t aer_wlock; //currently only implemented as per-session var
};

static int
callback_dumb_aer(struct libwebsocket_context *context,
			struct libwebsocket *wsi,
			enum libwebsocket_callback_reasons reason,
			void *user, void *in, size_t len)
{
	int n = 0, m = 0, k = 0;
	unsigned char buf[LWS_SEND_BUFFER_PRE_PADDING + 1024 +
			  LWS_SEND_BUFFER_POST_PADDING];
	unsigned char *p = &buf[LWS_SEND_BUFFER_PRE_PADDING];
	unsigned char *pw = &buf[LWS_SEND_BUFFER_PRE_PADDING];
	struct per_session_data__dumb_aer *pss = (struct per_session_data__dumb_aer *)user;
	char rx_str[255];
	char *rx_param;
	char *rx_param_val;
	Bias bias = {.id = -1, .coarse = -1, .fine = -1, .high = -1, .type = -1, .cascode = -1, .enable = -1};
	Synapse synapse = {.recurrent = ENABLE, .source = 0, .target = 0, .mode = 0, .wgt1 = 0, .wgt2 = 0, .type = NONPLASTIC};
	struct timeval tv;
	uint32_t ms;


	switch (reason) {

	case LWS_CALLBACK_ESTABLISHED:
		lwsl_info("callback_dumb_aer: " "LWS_CALLBACK_ESTABLISHED\n");
		gettimeofday(&tv, NULL);
		ms = (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
		pss->active = 1;
		pss->mute = 0;
		pss->poll_no = 0;
		pss->rd_addr = aerFindHead();
		pss->ev_addr = 0;
		pss->t_prev = ms;
		pss->binsize = 10;
		pss->aer_wlock = 0;
		break;

	case LWS_CALLBACK_SERVER_WRITEABLE:
		if (!pss->active) {
			break;
		}
		gettimeofday(&tv, NULL);
		ms = (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
		if ((ms - pss->t_prev) < pss->binsize)
			break;
		else
			pss->t_prev = ms;
		for (k = 0; k < 1024/4; k++) {
			pss->poll_no++;
			pss->ev_addr = aerReadMem(2*pss->rd_addr);
			if (pss->ev_addr != 0xffffffff) {
				pss->rd_addr = (pss->rd_addr+1)%AER_BUFFER_SIZE;
				n += sprintf((char *)pw, "%03x;", pss->ev_addr);
				pw += 4; //increment buffer pointer
					 //use 3 byte encoding
			}
			else //end batch if no more events are available
				break;
		}
		if (n > 0) {
			m = libwebsocket_write(wsi, p, n, LWS_WRITE_TEXT);
			if (m < n) {
				lwsl_err("ERROR %d writing to di socket\n", n);
				return -1;
			}
		}
		break;

	case LWS_CALLBACK_RECEIVE:
		if (!len ||
		    !strstr((const char *)in, "method") ||
		    !strstr((const char *)in, "params"))
		    break;
		cJSON *rx_root = cJSON_Parse((const char*)in);
		if (!rx_root ||
		    !cJSON_GetObjectItem(rx_root, "method") ||
		    !cJSON_GetObjectItem(rx_root, "params")) {
			printf("ERROR: Could not parse JSON.\n");
			break; }
		char *method = cJSON_GetObjectItem(rx_root, "method")->valuestring;
		cJSON *params = cJSON_GetObjectItem(rx_root, "params");
		if (strcmp(method, "setConn")) //reset printing if not setconn
		    pss->mute = 0;
		if (!pss->mute)
		    printf("RPC method: %s\n", (const char *)method);
		else if (pss->mute == 1000) {
		    pss->mute = 1;
		    printf(".\n", (const char *)method);
		}

		if (!strcmp(method, "pauseStream")) {
		    //pause
		    pss->active = 0;
		}

		if (!strcmp(method, "resetBuffer")) {
		    //reset
		    pss->rd_addr = aerFindHead();
		    pss->poll_no = 0;
		    pss->active = 1;
		}

		if (!strcmp(method, "resetConn")) {
		    if (pss->aer_wlock == 1) {
						printf("Error resetting: device input locked\n");
						break;
				}
		    //reset all connections
		    pss->aer_wlock = 1;
				printf("Resetting NPA.\n");
		    aerResetAll(NONPLASTIC);
				printf("Resetting PA.\n");
		    aerResetAll(PLASTIC);
				printf("Done resetting.\n");
		    pss->aer_wlock = 0;
		}

		if (!strcmp(method, "setBinsize")) {
		    //change binsize
		    pss->binsize = cJSON_GetObjectItem(params, "ms")->valueint;
		}

		if (!strcmp(method, "setBias")) {
		    if (pss->aer_wlock == 1) {
						printf("Error setting bias: device input locked\n");
						break;
				}
		    //set bias
		    bias.id = cJSON_GetObjectItem(params, "biasId")->valueint;
		    bias.coarse = cJSON_GetObjectItem(params, "coarseCurrent")->valueint;
		    bias.fine = cJSON_GetObjectItem(params, "fineCurrent")->valueint;
		    bias.high = cJSON_GetObjectItem(params, "highBias")->valueint;
		    bias.type = cJSON_GetObjectItem(params, "nBias")->valueint;
		    bias.cascode = 0;
		    bias.enable = 1;
		    aerSetBias(&bias);
		}

		if (!strcmp(method, "saveBiases")) {
		    //save all biases
		    char *fname = cJSON_GetObjectItem(params, "fileName")->valuestring;
				if (strstr((const char*)fname, "/")) {
					printf("Error saving file: Invalid filename\n");
					break; }
				char fpath [255];
		    strcpy(fpath, (const char *)resource_path);
		    strcat(fpath, "/biases/");
		    strcat(fpath, fname);
		    char *fcontent = cJSON_Print(params);
		    FILE *f = fopen(fpath, "w");
		    if (!f) {
		        printf("Error opening file!\n");
						break; }
		    printf(fcontent);
		    fprintf(f, "%s\n", fcontent);
		    fclose(f);
		}

		if (!strcmp(method, "setConn")) {
		    if (pss->aer_wlock == 1) {
						printf("Error setting connection: device input locked\n");
						break;
				}
		    pss->mute += 1;
		    synapse.source = cJSON_GetObjectItem(params, "connSource")->valueint;
		    synapse.target = cJSON_GetObjectItem(params, "connTarget")->valueint;
		    synapse.mode = cJSON_GetObjectItem(params, "connMode")->valueint;
		    synapse.wgt1 = cJSON_GetObjectItem(params, "connWgt1")->valueint;
		    synapse.wgt2 = cJSON_GetObjectItem(params, "connWgt2")->valueint;
		    synapse.type = cJSON_GetObjectItem(params, "connType")->valueint; 
		    aerSetConn(&synapse);
		}

		cJSON_Delete(rx_root);

		break;
	/*
	 * this just demonstrates how to use the protocol filter. If you won't
	 * study and reject connections based on header content, you don't need
	 * to handle this callback
	 */

	case LWS_CALLBACK_FILTER_PROTOCOL_CONNECTION:
		dump_handshake_info(wsi);
		/* you could return non-zero here and kill the connection */
		break;

	default:
		break;
	}

	return 0;
}



/* list of supported protocols and callbacks */

static struct libwebsocket_protocols protocols[] = {
	/* first protocol must always be HTTP handler */

	{
		"http-only",		/* name */
		callback_http,		/* callback */
		sizeof (struct per_session_data__http),	/*per_session_data_siz*/
		0,			/* max frame size / rx buffer */
	},
	{
		"dumb-aer-protocol",
		callback_dumb_aer,
		sizeof(struct per_session_data__dumb_aer),
		8192,
	},
	{ NULL, NULL, 0, 0 } /* terminator */
};

void sighandler(int sig)
{
	force_exit = 1;
	libwebsocket_cancel_service(context);
}

static struct option options[] = {
	{ "help",	no_argument,		NULL, 'h' },
	{ "debug",	required_argument,	NULL, 'd' },
	{ "port",	required_argument,	NULL, 'p' },
	{ "ssl",	no_argument,		NULL, 's' },
	{ "allow-non-ssl",	no_argument,		NULL, 'a' },
	{ "interface",  required_argument,	NULL, 'i' },
	{ "closetest",  no_argument,		NULL, 'c' },
	{ "libev",  no_argument,		NULL, 'e' },
#ifndef LWS_NO_DAEMONIZE
	{ "daemonize", 	no_argument,		NULL, 'D' },
#endif
	{ "resource_path", required_argument,		NULL, 'r' },
	{ NULL, 0, 0, 0 }
};

int main(int argc, char **argv)
{
	char cert_path[1024];
	char key_path[1024];
	int n = 0;
	int use_ssl = 0;
	int opts = 0;
	char interface_name[128] = "";
	const char *iface = NULL;
	unsigned int ms, oldms, oldms_long = 0;
	struct lws_context_creation_info info;

	int debug_level = 7;
#ifndef LWS_NO_DAEMONIZE
	int daemonize = 0;
#endif

	memset(&info, 0, sizeof info);
	info.port = 9000;

	while (n >= 0) {
		n = getopt_long(argc, argv, "eci:hsap:d:Dr:", options, NULL);
		if (n < 0)
			continue;
		switch (n) {
		case 'e':
			opts |= LWS_SERVER_OPTION_LIBEV;
			break;
#ifndef LWS_NO_DAEMONIZE
		case 'D':
			daemonize = 1;
			break;
#endif
		case 'd':
			debug_level = atoi(optarg);
			break;
		case 's':
			use_ssl = 1;
			break;
		case 'a':
			opts |= LWS_SERVER_OPTION_ALLOW_NON_SSL_ON_SSL_PORT;
			break;
		case 'p':
			info.port = atoi(optarg);
			break;
		case 'i':
			strncpy(interface_name, optarg, sizeof interface_name);
			interface_name[(sizeof interface_name) - 1] = '\0';
			iface = interface_name;
			break;
		case 'c':
			close_testing = 1;
			fprintf(stderr, " Close testing mode -- closes on "
					   "client after 50 dumb increments"
					   "and suppresses lws_mirror spam\n");
			break;
		case 'r':
			resource_path = optarg;
			printf("Setting resource path to \"%s\"\n", resource_path);
			break;
		case 'h':
			fprintf(stderr, "Usage: server "
					"[--port=<p>] [--ssl] "
					"[-d <log bitfield>] "
					"[--resource_path <path>]\n");
			exit(1);
		}
	}

	//open device
	aerOpenDevice();

	//set biases
        printf("Setting biases...\n");
	char fpath [255];
	strcpy(fpath, (const char *)build_path);
	strcat(fpath, "/data/_biases_default.datx");
        BGSetFromFile(fpath);
        printf("Done.\n");


#if !defined(LWS_NO_DAEMONIZE)
	/* 
	 * normally lock path would be /var/lock/lwsts or similar, to
	 * simplify getting started without having to take care about
	 * permissions or running as root, set to /tmp/.lwsts-lock
	 */
	if (daemonize && lws_daemonize("/tmp/.lwsts-lock")) {
		fprintf(stderr, "Failed to daemonize\n");
		return 1;
	}
#endif

	signal(SIGINT, sighandler);


	/* tell the library what debug level to emit and to send it to syslog */
	lws_set_log_level(debug_level, lwsl_emit_syslog);

	lwsl_notice("websockets AER server by Jonathan Binas <jbinas@gmail.com>"
		    "licensed under LGPL2.1\n");
	info.iface = iface;
	info.protocols = protocols;
#ifndef LWS_NO_EXTENSIONS
	info.extensions = libwebsocket_get_internal_extensions();
#endif
	//ssl is not supported in this version
	info.ssl_cert_filepath = NULL;
	info.ssl_private_key_filepath = NULL;
	info.gid = -1;
	info.uid = -1;
	info.options = opts;

	context = libwebsocket_create_context(&info);
	if (context == NULL) {
		lwsl_err("libwebsocket init failed\n");
		return -1;
	}

	n = 0;
	while (n >= 0 && !force_exit) {
		struct timeval tv;
		gettimeofday(&tv, NULL);

		/*
		 * This provokes the LWS_CALLBACK_SERVER_WRITEABLE for every
		 * live websocket connection using the DUMB_INCREMENT protocol,
		 * as soon as it can take more packets (usually immediately)
		 */

		ms = (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
		if ((ms - oldms) > 1) {
			libwebsocket_callback_on_writable_all_protocol(&protocols[PROTOCOL_DUMB_AER]);
			oldms = ms;
		}

		/*
		 * If libwebsockets sockets are all we care about,
		 * you can use this api which takes care of the poll()
		 * and looping through finding who needed service.
		 *
		 * If no socket needs service, it'll return anyway after
		 * the number of ms in the second argument.
		 */

		n = libwebsocket_service(context, 1); //return after 1ms
	}

	libwebsocket_context_destroy(context);

	lwsl_notice("libwebsockets-server exited cleanly\n");


	return 0;
}
