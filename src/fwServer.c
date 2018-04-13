/***************************************************************************
 *            fwServer.c
 *
 *  Copyright  2016  mc
 *  <mc@<host>>
 ****************************************************************************/

#include "fwServer.h"	

/**
 * Returns the port specified as an application parameter or the default port
 * if no port has been specified.
 * @param argc the number of the application arguments.
 * @param an array with all the application arguments.
 * @return  the port number from the command line or the default port if 
 * no port has been specified in the command line. Returns -1 if the application
 * has been called with the wrong parameters.
 */
int getPort(int argc, char* argv[])
{
  int param;
  int port = DEFAULT_PORT;

  optind=1;
  // We process the application execution parameters.
	while((param = getopt(argc, argv, "p:")) != -1){
		switch((char) param){
			case 'p':
			  // We modify the port variable just in case a port is passed as a 
			  // parameter
				port = atoi(optarg);
				break;
			default:
				printf("Parametre %c desconegut\n\n", (char) param);
				port = -1;
		}
	}
	
	return port;
}

  
 /**
 * Function that sends a HELLO_RP to the  client
 * @param sock the communications socket
 */
void process_HELLO_msg(int sock)
{
  struct hello_rp hello_rp;
  
  //TODO
}
 
 /** 
 * Receives and process the request from a client.
 * @param the socket connected to the client.
 * @param chain the chain with the filter rules.
 * @return 1 if the user has exit the client application therefore the 
 * connection whith the client has to be closed. 0 if the user is still 
 * interacting with the client application.
 */
int process_msg(int sock, struct FORWARD_chain *chain)
{
  unsigned short op_code;
  int finish = 0;
    

  switch(op_code)
  {
    case MSG_HELLO:
      process_HELLO_msg(sock);
      break;    
    case MSG_LIST:      
      break;
    case MSG_ADD:
      break;                              
    case MSG_CHANGE:      
      break;                              
    case MSG_DELETE:
      break;                              
    case MSG_FLUSH:
      break;                                                                                     
    case MSG_FINISH:
      //TODO
      finish = 1;
      break;
    default:
      perror("Message code does not exist.\n");
  } 
  
  return finish;
}
 
 int main(int argc, char *argv[]){
  int port = getPort(argc, argv);
  int finish=0;
  struct FORWARD_chain chain;
  
  chain.num_rules=0;
  chain.first_rule=NULL;

  socklen_t client_address_len;
  struct sockaddr_in client_address;
  client_address_len = sizeof(client_address);
  
  int server_socket = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);

  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(9411);
  server_address.sin_addr.s_addr = htonl(INADDR_ANY);

  if (server_socket < 0)
  {
    printf("Error en la creación del socket del servidor, codigo de error %d\n",server_socket);
  }
  else 
  {
    printf("Socket del servidor creado con exito, descriptor del socket %d\n",server_socket);
  }

  

  bind(server_socket,(struct sockaddr*)&server_address,sizeof(server_address));
  listen(server_socket,MAX_QUEUED_CON);

  while(1) {
    int s2 = accept(server_socket, (struct sockaddr*)&client_address,&client_address_len);
    do {
      int buffer [MAX_BUFF_SIZE];
      if (recv(s2,buffer,MAX_BUFF_SIZE,0) > 0)
      {
        printf("%d",buffer[0]);
      }
      //TODO: finish = process_msg(....., &chain);

    }while(!finish);
    
    //TODO
  }  
  
  return 0;
 }
