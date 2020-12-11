#include "socket.h"

int ftp_connect(char* ip, int port){
	int fd;
	char response[255];
	struct sockaddr_in address;

	bzero((char*)&address,sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr(ip);
	address.sin_port = htons(port);

	fd = socket(AF_INET, SOCK_STREAM, 0);

	if(fd < 0)
	{
		return -1;
	}

	if(connect(fd, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		fprintf(stderr, "Could not connect to server!\n");	
		return -2;
	}

    if((ftp_response(fd, response)) != SOCKET_READY)
    {
        return -3;
    }

	return fd;
}

int ftp_response(int fd, char* response){
	FILE* file = fdopen(fd, "r");

	char* buffer;
	size_t size = 0;
	int total = 0, code;

	while (getline(&buffer, &size, file) > 0) {
		strncat(response, buffer, total - 1);
		total += size;

		if (buffer[3] == ' ') {
			sscanf(buffer, "%d", &code);
			break;
		}
    }

    printf("< %s\n", response);

    return total < 1 ? -1 : code;
}

int ftp_command(int fd, char* raw_command, char* args) {
	char command[255];

	strcpy(command, raw_command);

	if (args != NULL) {
		strcat(command, " ");
		strcat(command, args);
	}

	strcat(command, "\r\n");

	int size;

    if ((size = write(fd, command, strlen(command))) != strlen(command)) 
    {
        return -1;
    }

	printf("> %s", command);

	if(size < 0)
	{
		return -1;
	}

	return 0;
}

int parse_pasv_port(char* response, char* ip){
	strtok(response, "(");
    char* ips = strtok(NULL, ")");

	int port[2], temp_ip[4];
	sscanf(ips, "%d,%d,%d,%d,%d,%d", &temp_ip[0], &temp_ip[1], &temp_ip[2], &temp_ip[3], &port[0], &port[1]);

	sprintf(ip, "%d.%d.%d.%d", temp_ip[0], temp_ip[1], temp_ip[2], temp_ip[3]);
	
	return port[0] * 256 + port[1];
}

int ftp_login(int fd, char* user, char* password){
	char response[255];
	
	if(ftp_command(fd, "user", user) < 0)
	{
		return -1;
	}

	if(ftp_response(fd, response) != USERNAME_OK)
	{
		return -2;
	}

	if(ftp_command(fd, "pass", password) < 0)
	{
		return -3;
	}

	if(ftp_response(fd, response) != LOGIN_OK)
	{
		return -4;
	}

	return 0;
}