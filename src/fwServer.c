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
  //char *buffer = (char*) malloc(14*sizeof(char));
  char buffer[MAX_BUFF_SIZE];
  //struct hello_rp hello_rp;
  //TODO
  unsigned short code=2;
  memset(buffer,'\0',(14*sizeof(char)));
  stshort(code,buffer);
  //Escribimos Hello World a partir del 2 byte (opcode)
  char message[11] = "Hello World";
  int i;
  for (i = 2; i< 13; i++)
  {
	  *(buffer+i)=message[i-2];
  }
  //Enviamos la respuesta al cliente
  send(sock,buffer,(14*sizeof(char)),0);
  //TODO
}
 
 
 void process_LIST_msg(int sock, struct FORWARD_chain *chain)
 {

  char buffer[MAX_BUFF_SIZE];
  struct fw_rule *aux = chain->first_rule;
  rule copia;
  unsigned short code=4;
  memset(buffer,'\0',MAX_BUFF_SIZE);
  stshort(code,buffer);
  unsigned short num_rules = (unsigned short)chain->num_rules;
  stshort(num_rules,buffer+2);
  if (chain->num_rules > 0 )
  {
	  int contador=4;
	  int i = 0;
	  for (i = 0; i < chain->num_rules; i++)
	  {
		copia=aux->rule;
		memcpy(buffer+contador,&copia,sizeof(rule));
		aux = aux->next_rule;
		contador+=sizeof(rule);
	  }
		send(sock,buffer,contador,0);
  }
  else {
	  send(sock,buffer,(4*sizeof(char)),0);
  }
  
  
 }
 
 void process_ADD_msg(int sock, struct FORWARD_chain *chain,char *buffer)
 {
	 //Creamos 2 reglasfw nuevas 1 para movernos(aux) y otra para insertar(newfw) y 1 regla rule que copiaremos del buffer
         struct fw_rule *aux;
	 struct fw_rule *newfw;
	 rule new;
	 //Copiamos lo que hay en el buffer a la nueva regla
	 memcpy(&new,buffer+2,sizeof(rule));
	 //Asignamos memoria dinamica a la nueva regla fw
	 newfw = malloc(sizeof(struct fw_rule));
	 //Inicializamos la nueva regla fw con los valores correspondientes
	 newfw->next_rule = NULL;
	 newfw->rule=new;
	 
     if (chain->first_rule==NULL)
     {
         chain->first_rule = newfw;
     }
     else{
         aux=chain->first_rule;
         while (aux->next_rule!=NULL)
         {
             aux = aux->next_rule;
         }
         aux->next_rule=newfw;
     }
	 
	 chain->num_rules++;
 }
 
 void process_DELETE_msg(int sock, struct FORWARD_chain *chain, char *buffer)
 {
     char mensaje[MAX_BUFF_SIZE];
     memset(mensaje,'\0',MAX_BUFF_SIZE);
     unsigned short indice = ldshort(buffer+2);
     struct fw_rule *aux = chain->first_rule;
     struct fw_rule *anterior;
     if (indice > chain->num_rules)
     {
         unsigned short code = 11;
         stshort(code,mensaje);
         unsigned short error=ERR_RULE;
         stshort(error,mensaje+2);
         send(sock,mensaje,4,0);
     }
     else{
         if (indice == 1)
         {
             if (chain->num_rules == 1)
             {
                 chain->first_rule = NULL;
                 free(aux);
                 chain->num_rules--;
             }
             else{
                 chain->first_rule=aux->next_rule;
                 free(aux);
                 chain->num_rules--;
             }
         }
         else
         {
              if (indice==chain->num_rules)
              {
                  int i = 1;
                  for ( i = 1;i < chain->num_rules;i++)
                  {
                      anterior=aux;
                      aux=aux->next_rule;
                  }
                  chain->num_rules--;
                  free(aux);
                  
              }
              else{
                  int i = 1;
                  for ( i = 1; i< indice;i++)
                  {
                      anterior=aux;
                      aux=aux->next_rule;
                  }
                  anterior->next_rule=aux->next_rule;
                  free(aux);
                  chain->num_rules--;
              }
         }
         unsigned short code = 10;   
         stshort(code,mensaje);
         send(sock,mensaje,2,0);
     }
 }
 
 void process_CHANGE_msg(int sock, struct FORWARD_chain *chain, char *buffer)
 {
	char mensaje[MAX_BUFF_SIZE];
    memset(mensaje,'\0',MAX_BUFF_SIZE);
    unsigned short indice = ldshort(buffer+2);
	struct fw_rule *aux = chain->first_rule;
	rule modificada;
	memcpy(&modificada,buffer+4,sizeof(rule));
    if ( indice > chain->num_rules )
    {
        unsigned short code = 11;
        stshort(code,mensaje);
        unsigned short error=ERR_RULE;
        stshort(error,mensaje+2);
        send(sock,mensaje,4,0);
    }
    else
	{
        int i =1;
		for (i = 1; i < indice; i++)
        {
			aux = aux->next_rule;
        }
		aux->rule=modificada;
        unsigned short code = 10;   
        stshort(code,mensaje);
        send(sock,mensaje,2,0); 
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
  //char *buffer = (char*) malloc(MAX_BUFF_SIZE*sizeof(char));
  char buffer[MAX_BUFF_SIZE];
  unsigned short op_code;
  recv(sock,buffer,MAX_BUFF_SIZE,0); 
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
        process_CHANGE_msg(sock,chain,&buffer);
      break;                              
    case MSG_DELETE:
        process_DELETE_msg(sock,chain,&buffer);
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
  server_address.sin_port = htons(port);
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
	  int pid ;
	  pid = fork();
	  if (pid == 0)
	  {
		do {
      
			finish=process_msg(s2,&chain);
	 
			//TODO: finish = process_msg(....., &chain);

			}while(!finish);
	  }
    //TODO
  }  
  
  return 0;
 }
