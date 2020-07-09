// /*
//  * proxy.c - ICS Web proxy
//  *
//  *
//  */
#include "csapp.h"
#include <stdarg.h>
#include <sys/select.h>

/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400

ssize_t Rio_readn_w(int fd, void *ptr, size_t nbytes) 
{
    ssize_t n;
  
    if ((n = rio_readn(fd, ptr, nbytes)) < 0)
	printf("Rio_readn error");
    return n;
}

void Rio_writen_w(int fd, void *usrbuf, size_t n) 
{
    if (rio_writen(fd, usrbuf, n) != n)
	printf("Rio_writen error");
}

void Rio_readinitb_w(rio_t *rp, int fd)
{
    rio_readinitb(rp, fd);
} 

ssize_t Rio_readnb_w(rio_t *rp, void *usrbuf, size_t n) 
{
    ssize_t rc;

    if ((rc = rio_readnb(rp, usrbuf, n)) < 0)
	printf("Rio_readnb error");
    return rc;
}

ssize_t Rio_readlineb_w(rio_t *rp, void *usrbuf, size_t maxlen) 
{
    ssize_t rc;

    if ((rc = rio_readlineb(rp, usrbuf, maxlen)) < 0)
	printf("Rio_readlineb error");
    return rc;
} 

/*
 * Function prototypes
 */
int parse_uri(char *uri, char *target_addr, char *path, char *port);
void format_log_entry(char *logstring, struct sockaddr_in *sockaddr, char *uri, size_t size);
void build_http(int end_serverfd,char* path,char* port,rio_t* client_rio,char* method,char* version);
void doit(int connfd,struct sockaddr_in clientaddr);
void *thread(void *vargp);

typedef struct 
{
    int connfd;
    struct sockaddr_in clientaddr;
}thread_arg;

/*
 * main - Main routine for the proxy program
 */
int main(int argc, char **argv)
{
    /* Check arguments */
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port number>\n", argv[0]);
        exit(0);
    }

    int listenfd,connfd;
    socklen_t clientlen;
    struct sockaddr_in clientaddr;
    char hostname[MAXLINE],port[MAXLINE];
    pthread_t tid;
    
    listenfd=open_listenfd(argv[1]);
    while(1)
    {
        clientlen=sizeof(clientaddr);
        connfd=Accept(listenfd,(SA *)&clientaddr,&clientlen);

        getnameinfo((SA *)&clientaddr,clientlen,hostname,MAXLINE,port,MAXLINE,0);
        thread_arg* varg=(thread_arg*)malloc(sizeof(thread_arg));
        varg->connfd=connfd;
        varg->clientaddr=clientaddr;

        Pthread_create(&tid,NULL,thread,(void *)varg);
        
        // doit(connfd,clientaddr);
        // Close(connfd);
    }

}

void *thread(void *vargp)
{
    thread_arg* temp=(thread_arg*)vargp;
    int connfd = temp->connfd;
    struct sockaddr_in clientaddr = temp->clientaddr;
    Pthread_detach(pthread_self());
    doit(connfd,clientaddr);
    Close(connfd);
    return NULL;
}


/*
 * parse_uri - URI parser
 *
 * Given a URI from an HTTP proxy GET request (i.e., a URL), extract
 * the host name, path name, and port.  The memory for hostname and
 * pathname must already be allocated and should be at least MAXLINE
 * bytes. Return -1 if there are any problems.
 */
int parse_uri(char *uri, char *hostname, char *pathname, char *port)
{
    char *hostbegin;
    char *hostend;
    char *pathbegin;
    int len;

    if (strncasecmp(uri, "http://", 7) != 0) {
        hostname[0] = '\0';
        return -1;
    }

    /* Extract the host name */
    hostbegin = uri + 7;
    hostend = strpbrk(hostbegin, " :/\r\n\0");
    if (hostend == NULL)
        return -1;
    len = hostend - hostbegin;
    strncpy(hostname, hostbegin, len);
    hostname[len] = '\0';

    /* Extract the port number */
    if (*hostend == ':') {
        char *p = hostend + 1;
        while (isdigit(*p))
            *port++ = *p++;
        *port = '\0';
    } else {
        strcpy(port, "80");
    }

    /* Extract the path */
    pathbegin = strchr(hostbegin, '/');
    if (pathbegin == NULL) {
        pathname[0] = '\0';
    }
    else {
        pathbegin++;
        strcpy(pathname, pathbegin);
    }

    return 0;
}

/*
 * format_log_entry - Create a formatted log entry in logstring.
 *
 * The inputs are the socket address of the requesting client
 * (sockaddr), the URI from the request (uri), the number of bytes
 * from the server (size).
 */
void format_log_entry(char *logstring, struct sockaddr_in *sockaddr,
                      char *uri, size_t size)
{
    time_t now;
    char time_str[MAXLINE];
    char host[INET_ADDRSTRLEN];

    /* Get a formatted time string */
    now = time(NULL);
    strftime(time_str, MAXLINE, "%a %d %b %Y %H:%M:%S %Z", localtime(&now));

    if (inet_ntop(AF_INET, &sockaddr->sin_addr, host, sizeof(host)) == NULL)
        unix_error("Convert sockaddr_in to string representation failed\n");

    /* Return the formatted log entry string */
    sprintf(logstring, "%s: %s %s %zu", time_str, host, uri, size);
}


void doit(int connfd,struct sockaddr_in clientaddr)
{
    int end_serverfd;
    char buf[MAXLINE],method[MAXLINE],uri[MAXLINE],version[MAXLINE];
    char hostname[MAXLINE],path[MAXLINE];
    char port[MAXLINE];
    rio_t client_rio,server_rio;

    Rio_readinitb_w(&client_rio,connfd);
    Rio_readlineb_w(&client_rio,buf,MAXLINE);
    sscanf(buf,"%s %s %s",method,uri,version);

    if(strncmp(version,"HTTP/",5)!=0 || strlen(method)<3) return;

    parse_uri(uri,hostname,path,port);
    end_serverfd=open_clientfd(hostname,port);
    if(end_serverfd==-1)
    {
        printf("connection failed\n");
        return;
    }
    Rio_readinitb_w(&server_rio,end_serverfd);
    build_http(end_serverfd,path,port,&client_rio,method,version);
    
    int n;
    int length=0;
    int tot_len=0;
    n=Rio_readlineb_w(&server_rio,buf,MAXLINE);
    char res_head[MAXLINE]="";
    
    while(n > 0)
    {
        // Rio_writen(connfd,buf,n);
        tot_len+=n; 
        strcat(res_head,buf);

        if (!strncmp(buf,"Content-Length:",15))//if same
            sscanf(buf+15," %d",&length);
        else if (!strcmp(buf,"\r\n")) // end of header   
            break;
        
        n=Rio_readlineb_w(&server_rio,buf,MAXLINE);
    }
    if (n<0) {Close(end_serverfd);return;}
    Rio_writen_w(connfd,res_head,tot_len);

    for(int i=0;i<length;i++)
    {
        if(Rio_readnb_w(&server_rio,buf,1)<=0) 
            {Close(end_serverfd);return; }
        Rio_writen_w(connfd,buf,1);
    }
    

    tot_len+=length;
    char logMess[MAXLINE];
    format_log_entry(logMess,&clientaddr,uri,tot_len);
    printf("%s\n",logMess);
    Close(end_serverfd);
}

void build_http(int end_serverfd,char* path,char* port,rio_t* client_rio,char* method,char* version)
{
    char req_hdr[MAXLINE],buf[MAXLINE];
    sprintf(req_hdr,"%s /%s %s\r\n",method,path,version);
    // Rio_writen(end_serverfd,req_hdr,strlen(req_hdr));  
 
    int length=0;
    int n=Rio_readlineb_w(client_rio,buf,MAXLINE);
    while(n>0)
    {   
        // Rio_writen(end_serverfd,buf,n);
        strcat(req_hdr,buf);
        if (!strncmp(buf,"Content-Length:",15))//if same
            sscanf(buf+15," %d",&length);
        else if(!strcmp(buf,"\r\n")) break;//end of header
        n=Rio_readlineb_w(client_rio,buf,MAXLINE);
    }
    if(n<0) return;
    // if(length==0)
    Rio_writen_w(end_serverfd,req_hdr,strlen(req_hdr));
        
    if(length>0)
    {
        for(int i=0;i<length;i++)
        {
            if(Rio_readnb_w(client_rio,buf,1)<=0) 
                return;  
            Rio_writen_w(end_serverfd,buf,1);
        }
    }

    return;
}
