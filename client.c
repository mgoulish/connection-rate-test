#include <netdb.h> 
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>



static
void 
error ( const char * msg )
{
  perror ( msg );
  exit ( 1 );
}


static
double
get_timestamp ( void )
{
  struct timeval t;
  gettimeofday ( & t, 0 );
  return t.tv_sec + ((double) t.tv_usec) / 1000000.0;
}


int 
main ( int argc, char ** argv )
{
  char name[200];

  int sockfd, 
      port;
  int n_messages,
      report_frequency;
  struct sockaddr_in   serv_addr;
  struct hostent     * server;

  if ( argc < 4 ) 
  {
    fprintf ( stderr,
              "usage %s port n_messages report_frequency\n", 
              argv[0]
            );
    exit ( 1 );
  }
  fprintf ( stderr, "client : starting\n" );

  port             = atoi(argv[1]);
  n_messages       = atoi(argv[2]);
  report_frequency = atoi(argv[3]);

  sprintf ( name, "client_%d", port );


  //-------------------------------------
  // Repeatedly make connections and
  // immediately drop them.
  //-------------------------------------
  int count = 0;
  double start = 0;
  while ( count < n_messages )
  {
    sockfd = socket ( AF_INET, SOCK_STREAM, 0 );
    if ( sockfd < 0 ) 
      error ( "client : error opening socket" );

    server = gethostbyname ( "localhost" );
    if ( server == NULL ) 
      error ( "client : no such host" );

    bzero ( (char *) & serv_addr, sizeof(serv_addr) );
    serv_addr.sin_family = AF_INET;
    bcopy ( (char *) server->h_addr, 
            (char *) & serv_addr.sin_addr.s_addr,
            server->h_length
          );
    serv_addr.sin_port = htons ( port );

    if ( 0 > connect ( sockfd,
                       (struct sockaddr *) & serv_addr,
                       sizeof(serv_addr)
                     )
       ) 
      error ( "client : error connecting" );
    close ( sockfd );
    if ( count == 0 )
    {
      start = get_timestamp();
    }
    ++ count;

    /*
    if ( ! (count % 50000) )
    {
      fprintf ( stderr, "%s pausing at count %d", name, count );
      sleep ( 10 );
    }
    */

    //usleep ( 150 );
    if ( report_frequency > 0 )
    {
      if (! (count % report_frequency) )
      {
        double duration  = get_timestamp() - start;
        fprintf ( stderr, 
                  "%s : %d connections at %.6lf seconds\n", 
                  name,
                  count, 
                  duration
                );
      }
    }
    /*
    if ( count == 50000 )
    {
      fprintf ( stderr, "MDEBUG 50000 pause!\n" );
      sleep ( 30 );
    }
    */
  }

  if ( report_frequency > 0 )
  {
    double stop = get_timestamp();
    double dur  = stop - start;
    double cps = (double)count / dur;
    fprintf ( stderr, "%s : %.1lf connections per second\n", name, cps);
  }

  return 0;
}

