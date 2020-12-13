#include "download.h"

int main(int argc, char * argv[])
{
    int ret;
    char** fields = malloc(sizeof(char*)*FIELDS_SIZE);

    if (argc != 2) {
        fprintf(stderr, "usage: download ftp://[<user>:<password>@]<host>/<url-path>\n");
        return -1;
    }


    if ((ret = paserArgs(argv[1], fields)) < 0) {
        fprintf(stderr, "Could not parse url! errno: %d\n", ret);
        return ret;
    }

    if((ret = download(fields)) < 0)
    {
        fprintf(stderr, "Could not download file! errno: %d\n", ret);
        return ret;
    }

    free(fields);
    return 0;
}

int download(char* fields[]){
    int socket_fd, port, data_fd, file_fd, size, errno;
    char response[2048] ;
    char ip[16], buffer[1024];

    if((socket_fd = ftp_connect(fields[IP_INDEX], 21)) < 0)
    {
        return socket_fd;
    }

    if((ftp_response(socket_fd, response)) != SOCKET_READY)
    {
        return -3;
    }

    if((errno = ftp_login(socket_fd, fields[USER_INDEX], fields[PASS_INDEX])) < 0)
    {
        return -errno;
    }


    if(ftp_command(socket_fd, "pasv", NULL))
    {
        return -8;
    }

    if(ftp_response(socket_fd, response) != PASV)
    {
        return -9;
    }

    port = parse_pasv_port(response, ip);
    

    if((data_fd = ftp_connect(ip, port)) < 0)
    {
        return -data_fd;
    }


    if(ftp_command(socket_fd, "retr", fields[PATH_INDEX]) < 0)
    {
        return -10;
    }

    if((file_fd = open(fields[FILE_INDEX], O_WRONLY | O_CREAT, 0777)) < 0){
        return -11;
    }

    while((size = read(data_fd, buffer, 1024)) > 0){
        if (write(file_fd, buffer, size) < 0) {
            return -12;
        }
    }

    if(close(file_fd) < 0)
    {    
        return -13;
    }

    if(close(data_fd) < 0)
    {
        return -14;
    }

    if(close(socket_fd < 0))
    {
        return -15;    
    }

    return 0;
}

int paserArgs(char* url, char* fields[]){

    char * ftp = strtok(url, "/");
    char * args = strtok(NULL, "/");
    fields[PATH_INDEX] = strtok(NULL, "");

    if(ftp == NULL || args == NULL || fields[PATH_INDEX] == NULL)
    {
        return -1;
    }

    fields[USER_INDEX] = strtok(args,":");
    fields[PASS_INDEX] = strtok(NULL,"@");
    fields[HOST_INDEX] = strtok(NULL, "");
    
    if(strchr(fields[USER_INDEX],'@') != NULL){

        fields[USER_INDEX] = "anonymous";
        
        fields[PASS_INDEX] = strtok(args,"@");
        fields[HOST_INDEX] = strtok(NULL, "");
        
        if(fields[HOST_INDEX] == NULL){
            char * temp = fields[PASS_INDEX];
            fields[PASS_INDEX] = "anonymous";
            fields[HOST_INDEX] = temp;
        }
    }
    else{
        char * temp = fields[USER_INDEX];
        fields[USER_INDEX] = "anonymous";
        fields[PASS_INDEX] = "anonymous";
        fields[HOST_INDEX] = temp;
    }

    char* file_name;
    (file_name = strrchr(fields[PATH_INDEX], '/')) ? ++file_name : (file_name = fields[PATH_INDEX]);

    fields[FILE_INDEX] = file_name;

    if(fields[USER_INDEX] == NULL || fields[PASS_INDEX] == NULL || fields[HOST_INDEX] == NULL || fields[FILE_INDEX] == NULL)
    {
        return -2;
    }

    struct hostent * host;

    if ((host = gethostbyname(fields[HOST_INDEX])) == NULL)
    {
        return -3;
    }

    fields[HOST_NAME_INDEX] = host->h_name;
    fields[IP_INDEX] = inet_ntoa(*((struct in_addr *)host->h_addr));

    printf("User: %s\nPassword: %s\nHost: %s\nPath: %s\nFile name: %s\nHost name: %s\nIP address: %s\n", fields[USER_INDEX], fields[PASS_INDEX], fields[HOST_INDEX], fields[PATH_INDEX], fields[FILE_INDEX],fields[HOST_NAME_INDEX],fields[IP_INDEX]);
    
    return 0;  
}