/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <signal.h>
#include "API.h"

#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

int flag=1, conta=1;

void atende()                   // atende alarme
{
	printf("alarme # %d\n", conta);
	flag=1;
	conta++;
}

volatile int STOP=FALSE;

int main(int argc, char** argv)
{
	
  int fd,c, res, retRes,contaactual;
  struct termios oldtio,newtio;
  char buf[255];
  char returnBuf[255];
  int i, sum = 0, speed = 0;
  
  if ( (argc < 2) || 
	     ((strcmp("/dev/ttyS0", argv[1])!=0) && 
	      (strcmp("/dev/ttyS1", argv[1])!=0) )) {
    printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
    exit(1);
  }

 
  /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
  */


  fd = open(argv[1], O_RDWR | O_NOCTTY );
  if (fd <0) {perror(argv[1]); exit(-1); }

  if ( tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
    perror("tcgetattr");
    exit(-1);
  }

  bzero(&newtio, sizeof(newtio));
  newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
  newtio.c_iflag = IGNPAR;
  newtio.c_oflag = 0;

  /* set input mode (non-canonical, no echo,...) */
  newtio.c_lflag = 0;

  newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
  newtio.c_cc[VMIN]     = 0;   /* blocking read until 5 chars received */



  /* 
    VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a 
    leitura do(s) próximo(s) caracter(es)
  */



  tcflush(fd, TCIOFLUSH);

  if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
    perror("tcsetattr");
    exit(-1);
  }

  printf("New termios structure set\n\n");

(void) signal(SIGALRM, atende);
  //do{
while(conta<4){
	contaactual = conta;
    char* SET = s_frame(A_EM,C_SET);
    
    res = write(fd,SET,5);
    printf("Sent SET[%d]\n", res);
    free(SET);
    

    enum s_frame_state_machine state_machine = START_S;
    char rcvd[1];
    char frame[5];
    alarm(3);
    do
    {
      res = read(fd,rcvd,1);
      change_s_frame_state(&state_machine, rcvd[0], frame);
    }while((state_machine != STOP_S) && (contaactual==conta));
    
    }
    
    if (state_machine==STOP_S){

    printf("Received: ");
    for(int i =0; i < 5; i++)
    {
      //printf(":%x", frame[i]);
    }
    printf(":\n");
    
    }
    else{
    printf("Didnt Work");
    }
    
    
   

    /*res = read(fd,buf,5);
    buf[res] = 0;
    printf("Received: ");
    for(int i =0; i < 5; i++)
    {
      printf(":%x", buf[i]);
    }
    printf(":\n");
    //retRes = read(fd,returnBuf,strlen(buf)+1);

  //}while(buf[0] != 'z');

  /* 
    O ciclo FOR e as instruções seguintes devem ser alterados de modo a respeitar 
    o indicado no guião 
  */
   
  if ( tcsetattr(fd,TCSANOW,&oldtio) == -1) {
    perror("tcsetattr");
    exit(-1);
  }

    close(fd);
    return 0;
}
