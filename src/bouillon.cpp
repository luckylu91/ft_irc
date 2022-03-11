#include <stdio.h>          // fprintf
#include <sys/event.h>      // kqueue
#include <netdb.h>          // addrinfo
#include <arpa/inet.h>      // AF_INET
#include <sys/socket.h>     // socket
#include <assert.h>         // assert
#include <string.h>         // bzero
#include <stdbool.h>        // bool
#include <unistd.h>         // close

int main(int argc, const char * argv[])
{

    /* Initialize server socket */
    struct addrinfo hints, *res;
    int sockfd;

    bzero(&hints, sizeof(hints));
    hints.ai_family     = AF_INET;
    hints.ai_socktype   = SOCK_STREAM;

    assert(getaddrinfo("localhost", "9090", &hints, &res) == 0);

    sockfd = socket(AF_INET, SOCK_STREAM, res->ai_protocol);

    assert(sockfd > 0);

    {
        unsigned opt =1;

        assert(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == 0);

        #ifdef SO_REUSEPORT
        assert(setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) == 0);
        #endif
    }

    assert(bind(sockfd, res->ai_addr, res->ai_addrlen) == 0);

    freeaddrinfo(res);

    /* Start to listen */
    (void)listen(sockfd, 5);

    {
        /* kevent set */
        struct kevent kevSet;
        /* events */
        struct kevent events[20];
        /* nevents */
        unsigned nevents;
        /* kq */
        int kq;
        /* buffer */
        char buf[20];
        /* length */
        ssize_t readlen;

        kevSet.data     = 5;    // backlog is set to 5
        kevSet.fflags   = 0;
        kevSet.filter   = EVFILT_READ;
        kevSet.flags    = EV_ADD;
        kevSet.ident    = sockfd;
        kevSet.udata    = NULL;

        assert((kq = kqueue()) > 0);

        /* Update kqueue */
        assert(kevent(kq, &kevSet, 1, NULL, 0, NULL) == 0);

        /* Enter loop */
        while (true) {
            /* Wait for events to happen */
            nevents = kevent(kq, NULL, 0, events, 20, NULL);

            assert(nevents >= 0);

            fprintf(stderr, "Got %u events to handle...\n", nevents);

            for (unsigned i = 0; i < nevents; ++i) {
                struct kevent event = events[i];
                int clientfd        = (int)event.ident;

                /* Handle disconnect */
                if (event.flags & EV_EOF) {

                    /* Simply close socket */
                    close(clientfd);

                    fprintf(stderr, "A client has left the server...\n");
                } else if (clientfd == sockfd) {
                    int nclientfd = accept(sockfd, NULL, NULL);

                    assert(nclientfd > 0);

                    /* Add to event list */
                    kevSet.data     = 0;
                    kevSet.fflags   = 0;
                    kevSet.filter   = EVFILT_READ;
                    kevSet.flags    = EV_ADD;
                    kevSet.ident    = nclientfd;
                    kevSet.udata    = NULL;

                    assert(kevent(kq, &kevSet, 1, NULL, 0, NULL) == 0);

                    fprintf(stderr, "A new client connected to the server...\n");

                    (void)write(nclientfd, "Welcome to this server!\n", 24);
                } else if (event.flags & EVFILT_READ) {

                    /* sleep for "processing" time */
                    readlen = read(clientfd, buf, sizeof(buf));

                    buf[readlen - 1] = 0;

                    fprintf(stderr, "bytes %zu are available to read... %s \n", (size_t)event.data, buf);

                    sleep(4);
                } else {
                    fprintf(stderr, "unknown event: %8.8X\n", event.flags);
                }
            }
        }
    }

    return 0;
} 
