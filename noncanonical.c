/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

volatile int STOP=FALSE;
int Setfim=1;// para o
typedef enum {
 start,
 flag_rcv,
 a_rcv,
 c_rcv,
 bcc_ok,
 stop
}statesNames;
statesNames currentState = start;
statesNames nextState;
int mdeSET(unsigned char  *buf, unsigned char *compara)
{
	switch(currentState)
	{
	case start:
	if(buf[0]==compara[0])
	{
	 nextState=flag_rcv;
	 printf("0x%.2x\n", buf[0]);
	}
	 else 
	 {
	  nextState=start;
	 }
	 break;
	
	
	 case flag_rcv:
	 if(buf[0]==compara[1])
	 {
	 
	 printf("0x%.2x\n", buf[0]);
	   nextState=a_rcv;
	 }
	 else if(buf[0]==compara[0])
	 {
	 	nextState=flag_rcv;
	 }
	 else
	 nextState=start;
	 break;
	 
	 
	 
	 case a_rcv:
	 if(buf[0]=compara[2])
	 { 
	  printf("0x%.2x\n", buf[0]);
	   nextState=c_rcv;
	 }
	 else if(buf[0]=compara[1])
	 {
	   nextState=flag_rcv;
	 }
	 else
	 nextState=start;
	 break;
	 case c_rcv:
	 if(buf[0]==compara[3])
	 {
	 	 printf("0x%.2x\n", buf[0]);
	 	nextState=bcc_ok;
	 }
	 else if(buf[0]=compara[1])
	 {
	   nextState=flag_rcv;
	 }
	 else
	 nextState=start;
	 break;
	 
	 case bcc_ok:
	 if(buf[0]==compara[4])
	 {
	 	printf("0x%.2x\n", buf[0]);
	 	nextState=stop;
		Setfim=0;//chegou ao fim
	 }
	 else
	 nextState=start;
	 break;
	 case stop:

	 //nextState=start;
	 printf("fimmmmm\n");
	 break;
	}
	
	
	currentState=nextState;
	
	
}
int main(int argc, char** argv)
{
    int fd,c, res;
    struct termios oldtio,newtio;
    char buf[255];

    /*if ( (argc < 2) ||
         ((strcmp("/dev/ttyS0", argv[1])!=0) &&
          (strcmp("/dev/ttyS1", argv[1])!=0) )) {
        printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
       // exit(1);
    }


    /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
    */


    fd = open(argv[1], O_RDWR | O_NOCTTY );
    if (fd < 0) { perror(argv[1]); exit(-1); }

   if (tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
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
    newtio.c_cc[VMIN]     = 5;   /* blocking read until 5 chars received */

    /*
    VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a
    leitura do(s) próximo(s) caracter(es)
    */


   tcflush(fd, TCIOFLUSH);

   if (tcsetattr(fd,TCSANOW,&newtio) == -1) {
       perror("tcsetattr");
        exit(-1);
    }

    printf("New termios structure set\n");

   // while (STOP==FALSE) {       /* loop for input */
     //   res = read(fd,buf,255);   /* returns after 5 chars have been input */
       // buf[res]=0;               /* so we can printf... */
        //printf(":%s:%d\n", buf, res);
        //if (buf[0]=='z') STOP=TRUE;
   // }
   unsigned char SET_comparar[5]={0x5c,0x01,0x03,0x01^0x03,0x5c};
   unsigned char SET_recebido[255];
   
   int count=0;
   int l=0;
while(Setfim)
{
	read(fd,&SET_recebido[count],1);    //le 1

	 mdeSET(SET_recebido,SET_comparar);

}
mdeSET(SET_recebido,SET_comparar);

if (currentState == stop){









}
printf("fugi\n");
// vamos envaiar agora o UA para confirmar
  char uasend[255]={0x5c, 0x01,0x07,0x01^0x07,0x5c};// o UA que  vai enviar
   int n_caracteres=strlen(uasend);
   int j=0;
   res=write(fd,uasend,n_caracteres+1);// enviou tudo de uma vez
    /*
    O ciclo WHILE deve ser alterado de modo a respeitar o indicado no guião
    */

 tcsetattr(fd,TCSANOW,&oldtio);
    close(fd);
    return 0;
}
