/*
    This lib is primarily a pass through for socket operations 
    so in the future, it can abstract windows/ linux.

*/

#ifndef NABLA_LIBC_NETWORK_SOCKETS
#define NABLA_LIBC_NETWORK_SOCKETS

#include<sys/socket.h>
#include<arpa/inet.h>

struct nabla_socket
{
    int socket_desc;
    struct sockaddr_in saddr;
};

//! \brief Creates a socket description 
//! \retval -1 Error, return object will be NULL
//! \retval  0 Success.
//! \note Sockets are sockets. Client and server. If client, the address will be fore the remote
//!       While, if its the server, it should be the address to bind to
struct nabla_socket * sockets_create_socket(int domain, int type,   int protocol, 
                                            char *addr, short port, int *result);

void sockets_delete(struct nabla_socket* ns);

// Applies to SOCK_STREAM/TCP sockets.
void sockets_connect(struct nabla_socket * ns, int *result);

void sockets_send(struct nabla_socket * ns, char* data, int *result);

void sockets_recv(struct nabla_socket *ns, char * buffer, unsigned bufferLen, int *result);

void socket_close(struct nabla_socket *ns);

#endif