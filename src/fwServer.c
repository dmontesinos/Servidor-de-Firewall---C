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
  //struct hello_rp hello_rp;
  //Creamos un buffer de 14bytes (especificado en pdf)
  char *buffer = (char*) malloc(14*sizeof(char));
  //struct hello_rp hello_rp;
  //TODO
  unsigned short code=2;
  memset(buffer,'\0',(14*sizeof(char)));
  stshort(code,buffer);
  //Escribimos Hello World a partir del 2 byte (opcode)
  char message[11] = "Hello World";
  for (int i = 2; i< 13; i++)
  {
	  *(buffer+i)=message[i-2];
  }
  
  *((short*)buffer) = htons(code);
  //Enviamos la respuesta al cliente
  send(sock,buffer,(14*sizeof(char)),0);
  free(buffer);
  //TODO
}
 
 
 void process_LIST_msg(int sock, struct FORWARD_chain *chain)
 {

  char *buffer = (char*) malloc(16*sizeof(char));
  struct fw_rule *aux = chain->first_rule;
  unsigned short code=4;
  memset(buffer,'\0',(16*sizeof(char)));
  stshort(code,buffer);
  
  if (chain->num_rules > 0 )
  {
		
		do{
			*((short*)buffer) = htons(code);
			*(buffer+2) = aux;
			send(sock,buffer,(16*sizeof(char)),0);
			aux = aux->next_rule;
		}while(aux->next_rule!=NULL);
  }
  else {
	  *(buffer+2)=chain->num_rules;
	  *((short*)buffer) = htons(code);
	  send(sock,buffer,(4*sizeof(char)),0);
  }
  
  
  //Enviamos la respuesta al cliente
  free(buffer);
 }
 
 void process_ADD_msg(int sock, struct FORWARD_chain *chain,char *buffer)
 {
     struct fw_rule aux;
     if (chain->first_rule==NULL)
     {
         //chain->first_rule->rule = *((rule*)buffer+2);
         memcpy(aux.rule,(buffer+2),sizeof(rule));
         chain->first_rule->next_rule = NULL;
         chain->num_rules++;
     }
     else{
         aux.next_rule=chain->first_rule->next_rule;
         while (aux.next_rule!=NULL)
         {
             aux.next_rule=aux.next_rule;
         }
         //aux.rule=*((rule*)buffer+2);
         memcpy(aux.rule,(buffer+2),sizeof(rule));
         aux.next_rule=NULL;
         chain->num_rules++;
     }
     
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
  int finish = 0;
  char *buffer = (char*) malloc(MAX_BUFF_SIZE*sizeof(char));
  unsigned short op_code;
  recv(sock,buffer,sizeof(MAX_BUFF_SIZE*sizeof(char)),0); 
  op_code = ldshort(buffer);
  switch(op_code)
  {
    case MSG_HELLO:
      process_HELLO_msg(sock);
      break;    
    case MSG_LIST: 
	process_LIST_msg(sock,chain);
      break;
    case MSG_ADD:
	process_ADD_msg(sock,chain,&buffer);
      break;                              
    case MSG_CHANGE:      
      break;                              
    case MSG_DELETE:
      break;                              
    case MSG_FLUSH:
      break;                                                                                     
    case MSG_FINISH:
	//Cerramos el socket con el cliente y salimos del bucle interno en el main
      close(sock);
      finish = 1;
      break;
    default:
      perror("Message code does not exist.\n");
  } 
  free(buffer);
  return finish;
}
 
 int main(int argc, char *argv[]){
  int port = getPort(argc, argv);
  int finish=0;
  struct FORWARD_chain chain;
  chain.first_rule=NULL;
  chain.num_rules = 0;

  socklen_t client_address_len;
  struct sockaddr_in client_address;
  client_address_len = sizeof(client_address);
  
  int server_socket = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);

  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(9411);
  server_address.sin_addr.s_addr = htonl(INADDR_ANY);

  //Creacion del server sockett
  if (server_socket < 0)
  {
    printf("Error en la creación del socket del servidor, codigo de error %d\n",server_socket);
  }
  else 
  {
    printf("Socket del servidor creado con exito, descriptor del socket %d\n",server_socket);
  }

  
  //Asignamos al socket una direccion IP
  bind(server_socket,(struct sockaddr*)&server_address,sizeof(server_address));
  //Maximo numero de peticiones que encolara el servidor
  listen(server_socket,MAX_QUEUED_CON);

  while(1) {
    int s2 = accept(server_socket, (struct sockaddr*)&client_address,&client_address_len);
    do {
      
	  finish=process_msg(s2,&chain);
	 
      //TODO: finish = process_msg(....., &chain);

    }while(!finish);
    
    //TODO
  }  
  
  return 0;
 }
