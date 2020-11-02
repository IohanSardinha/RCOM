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


volatile int STOP=FALSE;
volatile int TIME_OUT=FALSE;
enum s_frame_state_machine state_machine = START_S;

void atende()
{

    if(state_machine == STOP_S){
      STOP = TRUE;
      conta=1;}
    else{
      TIME_OUT=TRUE;
      conta++;}
}

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

/*

  (void) signal(SIGALRM, atende);

  do
  {
    TIME_OUT = FALSE;


    char* SET = s_frame(A_EM,C_SET);
    
    res = write(fd,SET,5);
    printf("Sent SET\n");
    free(SET);
    

    alarm(3);

    char rcvd[1];
    char frame[5];

    state_machine = START_S;
    contaactual=conta;

    while((!TIME_OUT)&& state_machine!=STOP_S)
    {
      res = read(fd,rcvd,1);
      if(TIME_OUT)
        break;
      change_s_frame_state(&state_machine, rcvd[0], frame);
    };
    
   
    if(state_machine==STOP_S)
      printf("Recived UA\n");

  }while(!STOP && state_machine != STOP_S && conta<4);

*/


    char isla[3]= {0x01,0x7e,0x01};
    int frameSize;
    char* IFRAME = i_frame(isla,A_EM,C_SET,3,&frameSize);

    for (int i=0; i<13;i++)
    	printf("%x",IFRAME[i]);
    
    res = write(fd,IFRAME,frameSize);
    
    
    printf("Sent Dados\n");
    free(IFRAME);


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
