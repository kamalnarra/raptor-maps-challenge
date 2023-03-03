// This function creates a socket, binds it to a port, and listens for incoming connections.
// It takes a pointer to a struct containing server arguments as a parameter.
int create_socket(struct server_arguments *args){

    // Create a socket of the specified type.
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // Set up the server address structure.
    struct sockaddr_in server_address;
    bzero(&server_address, sizeof(server_address)); 
    server_address.sin_family = AF_INET; 
    server_address.sin_addr.s_addr = INADDR_ANY; 
    server_address.sin_port = htons(args->port); 

    // Bind the socket to the specified address and port.
    bind(sockfd, (struct sockaddr*)&server_address, sizeof(server_address)); 

    // Start listening for incoming connections on the socket.
    listen(sockfd, 5); 

    // Accept an incoming connection and create a new socket for it.
    struct sockaddr_in client_address;
    socklen_t client_address_len = sizeof(client_address);
    int client_sockfd = accept(sockfd, (struct sockaddr*)&client_address, &client_address_len);

    // Return the new socket file descriptor to the calling function.
    return client_sockfd;
}


// This function initializes a client socket by receiving data from it.
// It takes a file descriptor for the client socket as a parameter.
int initialize(int client_sockfd){

    // Create a buffer to store the received data.
    unsigned char buffer[8];

    // Receive data from the client socket and store it in the buffer.
    recv(client_sockfd, buffer, 8, 0);

    // Extract the type and length values from the received data.
    int type, length;
    memcpy(&type, buffer, 4); 
    memcpy(&length, buffer + 4, 4); 

    // Convert the type and length values from network byte order to host byte order.
    type = ntohl(type);
    length = ntohl(length);

    // Return the length value to the calling function.
    return length;
}

// This function performs a hash request for a specified number of messages from a client socket.
// It takes the client socket file descriptor, the salt value, the length of the salt value, and the number of hash requests as parameters.
void hash_request(int client_sockfd, char* salt, int salt_len, int hashRequestNum){

	// Loop through the specified number of hash requests.
	for (int i = 0; i < hashRequestNum; i++) {

		// Create variables to store the message type and length.
		int type, length;

		// Create buffers to store the ID and message data.
		unsigned char id_buffer[8];
		unsigned char buffer_for_hash[4096];
		unsigned char actual_hash[32];

		// Zero out the buffers.
		bzero(buffer_for_hash, 4096);
		bzero(actual_hash, 32);

		// Receive the ID buffer from the client socket.
		recv(client_sockfd, id_buffer, 8, 0);

		// Extract the message type and length from the ID buffer.
		memcpy(&type, id_buffer, 4);
		memcpy(&length, id_buffer + 4, 4);
		type = ntohl(type);

		// Check that the message type is correct.
		if(type != 3){
			perror("message failed");
			exit(EXIT_FAILURE);
		}

		// Convert the message length from network byte order to host byte order.
		length = ntohl(length);
		
		// Create a buffer to store the message data and receive it from the client socket.
		unsigned char message[length];
    	recv(client_sockfd, message, length, 0);

		// Create a checksum context and compute the hash.
		struct checksum_ctx* checksum = checksum_create((const unsigned char *)salt, salt_len);
		memcpy(buffer_for_hash, message, length);
		checksum_finish(checksum,(const unsigned char *) buffer_for_hash, length, actual_hash);

		// Create a response buffer and populate it with the hash data.
		char response[40];
		type = htonl(4);
		int index = htonl(i);

		memcpy(
			response,
			&type, sizeof(int)
		);
		memcpy(
			response + sizeof(int),
			&index, 4
		);
		memcpy(
			response + 4 + 4,
			actual_hash, 32
		);

		// Send the response buffer to the client socket.
		send(client_sockfd, response, 40, 0);
	}
}

//Sends an acknowledgement message to the client
void acknowledgement(int hashRequestNum, int client_sockfd, char* salt, size_t salt_len){
    // Calculate the size of the response buffer
    int request_in_bytes = hashRequestNum * 40;

    // Create a buffer for the acknowledgement message
    unsigned char new_buffer[8];

    // Initialize the acknowledgement message fields
    int code = htonl(2);
    int hashnum = htonl(request_in_bytes);

    // Copy the fields into the acknowledgement buffer
    memcpy(new_buffer, &code, 4);
    memcpy(new_buffer + 4, &hashnum, 4);

    // Send the acknowledgement message to the client
    send(client_sockfd, new_buffer, 8, 0);

    // Process the hash requests from the client
    hash_request(client_sockfd, salt, salt_len, hashRequestNum);
}



int main(int argc, char *argv[]) {
    struct server_arguments args;
    server_parseopt(argc, argv, &args); 
    int client_sockfd = create_socket(&args);
    int hashRequestNum = initialize(client_sockfd);
    acknowledgement(hashRequestNum, client_sockfd, args.salt, args.salt_len);
}