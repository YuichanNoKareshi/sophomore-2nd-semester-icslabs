//name: Zhou Yitian
//login_ID: ics518030910237

#include "cachelab.h"
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stddef.h>

typedef struct 
{
    int valid_bits;
    unsigned tag;   //tag_bits
    int stamp;  //time_slot
}cache_line;

cache_line** cache = NULL;
int s,E,b,S;    //E:lines , b=log2B, B:bytes per line
int hit=0,miss=0,eviction=0;    
char* filename = NULL;

void init()//construct cache[S][E]
{
    cache = (cache_line**)malloc(sizeof(cache_line*)*S);//malloc cache[S][E]
    for(int i=0;i<S;i++)
        cache[i] = (cache_line*)malloc(sizeof(cache_line)*E);
    
    for(int i=0;i<S;i++)
    {
        for(int j=0;j<E;j++)
        {
            cache[i][j].valid_bits=0;
            cache[i][j].tag=0xffffffff;
            cache[i][j].stamp=0;

        }
    }
}

void update(unsigned address)
{
    unsigned s_address = (address>>b) & ((0xffffffff)>>(32-s));    //set index
    unsigned t_address = address>>(s+b);    //tag
    
    for (int i=0;i<E;i++)   //check if hit
    {
        if( cache[s_address][i].tag == t_address )  
        {
            cache[s_address][i].stamp = 0;
            hit++;
            return;
        }
    }

    for (int i=0;i<E;i++)   //check if hit but invalid
    {
        if( cache[s_address][i].valid_bits == 0 )
        {
            cache[s_address][i].tag = t_address;
            cache[s_address][i].valid_bits = 1;
            cache[s_address][i].stamp = 0;
            miss++;
            return;
        }
    }

    int max_stamp=0;
    int max_line=0;
    for (int i=0;i<E;i++)   //find lateset line
    {
        if( cache[s_address][i].stamp > max_stamp )
        {
            max_line = i;
            max_stamp = cache[s_address][i].stamp;
        }
    }

    eviction++;
    miss++;
    cache[s_address][max_line].tag = t_address;
    cache[s_address][max_line].stamp = 0;   //reset stamp

}

void time()
{
    for(int i=0;i<S;i++)
    {
        for(int j=0;j<E;j++)
        {
            if(cache[i][j].valid_bits == 1)
                cache[i][j].stamp++;    //increase valid datas
        }
    }
}

int main(int argc,char *argv[])
{
    int opt;
    while( (opt=getopt(argc,argv,"s:E:b:t:")) != -1 )
    {
        switch(opt)
        {
        case 's':
            s=atoi(optarg);
            break;
        case 'E':
            E=atoi(optarg);
            break;  
        case 'b':
            b=atoi(optarg);
            break;
        case 't':
            filename=optarg;
            break;
        default: break;
        }
    }

    S=1<<s;//s=logS
    init();
    
    FILE* file=fopen(filename,"r");
    if(file == NULL)
    {
        printf("Open file wrong");
        exit(-1);
    }

    char operation;
    unsigned address;
    int size;
    while (fscanf(file," %c %x,%d",&operation,&address,&size) > 0)
    {
        switch (operation)
        {
        case 'L':   //check
        case 'S':
            update(address);
            break;
        case 'M':   //M need to check twice
            update(address);
            update(address);
            break;
        default: break;
        }
        time();
    }

    for(int i=0;i<S;i++)
        free(cache[i]);
    free(cache);
    fclose(file);

    printSummary(hit,miss,eviction);
    return 0;
}
