#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "shared.h"

// info about where we are listening/connecting
const char* RECV_ADDRESS = "localhost";
const char* SEND_ADDRESS = "localhost";  
const char* RECV_PORT_NUMBER = "5998";  
const char* SEND_PORT_NUMBER = "5999";
const int BACKLOG = 1; //number of incoming connections allowed
const int BUFFER_SIZE = 512; //max msg length allowed
char* user = "ani1: ";

void* receive_symmetric();
void* send_symmetric();

int main(){
	pthread_t receive_thread;
	int return_code = pthread_create(&receive_thread, NULL, receive_symmetric, NULL);
	if (return_code){
		FATAL("could not start receive_thread");
	}
	
	//necessary for chat_mary to initialize
	sleep(10);
	
	pthread_t send_thread;
	return_code = pthread_create(&send_thread, NULL, send_symmetric, NULL);
	if (return_code){
		FATAL("could not start send_thread");
	}

 	pthread_join(receive_thread, NULL);
 	pthread_join(send_thread, NULL);
	
	return 0;
}

/**
 * receive()
 */
void* receive_symmetric(){
	TRACE("Entering %s \n", __func__);
	struct addrinfo* serv_info;
	struct addrinfo hints;
	
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
	//hints.ai_flags = AI_PASSIVE;     // fill in my IP for me
	
	TRACE("%s - calling getaddrinfo \n", __func__);
	int status = getaddrinfo(RECV_ADDRESS, RECV_PORT_NUMBER, &hints, &serv_info);
	if (status){
		ERROR("cannot get address info for %s:  %s. \n", RECV_ADDRESS, gai_strerror(status));
		exit(1);
	}
	
	//TODO: don't assume the first item in the list is valid
	// look for a valid entry in serv_info
	TRACE("%s - calling socket \n", __func__);
	int listen_sock = 
		socket(serv_info->ai_family, serv_info->ai_socktype, serv_info->ai_protocol);
	if (listen_sock == -1){
        FATAL("cannot open listen socket: %s\n", strerror(errno));
	}
	
	TRACE("%s - calling bind \n", __func__);
	status = bind (listen_sock, serv_info->ai_addr, serv_info->ai_addrlen);
	if (status == -1){
        FATAL("cannot open connect socket: %s\n", strerror(errno));
	}
	
	TRACE("%s - calling listen \n", __func__);
	status = listen (listen_sock, BACKLOG);
	if (status == -1){
        FATAL("cannot open listen socket: %s\n", strerror(errno));
	}
	
	struct sockaddr_storage incoming_addr;
	size_t incoming_addr_size = sizeof(incoming_addr);
	TRACE("%s - calling accept \n", __func__);
	int connect_sock = 
		accept(listen_sock, (struct sockaddr*) &incoming_addr, &incoming_addr_size);
	if (connect_sock == -1){
        FATAL("cannot accept: %s\n", strerror(errno));
	}
	
	while(1){
			//TRACE("%s - calling recv \n", __func__);
			char msg[BUFFER_SIZE];
			int bytes_received = recv(connect_sock, msg, sizeof msg, 0);
			if (bytes_received == -1){
    	    	FATAL("cannot recv: %s\n", strerror(errno));
			}
			if (bytes_received == 0){
				TRACE("%s - connection closed by remote client \n", __func__);
				break;
			}
	
			if (bytes_received >= BUFFER_SIZE){
				msg[BUFFER_SIZE-1]  = 0;
			} 
			else {
				msg[bytes_received] = 0;
			}
			printf("%s", msg);
	}
	
	close(connect_sock);
	
	// clean up
	close(listen_sock);
	freeaddrinfo(serv_info);
}

/**
 * send()
 */
void* send_symmetric(){
	TRACE("entering \n", __func__);
	struct addrinfo* serv_info;
	struct addrinfo hints;
	
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
	
	TRACE("%s - calling getaddrinfo \n", __func__);
	int status = getaddrinfo(SEND_ADDRESS, SEND_PORT_NUMBER, &hints, &serv_info);
	// See man getaddrinfo.
	if (status){
		ERROR("cannot get address info for %s:  %s. \n", SEND_ADDRESS, gai_strerror(status));
		exit(1);
	}
	
	TRACE("%s - calling socket \n", __func__);
	int connect_sock = socket(serv_info->ai_family, serv_info->ai_socktype, serv_info->ai_protocol);
	
	if (connect_sock == -1){
        FATAL("cannot open connect socket: %s\n", strerror(errno));
	}
 
	TRACE("%s - calling connect \n", __func__);
	status = connect (connect_sock, serv_info->ai_addr, serv_info->ai_addrlen);
	
	if (status == -1) {
        FATAL("cannot connect: %s\n", strerror(errno));
	}
	
	while (1){
		//printf("me: ");
		char msg[BUFFER_SIZE];
		fgets(msg, BUFFER_SIZE, stdin);
		
		size_t bytes_read = strlen(msg);
		size_t user_length = strlen(user);
		
		//TODO: fix sizes
		char outgoing_msg[BUFFER_SIZE];
		strcpy(outgoing_msg, user);
		strcat(outgoing_msg, msg);
		
		//TODO: error checking
		
		//TRACE("msg is - %s", msg);

		size_t length = strlen(outgoing_msg);
		int bytes_sent = send(connect_sock, outgoing_msg, length, 0);
	
		if (status == -1){
        	FATAL("cannot send: %s\n", strerror(errno));
		}
	}
	
	// clean up
	close(connect_sock);
	freeaddrinfo(serv_info);
}