/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

volatile int STOP=FALSE;
//criar os estados da maquina de estado

int fd;
int count = 0;


typedef enum {
 start,
 flag_rcv,
 a_rcv,
 c_rcv,
 bcc_ok,
 stop
}stateNames;
int Uafim=1;// para o
stateNames currentState = start;
stateNames nextState;
int mdeUA(char *buf, char *compara)
{

	switch(currentState)
	{
	case start:
	if(buf[count]==compara[0])
	{
	 nextState=flag_rcv;
	 printf("0x%.2x\n", buf[count]);
	 count++;
	}
	 else 
	 {
//	 printf("%d\n",count);
	  nextState=start;
	  count++;
	 }
	 break;
	
	
	 case flag_rcv:
	 if(buf[count]==compara[1])
	 {
	 
	 printf("0x%.2x\n", buf[count]);
	   nextState=a_rcv;
	   count++;
	 }
	 else if(buf[count]==compara[0])
	 {
	 count++;
	 	nextState=flag_rcv;
	 }
	 else{
	 nextState=start;
	 count++;
	 }
	 break;
	 
	 
	 
	 case a_rcv:
	 if(buf[count]=compara[2])
	 { 
	  printf("0x%.2x\n", buf[count]);
	   nextState=c_rcv;
	   count++;
	 }
	 else if(buf[count]=compara[1])
	 {
	   nextState=flag_rcv;
	   count++;
	 }
	 else{
	 nextState=start;
	 count++;
	 }
	 break;
	 case c_rcv:
	 if(buf[count]==compara[3])
	 {
	 	 printf("0x%.2x\n", buf[count]);
	 	nextState=bcc_ok;
	 	count++;
	 }
	 else if(buf[count]=compara[1])
	 {
	   nextState=flag_rcv;
	   count++;
	 }
	 else{
	 nextState=start;
	 count++;
	 }
	 break;
	 
	 case bcc_ok:
	 if(buf[count]==compara[4])
	 {
	 	printf("0x%.2x\n", buf[count]);
	 	nextState=stop;
	 	count++;
		Uafim=0;//chegou ao fim
		alarm(0);
	 }
	 else{
	 nextState=start;
	 count++;
	 }
	 break;
	 case stop:
	 Uafim=0;
	 //nextState=start;
	 alarm(0);
	 printf("fimmmmm\n");
	 break;
	}
	
	
	currentState=nextState;
	
}
 void escreve()
{
 printf("New termios structure set\n");
 char setsend[255]={0x5c,0x01,0x03,0x01^0x03,0x5c};// criado o set que se vai enviar
  int n_caracteres=strlen(setsend);
  int j=0;
  alarm(3);
  for(j=0; j<n_caracteres; j++)
   {
      write(fd, &setsend[j], 1); // vai escrever um  caracter de cada vez
   }
}

int main(int argc, char** argv)
{
(void) signal(SIGALRM, escreve);  // instala rotina que atende interrupcao
    int c, res;
    struct termios oldtio,newtio;
    char buf[255];
    int i, sum = 0, speed = 0;

   /* if ( (argc < 2) ||
         ((strcmp("/dev/ttyS0", argv[1])!=0) &&
          (strcmp("/dev/ttyS1", argv[1])!=0) )) {
        printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
       // exit(1);
    }*/


    /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
    */


    fd = open(argv[1], O_RDWR | O_NOCTTY );
    if (fd < 0) { perror(argv[1]); exit(-1); }

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
    newtio.c_cc[VMIN]     = 5;   /* blocking read until 5 chars received */



    /*
    VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a
    leitura do(s) próximo(s) caracter(es)
    */


    tcflush(fd, TCIOFLUSH);

    if (tcsetattr(fd,TCSANOW,&newtio) == -1) {
        //perror("tcsetattr");
        //exit(-1);
    }

   



 //   for (i = 0; i < 255; i++) {
   //     buf[i] = 'a';
   // }
   escreve();  // chama a funcao para escrever no ficheiro uma coisa para depois chamarmos o alarme
   while (Uafim){
   //vamos ler o UA recerbido
   char UA_comparar[5]={0x5c, 0x01, 0x07, 0x01^0x07, 0x5c};
   char UA_recebido[255];
   read(fd,UA_recebido,255);
   count = 0;
   currentState = start;
   
  while(Uafim && (count != strlen(UA_recebido)))
   {
   	 mdeUA(UA_recebido,UA_comparar);
   	 //printf("%d\n",count);
   	 printf("Uafim %d\n",Uafim);
   }
  
   
// aqui ja enviou o set ;
//agora vamos ler o UA mandado pelo o outro e seguir
//char UA_comparar[5]={0x5c, 0x01, 0x07, 0x06, 0x5c};
//char UA_recebido[255];
//int count=0;
//while(1)
//{
	//read(fd,&UA_recebido[count],1);    //le 1
	
	 //if(Uafim)
	 //{
	 //mdeUA(UA_recebido,UA_comparar);
	 //}
	//else
	  // break;

//}
    /*testing*/
    //buf[25] = '\n';

    //res = write(fd,buf,255);
    //printf("%d bytes written\n", res);


    /*
    O ciclo FOR e as instruções seguintes devem ser alterados de modo a respeitar
    o indicado no guião
    */


    if ( tcsetattr(fd,TCSANOW,&oldtio) == -1) {
        perror("tcsetattr");
        exit(-1);
    }

}

    close(fd);
    return 0;
}
