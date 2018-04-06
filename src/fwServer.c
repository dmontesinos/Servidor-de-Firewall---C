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
  
  
  while(1) {
    //TODO
    do {
      //TODO: finish = process_msg(....., &chain);
    }while(!finish);
    
    //TODO
  }  
  
  return 0;
 }
