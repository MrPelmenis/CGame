#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080

int main() {
	printf("\n\n laizu serveri \n\n");

	int server_fd, new_socket;
	ssize_t valread;
	struct sockaddr_in address;
	int opt = 1;
	socklen_t addrlen = sizeof(address);
	char buffer[1024] = { 0 };
	char* hello = "suutu arpakal";

	if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("socket nesagaja");
		return 1;
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);


	if((bind(server_fd, (struct sockaddr*)&address, sizeof(address))) < 0){
		perror("bind fail uj uj");
		return 1;
	}

	if(listen(server_fd, 3) < 0){
		perror("listen fail ajaj");
		return 1;
	}

	while (1) {
	    printf("1111111\n");

	    if ((new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen)) < 0) {
	        perror("accept failing D:");
	        continue;
	    }

	    printf("2222\n");

	    while (1) { // Keep the connection open for the client
	        valread = read(new_socket, buffer, sizeof(buffer) - 1);
	        if (valread <= 0) {
	            if (valread == 0) {
	                printf("Client disconnected\n");
	            } else {
	                perror("read failed");
	            }
	            close(new_socket);
	            break;
	        }

	        buffer[valread] = '\0';
	        printf("Client sent: %s\n", buffer);

	        if (send(new_socket, hello, strlen(hello), 0) < 0) {
	            perror("send failed");
	            close(new_socket);
	            break;
	        }
	        printf("Sent response to client\n");
	    }
	}

	close(server_fd);
	return 0;
}
