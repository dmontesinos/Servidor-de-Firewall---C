/***************************************************************************
 *            fwClient.h
 *
 *  Copyright  2016  mc
 *  <mcarmen@<host>>
 ****************************************************************************/
#include "fwClient.h"

/**
 * Function that sets the field addr->sin_addr.s_addr from a host name 
 * address.
 * @param addr struct where to set the address.
 * @param host the host name to be converted
 * @return -1 if there has been a problem during the conversion process.
 */
int setaddrbyname(struct sockaddr_in *addr, char *host)
{
  struct addrinfo hints, *res;
	int status;

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM; 
 
  if ((status = getaddrinfo(host, NULL, &hints, &res)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
    return -1;
  }
  
  addr->sin_addr.s_addr = ((struct sockaddr_in *)res->ai_addr)->sin_addr.s_addr;
  
  freeaddrinfo(res);
    
  return 0;  
}


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
	while((param = getopt(argc, argv, "h:p:")) != -1){
		switch((char) param){
		  case 'h': break;
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
 * Returns the host name where the server is running.
 * @param argc the number of the application arguments.
 * @param an array with all the application arguments.
 * @Return Returns the host name where the server is running.<br />
 * Returns null if the application has been called with the wrong parameters.
 */
 char * getHost(int argc, char* argv[]){
  char * hostName = NULL;
  int param;
  
  optind=1;
    // We process the application execution parameters.
	while((param = getopt(argc, argv, "h:p:")) != -1){
		switch((char) param){
			case 'p': break;
			case 'h':
        hostName = (char*) malloc(sizeof(char)*strlen(optarg)+1);
				// Un cop creat l'espai, podem copiar la cadena
				strcpy(hostName, optarg);
				break;
			default:
				printf("Parametre %c desconegut\n\n", (char) param);
				hostName = NULL;
		}
	}
	
	printf("in getHost host: %s\n", hostName); //!!!!!!!!!!!!!!
	return hostName;
 }
 
 
 
/**
 * Shows the menu options. 
 */
void print_menu()
{
		// Mostrem un menu perque l'usuari pugui triar quina opcio fer

		printf("\nAplicació de gestió del firewall\n");
		printf("  0. Hello\n");
		printf("  1. Llistar les regles filtrat\n");
		printf("  2. Afegir una regla de filtrat\n");
		printf("  3. Modificar una regla de filtrat\n");
		printf("  4. Eliminar una regla de filtrat\n");
		printf("  5. Eliminar totes les regles de filtrat.\n");
		printf("  6. Sortir\n\n");
		printf("Escull una opcio: ");
} 


/**
 * Sends a HELLO message and prints the server response.
 * @param sock socket used for the communication.
 */
void process_hello_operation(int sock)
{
  //struct hello_rp hello_rp;
  //TODO
  char buffer[MAX_BUFF_SIZE];
  memset(buffer,'\0',sizeof(buffer));
  unsigned short code = 5;
  stshort(code,buffer);
  buffer[18]="H";
  send(sock,buffer,sizeof(buffer),0);
  
}

/**
 * Closes the socket connected to the server and finishes the program.
 * @param sock socket used for the communication.
 */
void process_exit_operation(int sock)
{
  //TODO
  exit(0);     
}

/** 
 * Function that process the menu option set by the user by calling 
 * the function related to the menu option.
 * @param s The communications socket
 * @param option the menu option specified by the user.
 */
void process_menu_option(int s, int option)
{		  
  switch(option){
    // Opció HELLO
    case MENU_OP_HELLO:
      process_hello_operation(s);
      break;
    case MENU_OP_LIST_RULES:      
      break;  
    case MENU_OP_ADD_RULE:      
      break;   
    case MENU_OP_CHANGE_RULE:
      break;   
    case MENU_OP_DEL_RULE:    	
      break;
    case MENU_OP_FLUSH:
      break;       
    case MENU_OP_EXIT:      
    default:
      printf("Invalid menu option\n");          
  }
}


int main(int argc, char *argv[]){ 
 	int client_socket;
 	unsigned short port;
 	char *hostName;  
 	int menu_option = 0;
    
 	port = getPort(argc, argv);  
 	hostName = getHost(argc, argv);


 	struct sockaddr_in server_address;
 	server_address.sin_family = AF_INET;
 	server_address.sin_port = htons(port);
 	setaddrbyname(&server_address,hostName);
 
  //Checking that the host name has been set.Otherwise the application is stopped. 
	if(hostName == NULL){
		perror("No s'ha especificat el nom del servidor\n\n");
		return -1;
	}

	client_socket = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (client_socket < 0)
	{
		printf("Error en la creación del socket del cliente, codigo de error %d\n",client_socket);
	}
	else 
	{
		printf("Socket del cliente creado con exito, descriptor del socket %d\n",client_socket);
	}

	if (connect(client_socket,(struct sockaddr*)&server_address, sizeof(server_address)) < 0 )
	{
		printf("No ha sido posible establecer conexion con el servidor\n");
	}
	else
	{
		printf("Conexion con el servidor establecida con exito\n");
	}
  
  	do{
      print_menu();
		  // getting the user input.
		  scanf("%d",&menu_option);
		  printf("\n\n"); 
		  process_menu_option(client_socket,menu_option);

	  }while(menu_option != MENU_OP_EXIT); //end while(opcio)
 
  return 0; 
}
