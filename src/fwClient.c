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
  //Creamos un buffer de 2 bytes para enviar el opCode de la peticion hello
  //char *buffer = (char*) malloc(2*sizeof(char));
  char buffer[MAX_BUFF_SIZE];
  //struct hello_rp hello_rp;
  unsigned short code=1;
  memset(buffer,'\0',MAX_BUFF_SIZE);
  stshort(code,buffer);
  //Enviamos la peticion al server
  send(sock,buffer,(2*sizeof(char)),0);
  //Server nos responde 
  recv(sock,buffer,(14*sizeof(char)),0);
  //Printamos la respuesta del server a partir del 2 byte (opCode)
  char c='A';
  int i = 2;
  while (c!='\0')
  {
	  c = *(buffer+i);
	  i++;
	  printf("%c",c);
  }
}

void process_LIST_operation(int sock)
{
	 //Creamos un buffer de 2 bytes para enviar el opCode de la peticion hello
  rule aux;
  char *buffer = (char*) malloc(2*sizeof(char));
  unsigned short code=3;
  memset(buffer,'\0',(2*sizeof(char)));
  stshort(code,buffer);
  //Enviamos la peticion al server
  send(sock,buffer,(2*sizeof(char)),0);
  //Cambiamos el tamaño del buffer eliminando el buffer y creando otro nuevo
  free(buffer);
  buffer = (char*) malloc(16*sizeof(char));
  //Server nos responde 
  recv(sock,buffer,(16*sizeof(char)),0);
  //Printamos la respuesta del server a partir del 2 byte (opCode)
  
  unsigned short contador = *(buffer+2);
  printf("En el servidor hay %u reglas\n",contador);
  while (contador > 0)
  {
	  //Tratar la regla
	  
	   contador--;
	   if (contador>0)
	   {
		  recv(sock,buffer,(16*sizeof(char)),0); 
	   }
  }

  free(buffer);
}

rule introducir_regla(rule regla)
{
	char ip[MAX_BUFF_SIZE];
	memset(ip,'\0',MAX_BUFF_SIZE);
    int ip1;
	int ip2;
	int ip3;
	int ip4;
    
	printf("Src/dst addr");
	scanf("%hu",&regla.src_dst_addr);
    printf("IP1: ");
    scanf("%d",&ip1);
	printf("IP2: ");
    scanf("%d",&ip2);
	printf("IP3: ");
    scanf("%d",&ip3);
	printf("IP4: ");
    scanf("%d",&ip4);
    printf("Mascara: ");
    scanf("%hu",&regla.mask);
    printf("Puerto: ");
    scanf("%hu",&regla.port);
    printf("Dport/sport");
    scanf("%hu",&regla.src_dst_port);
	
    
	regla.src_dst_addr = htons(regla.src_dst_addr);
	regla.mask = htons(regla.mask);
	regla.src_dst_port = htons(regla.src_dst_port);
	regla.port = htons(regla.port);
	sprintf(ip, "%d.%d.%d.%d",ip1,ip2,ip3,ip4);
    inet_aton(ip,&regla.addr);
	
	return regla;
}

void process_ADD_operation(int sock)
{
    //char *buffer = (char*) malloc(14*sizeof(char));
	char buffer[MAX_BUFF_SIZE];
    memset(buffer,'\0',MAX_BUFF_SIZE);
    unsigned short code = 5;
    stshort(code,buffer);
    rule nueva_regla;
    printf("He creado una nueva regla con exito \n");
    nueva_regla=introducir_regla(nueva_regla);
    printf("He introducido la regla con exito \n");
    //memcpy(buffer+2,&nueva_regla,sizeof(nueva_regla)); //memcpy-> para copiar buffers/reglas
    printf("He asignado bien la memoria \n");
    send(sock,buffer,MAX_BUFF_SIZE,0);
    
    
    
}



/*
 * Closes the socket connected to the server and finishes the program.
 * @param sock socket used for the communication.
 */
void process_exit_operation(int sock)
{
  //Creamos un buffer de 2 bytes para enviar el opCode de la peticion exit
  //char *buffer = (char*) malloc(2*sizeof(char));
  char buffer[MAX_BUFF_SIZE];
  unsigned short code=9;
  memset(buffer,'\0',MAX_BUFF_SIZE);
  stshort(code,buffer);
  //Enviamos la peticion al server y cerramos el cliente
  send(sock,buffer,(2*sizeof(char)),0);
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
    case MENU_OP_HELLO:
      process_hello_operation(s);
      break;
    case MENU_OP_LIST_RULES: 
		process_LIST_operation(s);
      break;  
    case MENU_OP_ADD_RULE:  
		process_ADD_operation(s);
      break;   
    case MENU_OP_CHANGE_RULE:
      break;   
    case MENU_OP_DEL_RULE:    	
      break;
    case MENU_OP_FLUSH:
      break;       
    case MENU_OP_EXIT:
		process_exit_operation(s);
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
	//Abrimos el socket del cliente
	client_socket = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (client_socket < 0)
	{
		printf("Error en la creación del socket del cliente, codigo de error %d\n",client_socket);
	}
	else 
	{
		printf("Socket del cliente creado con exito, descriptor del socket %d\n",client_socket);
	}
	//Conectamos cliente con servidor
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
