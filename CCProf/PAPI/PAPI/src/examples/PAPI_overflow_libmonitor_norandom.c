/*****************************************************************************
* This example shows how to use PAPI_overflow to set up an event set to      *
* begin registering overflows.
******************************************************************************/
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include "monitor.h"
#include "papi.h"		/* This needs to be included every time you use PAPI */
#include <pthread.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#define MAXBUFF 11500000
#define OVER_FMT    "handler(%d ) Overflow at %p! bit=0x%llx weight = %d\n"
#define THRESHOLD 100013	//3//000 //1000 for olden tsp 
#define ALLOCTHRESHOLD 1000
#define LIMITEDTHREADMONITORING 16
#define ERROR_RETURN(retval) { fprintf(stderr, "Error %d %s:line %d: \n", retval,__FILE__,__LINE__);  exit(retval); }
#define PROCESSIDENTIFIER -1
#define TOTALTHREAD 100
int SamplingThreshold = 100013;
pthread_mutex_t init_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t fini_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;
static int ldlat = 8;
int isPapiInitialized = 0;
bool isProcessEnd = false;
__thread int MapIndex;
__thread int thisEventSet;
//__thread int thisEventSet_p;
int thisEventSet_p;
struct timespec now, tmstart;
clock_t begin, end;
char programName[1000];
pthread_t bufferCheckerThread;
typedef enum {
   MALLOC = 0,
   FREE = 1,
   OVERFLOW = 2
}EVENTTYPE;
#define MAXFRAMESIZE 10

struct MallocEventData
{
  EVENTTYPE eventtype;
  int ThreadId;   

  size_t size;
  unsigned long handle;
  char array[MAXFRAMESIZE][300];
  int frameSize;
  struct timespec tmstop_alloc, tmstart_alloc;
  unsigned int MallocCodeAddress;
};

struct MallocEvent
{
  EVENTTYPE eventtype;
  int ThreadId;   

  size_t size;
  void *handle;
  void *array[MAXFRAMESIZE];
  int frameSize;
  struct timespec tmstop_alloc, tmstart_alloc;
};

struct OverFlowEventData
{
  EVENTTYPE eventtype;

  int EventSet;
  unsigned long address;
  unsigned long data_addr;
  unsigned long weight;
  unsigned long data_src;
  unsigned long cpu;
  unsigned long phys;
  unsigned long cacheSet;
  int freq;
 // struct timespec tmevent;
  unsigned long tmevent;
};

struct OverFlowEvent
{
  EVENTTYPE eventtype;

  int EventSet;
  void *address;
  void *data_addr;
  unsigned long weight;
  unsigned long data_src;
  unsigned long cpu;
  void* phys;
  unsigned long cacheSet;
  //struct timespec tmevent;
  unsigned long tmevent;
};

struct FreqOfPEBSEventLinkedList
{
  struct OverFlowEvent *this;
  int freq;
  int weight;
  struct InnerPEBSEventLinkedList *prev;
  struct InnerPEBSEventLinkedList *next;
};

struct InnerPEBSEventLinkedList
{
  struct OverFlowEvent *this;
  struct InnerPEBSEventLinkedList *prev;
  struct InnerPEBSEventLinkedList *next;
};
struct OverFlowEventLinkedList
{
  struct OverFlowEvent *this;
  struct OverFlowEventLinkedList *prev;
  struct OverFlowEventLinkedList *next;
};
struct MallocEventLinkedList
{
  struct MallocEvent *this;
  struct MallocEventLinkedList *prev;
  struct MallocEventLinkedList *next;

  struct InnerPEBSEventLinkedList *innerPEBSEventLinkedList;
  int totalPEBSEventsWithInMemRange;
};
typedef struct
{
  pid_t threadid;
  int PAPI_event; /* a place holder for an event preset */
  int EventSet;

  int total; /* we use total to track the amount of overflows that occured */
  int unWindZone;

  void *mallocEventLinkedList;
  int totalMallocEvents;

  void *overFlowEventLinkedList;//allocated buffer head 
  int totalOverflowEvents;//buffer space

  void *headPtrToOverFlowEventLinkedList;//where to make entry
  int totalEntryOfOverflowEvents;//total events that have been counted

  void *FreqOfPEBSEventLinkedList;
  int totalPEBSEventsInFreq;

  int isPapiRunning;
} ThreadEvent;
ThreadEvent ThreadEventMap[TOTALTHREAD];
int ThreadEventMapCurrentPtr;
int ThreadEventMapCurrentPtrRef;
#define MAXALLOCADDRESS 100
unsigned int allocAddreList[MAXALLOCADDRESS];
unsigned int allocAddreList_counter[MAXALLOCADDRESS];
int current_allocAddreList_ptr = 0;

//************************* dl_iterate_phdr **************
//
#if 1
#define _GNU_SOURCE
#include <link.h>
#include <stdlib.h>
#include <stdio.h>
struct dl_phdr_info
  {
    ElfW(Addr) dlpi_addr;
    const char *dlpi_name;
    const ElfW(Phdr) *dlpi_phdr;
    ElfW(Half) dlpi_phnum;

    /* Note: Following members were introduced after the first
       version of this structure was available.  Check the SIZE
       argument passed to the dl_iterate_phdr callback to determine
       whether or not each later member is available.  */

    /* Incremented when a new object may have been added.  */
    unsigned long long int dlpi_adds;
    /* Incremented when an object may have been removed.  */
    unsigned long long int dlpi_subs;

    /* If there is a PT_TLS segment, its module ID as used in
       TLS relocations, else zero.  */
    size_t dlpi_tls_modid;

    /* The address of the calling thread's instance of this module's
       PT_TLS segment, if it has one and it has been allocated
       in the calling thread, otherwise a null pointer.  */
    void *dlpi_tls_data;
  };

static int
callback(struct dl_phdr_info *i, size_t size, void *data) {
        printf("0x%8.8x %s 0x%8.8x %d %d %d %d 0x%8.8x\n",
        i->dlpi_addr, i->dlpi_name, i->dlpi_phdr, i->dlpi_phnum,
        i->dlpi_adds, i->dlpi_subs, i->dlpi_tls_modid,
        i->dlpi_tls_data
        );
 //     printf("relocation: 0x%lx\n", (long)i->dlpi_addr);
   //   printf("\n\n dl_phdr_info Size /%d\n", size);
        return 0;
}
#endif

//***************** looking for phys address **************************************//
#define PAGEMAP_ENTRY 8
#define GET_BIT(X,Y) (X & ((unsigned long)1<<Y)) >> Y
#define GET_PFN(X) X & 0x7FFFFFFFFFFFFF

const int __endian_bit = 1;
#define is_bigendian() ( (*(char*)&__endian_bit) == 0 )

unsigned long read_pagemap(char * path_buf, unsigned long virt_addr){

   int i, c, pid, status;
   //unsigned long virt_addr;
   unsigned long read_val, file_offset;
   //char path_buf [0x100] = {};
   FILE * f;
   char *end;

  // printf("Big endian? %d\n", is_bigendian());
   f = fopen(path_buf, "rb");
   if(!f){
      printf("Error! Cannot open %s\n", path_buf);
      return -1;
   }
   //printf("ThreadEventMap[MapIndex].unWindZone %d \n", ThreadEventMap[MapIndex].unWindZone);
   //Shifting by virt-addr-offset number of bytes
   //and multiplying by the size of an address (the size of an entry in pagemap file)
   file_offset = virt_addr / getpagesize() * PAGEMAP_ENTRY;
   //printf("Vaddr: 0x%lx, Page_size: %d, Entry_size: %d\n", virt_addr, getpagesize(), PAGEMAP_ENTRY);
   //printf("Reading %s at 0x%llx\n", path_buf, (unsigned long long) file_offset);
   status = fseek(f, file_offset, SEEK_SET);
   if(status){
      perror("Failed to do fseek!");
      return -1;
   }
   errno = 0;
   read_val = 0;
   unsigned char c_buf[PAGEMAP_ENTRY];
   for(i=0; i < PAGEMAP_ENTRY; i++){
      c = getc(f);
      if(c==EOF){
         printf("\nReached end of the file\n");
         return 0;
      }
      if(is_bigendian())
           c_buf[i] = c;
      else
           c_buf[PAGEMAP_ENTRY - i - 1] = c;
     // printf("[%d]0x%x ", i, c);
   }
   for(i=0; i < PAGEMAP_ENTRY; i++){
      //printf("%d ",c_buf[i]);
      read_val = (read_val << 8) + c_buf[i];
   }
   //printf("\n");
   unsigned long phys = (GET_PFN(read_val))<<12;
   phys = phys | (virt_addr % getpagesize());
   //printf("Result: 0x%llx physical address: 0x%llx virtual Address: 0x%llx \n", (unsigned long long) read_val, (unsigned long long)phys, (unsigned long long)virt_addr);
   //if(GET_BIT(read_val, 63))
#if 0
   if(GET_BIT(read_val, 63))
      printf("PFN: 0x%llx\n",(unsigned long long) GET_PFN(read_val));
   else
      printf("Page not present\n");
   if(GET_BIT(read_val, 62))
      printf("Page swapped\n");
#endif
   fclose(f);
   return phys;
}
//*******************************finish looking for phys add *************************************//
/* THis is the handler called by PAPI_overflow*/
//void
//handlerForProcess(int EventSet, void *address, void *data_addr, unsigned long weight,
//                   unsigned long data_src, unsigned long cpu, long long overflow_vector, void *context)
void
handlerForProcess (int EventSet, void *address,
                                void *data_addr, unsigned long cpu,
                                unsigned long tid, unsigned long time,
                                unsigned long weight, unsigned long src,
                                long long overflow_vector, void *context)
{
//  printf("FROM PEBS SMAPLING HANDLER=========================> PROCESS\n");
 if(data_addr==NULL )// || address > 0x100000000)// || data_src == 1745879362)
	return;
//  if(data_addr==NULL )
//	return;
  //ThreadEventMap[MapIndex].unWindZone = 1;
 //  fprintf(stderr, OVER_FMT, EventSet, address, overflow_vector,weight);
  // char path_buf [0x100] = {};
   //unsigned long phys = read_pagemap("/proc/self/pagemap", data_addr);
   //unsigned long cacheSet = ((unsigned long)phys>>6) & 0x3F;//6 bit L1 set
   //recordOverflowEvent(EventSet, address, data_addr, weight, data_src, cpu, overflow_vector, context);
   //recordOverflowEvent(EventSet, address, /*phys*/NULL, weight, data_src, cpu, data_addr, 0);
  // printf("FROM PEBS SMAPLING HANDLER=========================> PROCESS address %x\n",address);
   recordOverflowEvent(EventSet, address, /*phys*/NULL, weight, src, cpu, data_addr, time);
   ThreadEventMap[EventSet].total++;
   //ThreadEventMap[MapIndex].unWindZone = 3;
  // if(0==(ThreadEventMap[0].total%20))
//	printf("running %d \n",ThreadEventMap[0].total);
}

/* THis is the handler called by PAPI_overflow*/
//void
//handlerForThread(int EventSet, void *address, void *data_addr, unsigned long weight,
//                   unsigned long data_src, unsigned long cpu, long long overflow_vector, void *context)
void 
handlerForThread  (int EventSet, void *address,
                                void *data_addr, unsigned long cpu,
                                unsigned long tid, unsigned long time,
                                unsigned long weight, unsigned long src,
                                long long overflow_vector, void *context)
{
//	printf("FROM PEBS SMAPLING HANDLER=========================> THREAD\n");
	if(data_addr==NULL )//|| address > 0x10000000 )//|| data_src == 1745879362)
        	return;
//	printf("FROM PEBS SMAPLING HANDLER=========================> THREAD address %x\n", address);
	//ThreadEventMap[MapIndex].unWindZone = 1;
  	ThreadEventMap[EventSet].total++;
     //   unsigned long phys = read_pagemap("/proc/self/pagemap", data_addr);
      //  unsigned long cacheSet = ((unsigned long)phys>>6) & 0x3F;
	//recordOverflowEvent(EventSet, address, data_addr, weight, data_sVrc, cpu, overflow_vector, context);  
	//recordOverflowEvent(EventSet, address, /*phys*/NULL, weight, data_src, cpu, data_addr, 0);
	recordOverflowEvent(EventSet, address, /*phys*/NULL, weight, src, cpu, data_addr, time);
	//ThreadEventMap[MapIndex].unWindZone = 3;
}


void initPapi()
{
   int retval;
   ThreadEventMapCurrentPtr = -1;
   isPapiInitialized = 0;
   /****************************************************************************
   *  This part initializes the library and compares the version number of the *
   * header file, to the version of the library, if these don't match then it  *
   * is likely that PAPI won't work correctly.If there is an error, retval     *
   * keeps track of the version number.                                        *
   ****************************************************************************/

   if ((retval = PAPI_library_init (PAPI_VER_CURRENT)) != PAPI_VER_CURRENT)
   {
      printf("Library initialization error! %d \n",retval);
      exit(1);
   }
   
   isPapiInitialized = 1;
}

void monitorInit(bool forProcess)
{
   /* must be set to null before calling PAPI_create_eventset */

   char errstring[PAPI_MAX_STR_LEN];
   int retval, i;
   double tmp = 0;
   char event_name[PAPI_MAX_STR_LEN];
		
 //  pid_t self = PAPI_NULL;
   if(forProcess)
   {
	printf("for process\n");
      initPapi();
   //   self= PROCESSIDENTIFIER;
   }
   else
   {
	printf("for thread \n");
     // self= syscall(SYS_gettid);// MapIndex;
   }
	//printf("came here \n");   
        fflush(stdout);
	//printf("for thread 0 \n");
   ThreadEventMapCurrentPtr++;
   ThreadEventMap[ThreadEventMapCurrentPtr].threadid = ThreadEventMapCurrentPtr;
   ThreadEventMap[ThreadEventMapCurrentPtr].EventSet = PAPI_NULL;
  // thisEventSet = PAPI_NULL;
   ThreadEventMap[ThreadEventMapCurrentPtr].total = 0;
   ThreadEventMapCurrentPtrRef = ThreadEventMapCurrentPtr;
   ThreadEventMap[ThreadEventMapCurrentPtr].unWindZone = 0;
   MapIndex = ThreadEventMapCurrentPtr;
   ThreadEventMap[ThreadEventMapCurrentPtr].totalMallocEvents = 0;
   ThreadEventMap[ThreadEventMapCurrentPtr].totalOverflowEvents = 0;
   ThreadEventMap[ThreadEventMapCurrentPtr].totalPEBSEventsInFreq = 0;
   ThreadEventMap[ThreadEventMapCurrentPtr].totalEntryOfOverflowEvents = 0;
   ThreadEventMap[ThreadEventMapCurrentPtr].isPapiRunning = 0;
   /* Here we create the eventset */
	//printf("for thread 0 \n");
	//fflush(stdout);
//	int eventset = PAPI_NULL;
   if(forProcess)
   {
	thisEventSet_p = PAPI_NULL;
   	if ((retval=PAPI_create_eventset (&thisEventSet_p)) != PAPI_OK)
   	{
		printf("for Process error \n");fflush(stdout);
      		ERROR_RETURN(retval);
   	}
   }
   else
   {
	thisEventSet = PAPI_NULL;
	if ((retval=PAPI_create_eventset (&thisEventSet)) != PAPI_OK)
	{
        	printf("for thread error \n");fflush(stdout);
      		ERROR_RETURN(retval);
   	}

   }
	
//printf("for thread 1 \n");fflush(stdout);
   //retval = PAPI_event_name_to_code("MEM_TRANS_RETIRED:LATENCY_ABOVE_THRESHOLD", &ThreadEventMap[ThreadEventMapCurrentPtr].PAPI_event);
   retval = PAPI_event_name_to_code("MEM_LOAD_UOPS_RETIRED:L1_MISS", &ThreadEventMap[ThreadEventMapCurrentPtr].PAPI_event);
//   retval = PAPI_event_name_to_code("MEM_LOAD_UOPS_RETIRED:L3_MISS", &ThreadEventMap[ThreadEventMapCurrentPtr].PAPI_event);
   if ( retval != PAPI_OK ) {
      printf( "MEM_TRANS_RETIRED not found\n",retval );
   }
//printf("for thread 2 \n");
   /* Here we are querying for the existence of the PAPI MEM_TRANS_RETIRED  */
   if (PAPI_query_event (ThreadEventMap[ThreadEventMapCurrentPtr].PAPI_event) != PAPI_OK)
   {
      ThreadEventMap[ThreadEventMapCurrentPtr].PAPI_event = PAPI_TOT_CYC;

      if ((retval=PAPI_query_event (ThreadEventMap[ThreadEventMapCurrentPtr].PAPI_event)) != PAPI_OK)
         ERROR_RETURN(retval);

      printf ("PAPI_TOT_INS not available on this platform.");
      printf (" so subst PAPI_event with PAPI_TOT_CYC !\n\n");

   }


   /* PAPI_event_code_to_name is used to convert a PAPI preset from 
     its integer value to its string name. */
   if ((retval = PAPI_event_code_to_name (ThreadEventMap[ThreadEventMapCurrentPtr].PAPI_event, event_name)) != PAPI_OK)
      ERROR_RETURN(retval);
printf("event_name %s \n",event_name);
   /* add event to the event set */

   if(forProcess)
   {
	if ((retval = PAPI_add_event (thisEventSet_p, ThreadEventMap[ThreadEventMapCurrentPtr].PAPI_event)) != PAPI_OK)
      ERROR_RETURN(retval);
         /* register overflow and set up threshold */
   /* The threshold "THRESHOLD" was set to 100000 */
   if ((retval = PAPI_overflow (thisEventSet_p, ThreadEventMap[ThreadEventMapCurrentPtr].PAPI_event, SamplingThreshold, 0, handlerForProcess)) != PAPI_OK)
      ERROR_RETURN(retval);
   }
   else
   {
	if ( thisEventSet == PAPI_NULL)
		printf("\n\n\n error occured \n\n\n\n");
	if ((retval = PAPI_add_event (thisEventSet, ThreadEventMap[ThreadEventMapCurrentPtr].PAPI_event)) != PAPI_OK)
      		ERROR_RETURN(retval);
         /* register overflow and set up threshold */
   /* The threshold "THRESHOLD" was set to 100000 */
   if ((retval = PAPI_overflow (thisEventSet, ThreadEventMap[ThreadEventMapCurrentPtr].PAPI_event, SamplingThreshold, 0, handlerForThread)) != PAPI_OK)
      ERROR_RETURN(retval);
   }


   printf ("Here are the addresses at which overflows occured and overflow vectors \n");
   printf ("--------------------------------------------------------------\n");
   fflush(stdout);

  /* Start counting */
  ThreadEventMap[ThreadEventMapCurrentPtr].isPapiRunning = -1;
   if(isPapiInitialized==1 && ThreadEventMap[ThreadEventMapCurrentPtr].isPapiRunning==-1)
   {

	  if(forProcess)
	   {
		if ( (retval=PAPI_start (thisEventSet_p)) != PAPI_OK)
	      		ERROR_RETURN(retval);
	   	ThreadEventMap[ThreadEventMapCurrentPtr].isPapiRunning = 1;        
	   }
	   else
	   {
	   	if ( (retval=PAPI_start (thisEventSet)) != PAPI_OK)
	      		ERROR_RETURN(retval);
	   		ThreadEventMap[ThreadEventMapCurrentPtr].isPapiRunning = 1;
	   }
   }
}

int monitorEnd(bool forProcess)
{
   long long (values[2])[2];
   int retval;
	
 //  if(forProcess)
   //   printf("within Monitor fini Process from papi test- Probir MapIndex : %d and ThreadEventMapCurrentPtrRef %d , ThreadEventMap[MapIndex].EventSet %d\n", MapIndex,ThreadEventMapCurrentPtrRef,ThreadEventMap[MapIndex].EventSet);
   //else
     // printf("within Monitor fini Thread from papi test- Probir MapIndex : %d and ThreadEventMapCurrentPtrRef %d, ThreadEventMap[MapIndex].EventSet %d \n", MapIndex,ThreadEventMapCurrentPtrRef,ThreadEventMap[MapIndex].EventSet);
   /* Stops the counters and reads the counter values into the values array */
   if(!forProcess)
   {
	   if(isPapiInitialized==1 && ThreadEventMap[MapIndex].isPapiRunning==1)
	   {
		   if ( (retval=PAPI_stop (thisEventSet, values[0])) != PAPI_OK)
		   {
				printf("Error Papi stop reval %d",retval);
			ERROR_RETURN(retval);
		   }
		ThreadEventMap[MapIndex].isPapiRunning = 0;
	   
	   if ((retval = PAPI_overflow (thisEventSet, ThreadEventMap[MapIndex].PAPI_event, 0, 0,
						       NULL)) != PAPI_OK)
		      ERROR_RETURN(retval);

   /************************************************************************
    * PAPI_cleanup_eventset can only be used after the counter has been    *
    * stopped then it remove all events in the eventset                    *
    ************************************************************************/
	  if ( (retval=PAPI_cleanup_eventset (thisEventSet)) != PAPI_OK)
		      ERROR_RETURN(retval);

   	/* Free all memory and data structures, EventSet must be empty. */
	   if ( (retval=PAPI_destroy_eventset(&thisEventSet)) != PAPI_OK)
		      ERROR_RETURN(retval);
	}
   }
   else
   {
	if(isPapiInitialized==1 && ThreadEventMap[MapIndex].isPapiRunning==1)
           {
                   if ( (retval=PAPI_stop (thisEventSet_p, values[0])) != PAPI_OK)
                   {
                                printf("Error Papi stop reval %d",retval);
                        ERROR_RETURN(retval);
                   }
                ThreadEventMap[MapIndex].isPapiRunning = 0;
           
           if ((retval = PAPI_overflow (thisEventSet_p, ThreadEventMap[MapIndex].PAPI_event, 0, 0,
                                                       NULL)) != PAPI_OK)
                      ERROR_RETURN(retval);
	 //PAPI_overflow (thisEventSet_p, ThreadEventMap[MapIndex].PAPI_event, 0, 0,NULL);
   /************************************************************************
 *     * PAPI_cleanup_eventset can only be used after the counter has been    *
 *         * stopped then it remove all events in the eventset                    *
 *             ************************************************************************/
          if ( (retval=PAPI_cleanup_eventset (thisEventSet_p)) != PAPI_OK)
                      ERROR_RETURN(retval);

        /* Free all memory and data structures, EventSet must be empty. */
           if ( (retval=PAPI_destroy_eventset(&thisEventSet_p)) != PAPI_OK)
                      ERROR_RETURN(retval);

	}
   }
   if(!forProcess)
   {
	   retval = PAPI_unregister_thread( );
	   if ( retval != PAPI_OK )
	   {
		printf ("Error %d\n", retval);
	      pthread_mutex_unlock(&fini_mutex);
	      ERROR_RETURN( retval );
	   }
   }
   	fflush(stdout);
   printf ("values[0] %d\n", values[0]);
   printf (".totalOverflowEvents %d\n", ThreadEventMap[MapIndex].totalOverflowEvents);
   printf ("The total no of overflows was %d for mapindex %d\n", ThreadEventMap[MapIndex].total,MapIndex);
   printf ("The total no of overflows recorded %d\n", ThreadEventMap[MapIndex].totalEntryOfOverflowEvents);

   /* free the resources used by PAPI */
   if(ThreadEventMapCurrentPtrRef==0)
   { 
      PAPI_shutdown();
	//isPapiInitialized=0;
   }
	return PAPI_OK;
}

void printEventList(int index, EVENTTYPE eventType)
{
	struct MallocEvent *mallocEvent;
	struct OverFlowEvent *overflowEvent;
	struct MallocEventLinkedList* mallocEventPtr;
	struct OverFlowEventLinkedList* overFlowEventptr;

	int i=0;
	switch(eventType)
	{
		case MALLOC:
			for(i =0;i<ThreadEventMap[index].totalMallocEvents;i++ )
			{
				if(0==i)
				{
					mallocEventPtr = (struct MallocEventPtr *)ThreadEventMap[index].mallocEventLinkedList;
				}
				else
				{
					if(NULL!=mallocEventPtr->next)
					{
						mallocEventPtr = mallocEventPtr->next;					
					}
					else
					{
						printf("\ngoing out probir> \n");
						return;
					}
				}
				mallocEvent = mallocEventPtr->this;
				//printf("\n\n\index:%d EventType %d, size %d, handle %p\n",index, mallocEvent->eventtype,mallocEvent->size,mallocEvent->handle);
				//backtrace_symbols_fd(mallocEvent->array, mallocEvent->frameSize, STDOUT_FILENO);
			}
		break; 
		case OVERFLOW:
			for(i =0;i<ThreadEventMap[index].totalEntryOfOverflowEvents;i++ )
			{
				if(0==i)
				{
					overFlowEventptr = (struct OverFlowEventLinkedList *)ThreadEventMap[index].headPtrToOverFlowEventLinkedList;
				}
				else
				{
					if(NULL!=overFlowEventptr->next)
					{
						overFlowEventptr = overFlowEventptr->next;					
					}
					else
					{
						printf("\ngoing out probir> \n");
						return;
					}
				}
				overflowEvent = overFlowEventptr->this;
				printf("\n\n\index:%d EventType %d, EventSet %d, address %p, data_addr %p weight %d data_src %p cpu %d\n",index, overflowEvent->eventtype,overflowEvent->EventSet,overflowEvent->address,overflowEvent->data_addr,overflowEvent->weight,overflowEvent->data_src,overflowEvent->cpu);
			}
		break; 
	}
}
unsigned int findMallocPosition(struct MallocEventData tempMap)
{
	int j = 3;// for test mst set it 4
	const char *hexstring;
	bool found = true;
	int level = 2;
	char *ptr = strstr(tempMap.array[j], programName);
	while(ptr == NULL) {
		j++;
	//	printf("address : ---->%s \n",tempMap.array[j]);
		if(j>tempMap.frameSize)
		{
			if(level==0)
			{
		//	printf("address : ----> not found");
			found = false;
			break;
			}
			level--;
		}
		ptr = strstr(tempMap.array[j], programName);
		//printf("address : ---->%s \n",tempMap.array[j]);
	}
	if(ptr!=NULL)
	{
		//printf(" found address : ---->%s \n",tempMap.array[j]);
		char *ptr;
		ptr = (char *)malloc(strlen(tempMap.array[j]) * sizeof(char*));
		strcpy(ptr,tempMap.array[j]);
		hexstring=strtok(ptr, "[]");
		hexstring=strtok(NULL,"[]");
		
		unsigned int number = (unsigned int )strtol(hexstring, NULL, 0);
		free(ptr);
		return number;
	}
	return 0;
}
void storeData(int index, EVENTTYPE eventType, FILE* output)
{
	
	struct OverFlowEvent *overflowEvent;
	struct MallocEvent *mallocEvent;
	struct MallocEventLinkedList* mallocEventPtr;
	struct FreqOfPEBSEventLinkedList* freqOfPEBSEventLinkedList;

	int i=0;
	switch(eventType)
	{
		case MALLOC:
			printf("ThreadEventMap[index:%d].totalMallocEvents---------> %d\n",index,ThreadEventMap[index].totalMallocEvents);
			for(i =0;i<ThreadEventMap[index].totalMallocEvents;i++ )
			{
				if(0==i)
				{
					mallocEventPtr = (struct MallocEventPtr *)ThreadEventMap[index].mallocEventLinkedList;
				}
				else
				{
					if(NULL!=mallocEventPtr->next)
					{
						mallocEventPtr = mallocEventPtr->next;					
					}
					else
					{
						printf("\ngoing out probir> \n");
						return;
					}
				}
				if(NULL!=mallocEventPtr->this)
					mallocEvent = mallocEventPtr->this;
				else
				{
					printf("NULL Found");
					return;
				}
			//	printf("\n\nindex:%d EventType %d, size %d, handle 0x%x and data->MallocCodeAddress\n",index, mallocEvent->eventtype,mallocEvent->size,mallocEvent->handle,mallocEvent->MallocCodeAddress);
				//fflush(stdout);
				struct MallocEventData data;
				data.ThreadId = index;
				data.eventtype = mallocEvent->eventtype;
				data.size = mallocEvent->size;
				data.handle = mallocEvent->handle;
				data.frameSize = mallocEvent->frameSize;
				data.tmstop_alloc = mallocEvent->tmstop_alloc;
				data.tmstart_alloc = mallocEvent->tmstart_alloc;
				//printf(" %d %lu %lu  %d\n",data.size,data.handle,mallocEvent->handle , data.tmstart_alloc);			
				char **strings;
				strings = backtrace_symbols (mallocEvent->array, mallocEvent->frameSize);
	
				for (i = 0; i < mallocEvent->frameSize; i++)
				{
//					printf ("%s\n", strings[i]);
					memcpy(data.array[i], strings[i], strlen(strings[i])+1);
				}
				free (strings);
				fflush(stdout);
				data.MallocCodeAddress = 0;//findMallocPosition(data);
				//printf("came here \n");
				fflush(stdout);
				fwrite(&data, sizeof(data), 1, output);

			}
	//		printf("finsihing Malloc storing \n");
	//		fflush(stdout);
		break; 
		case OVERFLOW:
			printf("ThreadEventMap[index: %d].totalPEBSEventsInFreq---------> L1 Miss %d\n",index,ThreadEventMap[index].totalPEBSEventsInFreq);
			for(i =0;i<ThreadEventMap[index].totalPEBSEventsInFreq;i++ )
			{
				if(0==i)
				{
					freqOfPEBSEventLinkedList = (struct FreqOfPEBSEventLinkedList *)ThreadEventMap[index].FreqOfPEBSEventLinkedList;
				}
				else
				{
					if(NULL!=freqOfPEBSEventLinkedList->next)
					{
						freqOfPEBSEventLinkedList = freqOfPEBSEventLinkedList->next;					
					}
					else
					{
						printf("\ngoing out probir> \n");
						return;
					}
				}
				if(NULL!=freqOfPEBSEventLinkedList->this)
					overflowEvent = freqOfPEBSEventLinkedList->this;
				else
				{
					printf("NULL Found in overflow");
					return;
				}
				//printf("\n\n\i: %d index:%d EventType %d, EventSet %d, address %p, data_addr 0x%x weight %d freq %d data_src %p cpu %d\n",i,index, overflowEvent->eventtype,overflowEvent->EventSet,overflowEvent->address,overflowEvent->data_addr,freqOfPEBSEventLinkedList->weight,freqOfPEBSEventLinkedList->freq,overflowEvent->data_src,overflowEvent->cpu);
				//fflush(stdout);
				struct OverFlowEventData data;
				data.eventtype = overflowEvent->eventtype;
				
				data.EventSet = overflowEvent->EventSet;
				data.address = overflowEvent->address;
				data.data_addr = overflowEvent->data_addr;

				data.weight = freqOfPEBSEventLinkedList->weight;
				data.freq = freqOfPEBSEventLinkedList->freq;

				data.data_src = overflowEvent->data_src;
				data.cpu = overflowEvent->cpu;
				data.phys = overflowEvent->phys;
				data.cacheSet = overflowEvent->cacheSet;
				data.tmevent = overflowEvent->tmevent;
			
		//		printf("total bytes written %d\n ",fwrite(&data, sizeof(data), 1, output));
				fwrite(&data, sizeof(data), 1, output);


				//backtrace address location
				//140737348726134
			}
		break; 
	}
}

void saveInFile()
{
	FILE* output;
	int index = 0;
	output = fopen("DataMalloc0.dat", "wb");
	for(index = 0; index<=ThreadEventMapCurrentPtr;index++)
   	{
		storeData(index, MALLOC,output);
	}
	fclose(output);
	printf("came here--------------------------------------------->\n");
	output = fopen("DataPEBS0.dat", "wb");
	for(index = 0; index<=ThreadEventMapCurrentPtr;index++)
   	{
		storeData(index, OVERFLOW,output);
	}
	fclose(output);
	printf("finished here=============================================>\n");
}


void printBacktrace(void *array[], int frameSize)
{
	char **strings;
	int i;
	 strings = backtrace_symbols (array, frameSize);

  //printf ("Obtained %zd stack frames.\n", size);

  for (i = 0; i < frameSize; i++)
     printf ("%s\n", strings[i]);

  free (strings);
}
struct MallocEvent *findMemoryAllocForMemLocation(struct OverFlowEvent *overflowEvent,int freq)
{
	struct MallocEvent *mallocEvent;
	struct MallocEventLinkedList* mallocEventPtr;
	int index, i;
	//if(freq==1)
	//	return NULL;
	//printf("here 1: ThreadEventMapCurrentPtr %d \n",ThreadEventMapCurrentPtr);
	for(index = 0; index<=ThreadEventMapCurrentPtr;index++)
	{	
		//printf("here ThreadEventMap[index].totalMallocEvents %d index %d\n",ThreadEventMap[index].totalMallocEvents,index);
		for(i =0;i<ThreadEventMap[index].totalMallocEvents;i++ )
		{
		//	printf("here 2\n");
			if(0==i)
				{
					mallocEventPtr = (struct MallocEventLinkedList *)ThreadEventMap[index].mallocEventLinkedList;
				}
				else
				{
					if(NULL!=mallocEventPtr->next)
					{
						mallocEventPtr = mallocEventPtr->next;					
					}
					else
					{
						printf("\ngoing out probir findMemoryAllocForMemLocation ************************> \n");
						return;
					}
				}
			
			if(mallocEventPtr!=NULL ){
				mallocEvent = mallocEventPtr->this;
//				unsigned int k = mallocEvent->handle;
				if(overflowEvent->data_addr > mallocEvent->handle && overflowEvent->data_addr < (mallocEvent->handle + mallocEvent->size))// && mallocEvent->ThreadId!=overflowEvent->EventSet	)
				{
#if 1
					printf("Inside \n");
					printf("Range: %p %u ---", mallocEvent->handle,mallocEvent->handle);//%x %p 
								
				printf(" %p %u: TID <%d> ",mallocEvent->handle + mallocEvent->size, mallocEvent->handle + mallocEvent->size,mallocEvent->ThreadId);
					//printf("Value: %p :\n",overflowEvent->data_addr);
				printf("Value: %p %u: TID: <%d> Freq %d weight %d\n",overflowEvent->data_addr,overflowEvent->data_addr,overflowEvent->EventSet,freq,overflowEvent->weight);
				
					printBacktrace(mallocEvent->array, mallocEvent->frameSize);
					return mallocEvent;
#endif 
//#endif
				}
			//	else
				{
			//		printf("Outside %p\n",overflowEvent->data_addr);		
				}
			}
			else
			{
				return NULL;
			}
		}
	}
	return NULL;
}
void getUniquePEBSEventList()
{
	struct FreqOfPEBSEventLinkedList* FreqOfPEBSEventptr;
	struct FreqOfPEBSEventLinkedList* temp;
	struct OverFlowEventLinkedList* overFlowEventptr;
	struct OverFlowEvent *overflowEvent;
	int index,i;
	//printf("ThreadEventMapCurrentPtr---------------------> %d\n",ThreadEventMapCurrentPtr);
	for(index = 0; index<=ThreadEventMapCurrentPtr;index++)
	{	
		printf("Thread index %d : event Count ------------------------------->%d\n",index, ThreadEventMap[index].totalEntryOfOverflowEvents);		
		for(i =0;i<ThreadEventMap[index].totalEntryOfOverflowEvents;i++ )
		{
	//		printf("index -------------------------_>%d\n", i);
			if(0==i)
			{
				overFlowEventptr = (struct OverFlowEventLinkedList *)ThreadEventMap[index].headPtrToOverFlowEventLinkedList;
			}
			else
			{
				if(NULL!=overFlowEventptr->next)
				{
					overFlowEventptr = overFlowEventptr->next;					
				}
				else
				{
					printf("\ngoing out probir getUniquePEBSEventList ************************> \n");
					return;
				}
			}
			overflowEvent = overFlowEventptr->this;
			bool found = false;
#if 0
			int l = 0;
			for(l=0;l<ThreadEventMap[index].totalPEBSEventsInFreq;l++)
			{
				
				if(0!=ThreadEventMap[index].totalPEBSEventsInFreq)
				{
					if(0==l)
						temp = ((struct FreqOfPEBSEventLinkedList *)ThreadEventMap[index].FreqOfPEBSEventLinkedList);
					if(((struct OverFlowEvent *)temp->this)->data_addr != overflowEvent->data_addr)
					{
						if(NULL!=temp->next)
						{
							temp = temp->next;
							continue;						
						}
						else
							break;
					}
					else
					{
						found = true;
						temp->weight = temp->weight + overflowEvent->weight;
						temp->freq++;
						break;
					}
				}
			}
#endif
			if(!found)
			{
				FreqOfPEBSEventptr = (struct FreqOfPEBSEventLinkedList *)malloc(sizeof(struct FreqOfPEBSEventLinkedList));
				FreqOfPEBSEventptr->this = overflowEvent;
				FreqOfPEBSEventptr->freq = 1;
				FreqOfPEBSEventptr->weight = overflowEvent->weight;
				//printf("address %p \n", overflowEvent->data_addr);
				if(overflowEvent->data_addr==NULL)// || overflowEvent->data_addr > 0x800000000000)
					break;
				//printf("ThreadEventMap[index].totalPEBSEventsInFreq %d \n",ThreadEventMap[index].totalPEBSEventsInFreq);
				if(0==ThreadEventMap[index].totalPEBSEventsInFreq)
				{
					FreqOfPEBSEventptr->next = NULL;
					FreqOfPEBSEventptr->prev = NULL;
					ThreadEventMap[index].FreqOfPEBSEventLinkedList = FreqOfPEBSEventptr;
				}
				else
				{
					//printf("came here ----->  \n");
					((struct FreqOfPEBSEventLinkedList *)ThreadEventMap[index].FreqOfPEBSEventLinkedList)->prev = FreqOfPEBSEventptr;
					FreqOfPEBSEventptr->next = ((struct FreqOfPEBSEventLinkedList *)ThreadEventMap[index].FreqOfPEBSEventLinkedList);
					FreqOfPEBSEventptr->prev = NULL;
					ThreadEventMap[index].FreqOfPEBSEventLinkedList = FreqOfPEBSEventptr;
				}
				ThreadEventMap[index].totalPEBSEventsInFreq++;
				
			}
		}
	}

}
void MapPEBSEventsToMemoryAlloc()
{
	struct MallocEvent *mallocEvent;
	struct OverFlowEvent *overflowEvent;
	struct MallocEventLinkedList* mallocEventPtr;
	struct OverFlowEventLinkedList* overFlowEventptr;
	struct InnerPEBSEventLinkedList* innerPEBSEventPtr;
	struct FreqOfPEBSEventLinkedList* FreqOfPEBSEventptr;
	
	int index, i;
	printf("MapPEBSEventsToMemoryAlloc\n");
	
#if 1
	for(index = 0; index<=ThreadEventMapCurrentPtr;index++)
	{	
		for(i =0;i<ThreadEventMap[index].totalPEBSEventsInFreq;i++ )
		{
			if(0==i)
			{
				FreqOfPEBSEventptr = (struct FreqOfPEBSEventLinkedList *)ThreadEventMap[index].FreqOfPEBSEventLinkedList;
			}
			else
			{
				if(NULL!=overFlowEventptr->next)
				{
					FreqOfPEBSEventptr = FreqOfPEBSEventptr->next;					
				}
				else
				{
					printf("\ngoing out probir MapPEBSEventsToMemoryAlloc ************************> \n");
					return;
				}
			}
			overflowEvent = FreqOfPEBSEventptr->this;
			if(NULL!=overflowEvent)
			{
				//printf("overflowEvent addr %p, EventSet %d\n",overflowEvent->data_addr,overflowEvent->EventSet);
				findMemoryAllocForMemLocation(overflowEvent,FreqOfPEBSEventptr->freq);//overflowEvent->data_addr
				fflush(stdout); 
			}
			
		}
	}
#endif
}

void bufferLevelChecker()
{
	int i = 0;
	//printf("came here\n");
	while(!isProcessEnd)
	{
		for(i=0;i<ThreadEventMapCurrentPtr;i++)
		{
			if((ThreadEventMap[i].totalEntryOfOverflowEvents/ThreadEventMap[i].totalOverflowEvents)<0.5)
				createEventBuffer(i,MAXBUFF);
		}
		sleep(10);
	}
}

void *
monitor_init_process(int *argc, char **argv, void *data)
{
   int retval;
/*
   int err;
   char *s="bufferCheckerThread";
   err = pthread_create(&bufferCheckerThread, NULL, &bufferLevelChecker, &s);
   if (err != 0)
       printf("\ncan't create thread :[%s]", strerror(err));
   else
       printf("\n Thread created successfully\n");

*/
   //read from config file to get executing program name
   FILE* input;
   printf("Client>> Monitoring %s \n",programName);
   fflush(stdout);

#if 1
        dl_iterate_phdr(callback, NULL);
#endif

   input = fopen("memToolInfoForClient", "rb");
   if (input==NULL) {printf ("Client>> File error: Could not read info file \"memToolInfoForClient\""); exit (1);}
   //fseek (input , 0 , SEEK_END);
   //int lSize = ftell (input);
   //fseek(input, 0, SEEK_SET);
   fscanf(input,"%[^\n]",programName);
   
   // allocate memory to contain the whole file:
  // programName = (char*) malloc (sizeof(char)*(lSize-1));
  // if (programName == NULL) {printf("Client>> Memory error"); fflush(stdout);exit (2);}
  // if(0==fread(programName, sizeof(char), lSize, input))
   //{
	//programName[lSize] = '\0';
   	//printf("Client>> Monitoring %s \n",programName);
	//fflush(stdout);
   //}
   //else
   {
//	printf("Client>> Failed to determine program name... Aborting ... \n");
//	fclose(input);
//	exit(0);
   }
  // else
	printf("Client>> Monitoring %s \n",programName);
   fclose(input);
   printf("STARTING NEW PROCESS ===============================================================>");
   input = fopen("SampleRateThreshold", "rb");
   if (input==NULL) {printf ("Client>> File error: Could not read info file \"SampleRateThreshold\""); exit (1);}
   fscanf(input,"%d",&SamplingThreshold);
    fclose(input);
   printf("\n\nsetting sampleing threashold to %d --------------------------> \n", SamplingThreshold);
   clock_gettime(CLOCK_REALTIME, &tmstart);
   begin = clock();

   pthread_mutex_lock(&init_mutex);
   //monitorInit for process

   monitorInit(true);
   pthread_mutex_unlock(&init_mutex);
   //create thread to check buffer
   
   
   createEventBuffer(0,MAXBUFF);
   printf("ThreadEventMap[MapIndex].totalOverflowEvents %d\n",ThreadEventMap[0].totalOverflowEvents);
   fflush(stdout);
}
void
monitor_fini_process(int how, void *data)
{
   printf("process %d exiting--->how %d \n\n",MapIndex,how);
   printf("FINSHING  PROCESS ===============================================================> EXITING");
   pthread_mutex_lock(&fini_mutex);
   monitorEnd(true);
	ThreadEventMapCurrentPtrRef--;
   pthread_mutex_unlock(&fini_mutex);
   
   int index;
   getUniquePEBSEventList();
  // for(index = 0; index<=ThreadEventMapCurrentPtr;index++)
   {
    //  printEventList(index,MALLOC);
   //   printEventList(index,OVERFLOW);
   }
   //MapPEBSEventsToMemoryAlloc();
	clock_gettime(CLOCK_REALTIME, &now);
	saveInFile();
   isProcessEnd = true;
	end = clock();
	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	double seconds = (double)((now.tv_sec+now.tv_nsec*1e-9) - (double)(tmstart.tv_sec+tmstart.tv_nsec*1e-9));
    printf("\nwall time %fs\n", seconds);
	printf("\nClock time %fs\n", time_spent);
	
}
void* 
monitor_init_thread(int tid, void* data)
{
   int retval;
   MapIndex = 0;//syscall(SYS_gettid);
  
   pthread_mutex_lock(&init_mutex);
#ifdef LIMITEDTHREADMONITORING
	if(ThreadEventMapCurrentPtr>LIMITEDTHREADMONITORING)
   	{		
		MapIndex = -1;
		pthread_mutex_unlock(&init_mutex);
		return;
	}
#endif
	
	 printf("STARTING NEW THREAD ===============================================================>");
   retval = PAPI_thread_init( ( unsigned long ( * )( void ) )( pthread_self ) );
   if ( retval != PAPI_OK ) {
      ERROR_RETURN( retval );
   }
   retval = PAPI_register_thread( );
   if ( retval != PAPI_OK ) {
      ERROR_RETURN( retval );
   }
   monitorInit(false);
	//	printf("kjhagfk\n");
   //ThreadEventMapCurrentPtr
   pthread_mutex_unlock(&init_mutex);
   //pthread_mutex_lock(&init_mutex);
	//	if(1==ThreadEventMap[MapIndex].unWindZone)
		{
	//			pthread_mutex_unlock(&init_mutex);
	//			return;
		}
	//	ThreadEventMap[MapIndex].unWindZone = 1;
	//	pthread_mutex_unlock(&init_mutex);

  

   
	//ThreadEventMap[MapIndex].unWindZone = 0;
	createEventBuffer(MapIndex,MAXBUFF);
   
}

void
monitor_fini_thread(void* init_thread_data)
{
   int retval;
#ifdef LIMITEDTHREADMONITORING
	if(MapIndex ==-1)
		return;
#endif
 //  printf("----------->in thread %d\n",MapIndex);
   pthread_mutex_lock(&fini_mutex);
   
   //monitorEnd for thread	
   retval = monitorEnd(false);
   if(retval  != PAPI_OK)
   {
	pthread_mutex_unlock(&fini_mutex);
	ERROR_RETURN(retval);
   }
   ThreadEventMapCurrentPtrRef--;
   pthread_mutex_unlock(&fini_mutex);
   printf("FINSHING  THREAD ===============================================================> EXITING");
   //print_trace ();
	//printEventList(MALLOC);
	//printEventList(OVERFLOW);
}

void createEventBuffer(int threadIndex,int howManyEvents)
{
	while(howManyEvents--)
	{
		struct OverFlowEventLinkedList* overFlowEventptr;
        	struct OverFlowEvent *overflowEvent;	
		//pthread_mutex_lock(&init_mutex);
		if(0 != pthread_mutex_trylock(&init_mutex))
                	return;
		if(1==ThreadEventMap[MapIndex].unWindZone)
		{
				pthread_mutex_unlock(&init_mutex);
				return;
		}
		ThreadEventMap[MapIndex].unWindZone = 1;
		//pthread_mutex_unlock(&init_mutex);
	
		overflowEvent = (struct OverFlowEvent *)malloc(sizeof(struct OverFlowEvent));

		overFlowEventptr = (struct OverFlowEventLinkedList *)malloc(sizeof(struct OverFlowEventLinkedList));
		overFlowEventptr->this = overflowEvent;	
		if(0==ThreadEventMap[threadIndex].totalOverflowEvents)
		{
			overFlowEventptr->prev = NULL;
			overFlowEventptr->next = NULL;
			ThreadEventMap[threadIndex].overFlowEventLinkedList = overFlowEventptr;
			ThreadEventMap[threadIndex].headPtrToOverFlowEventLinkedList = overFlowEventptr;
		}
		else
		{
			((struct OverFlowEventLinkedList *)ThreadEventMap[threadIndex].overFlowEventLinkedList)->prev = overFlowEventptr;
			overFlowEventptr->next = ((struct OverFlowEventLinkedList *)ThreadEventMap[threadIndex].overFlowEventLinkedList);
			overFlowEventptr->prev = NULL;
			ThreadEventMap[threadIndex].overFlowEventLinkedList = overFlowEventptr;
			//printf("hello ==================================> %d \n",EventSet);	
		}
		ThreadEventMap[MapIndex].totalOverflowEvents++;	

		//pthread_mutex_lock(&init_mutex);
		ThreadEventMap[MapIndex].unWindZone = 3;//replaceing with 3 instead of 0: ready to run
		pthread_mutex_unlock(&init_mutex);
	}
}
void recordOverflowEvent(int EventSet, void *address, void *phys, unsigned long weight,
                   unsigned long data_src, unsigned long cpu, void *data_addr, unsigned long time_)
{
	//recordOverflowEvent(EventSet, address, phys, weight, data_src, cpu, data_addr, cacheSet);
	  //printf(" Within recordOverflowEvent\n");
	 if(0 != pthread_mutex_trylock(&init_mutex))
		return;
	//assuming no library function will be called. No unwinding safety check
	if(NULL==ThreadEventMap[MapIndex].headPtrToOverFlowEventLinkedList || !(3==ThreadEventMap[MapIndex].unWindZone||0==ThreadEventMap[MapIndex].unWindZone||2==ThreadEventMap[MapIndex].unWindZone) || (1==ThreadEventMap[MapIndex].unWindZone) || (4==ThreadEventMap[MapIndex].unWindZone))
	{
		pthread_mutex_unlock(&init_mutex);
		return;
	}
	ThreadEventMap[MapIndex].unWindZone = 1;//busy... dont try to record alloc
        ///find page map here
      //  printf(" data address %x and weight %lu \n",data_addr,weight);
        unsigned long phys1 = data_addr;//read_pagemap("/proc/self/pagemap", data_addr);
   	unsigned long cacheSet = ((unsigned long)phys1>>6) & 0x3F;
 //        unsigned long phys1 = 0;
        //unsigned long cacheSet = ((unsigned long)data_addr>>6) & 0x3F;
//	if(cacheSet != cacheSet1)
	{
	//	printf("virtual and phys address cache Set dont match\n");
//		ThreadEventMap[MapIndex].unWindZone = 3;//done working... now you can record alloc
  //                      pthread_mutex_unlock(&init_mutex);
    //                    return;
	}
	
	if(NULL!=((struct OverFlowEvent *)((struct OverFlowEventLinkedList *)ThreadEventMap[MapIndex].headPtrToOverFlowEventLinkedList)->this)->address)//not empty
	{
		if(NULL!=(((struct OverFlowEventLinkedList *)ThreadEventMap[MapIndex].headPtrToOverFlowEventLinkedList)->prev))
		{
			ThreadEventMap[MapIndex].headPtrToOverFlowEventLinkedList = ((struct OverFlowEventLinkedList *)ThreadEventMap[MapIndex].headPtrToOverFlowEventLinkedList)->prev;
		}
		else
		{
			ThreadEventMap[MapIndex].unWindZone = 3;//done working... now you can record alloc
			pthread_mutex_unlock(&init_mutex);
			return;
		}
	}
	((struct OverFlowEvent *)((struct OverFlowEventLinkedList *)ThreadEventMap[MapIndex].headPtrToOverFlowEventLinkedList)->this)->eventtype = OVERFLOW;
	((struct OverFlowEvent *)((struct OverFlowEventLinkedList *)ThreadEventMap[MapIndex].headPtrToOverFlowEventLinkedList)->this)->EventSet = EventSet;
	((struct OverFlowEvent *)((struct OverFlowEventLinkedList *)ThreadEventMap[MapIndex].headPtrToOverFlowEventLinkedList)->this)->address = address;
	((struct OverFlowEvent *)((struct OverFlowEventLinkedList *)ThreadEventMap[MapIndex].headPtrToOverFlowEventLinkedList)->this)->data_addr = data_addr;
	((struct OverFlowEvent *)((struct OverFlowEventLinkedList *)ThreadEventMap[MapIndex].headPtrToOverFlowEventLinkedList)->this)->weight = weight;//ThreadEventMap[MapIndex].isPapiRunning;//
	((struct OverFlowEvent *)((struct OverFlowEventLinkedList *)ThreadEventMap[MapIndex].headPtrToOverFlowEventLinkedList)->this)->data_src = data_src;
	((struct OverFlowEvent *)((struct OverFlowEventLinkedList *)ThreadEventMap[MapIndex].headPtrToOverFlowEventLinkedList)->this)->cpu = cpu;
	((struct OverFlowEvent *)((struct OverFlowEventLinkedList *)ThreadEventMap[MapIndex].headPtrToOverFlowEventLinkedList)->this)->phys = phys1;
	((struct OverFlowEvent *)((struct OverFlowEventLinkedList *)ThreadEventMap[MapIndex].headPtrToOverFlowEventLinkedList)->this)->cacheSet = cacheSet;
	//clock_gettime(CLOCK_REALTIME, &((struct OverFlowEvent *)((struct OverFlowEventLinkedList *)ThreadEventMap[MapIndex].headPtrToOverFlowEventLinkedList)->this)->tmevent);
	((struct OverFlowEvent *)((struct OverFlowEventLinkedList *)ThreadEventMap[MapIndex].headPtrToOverFlowEventLinkedList)->this)->tmevent = time_;
	
	ThreadEventMap[MapIndex].totalEntryOfOverflowEvents++;
        ThreadEventMap[MapIndex].unWindZone = 3;//done working... now you can record alloc
	pthread_mutex_unlock(&init_mutex);
}
void recordMallocEvent(size_t size, void *handle)
{
	struct MallocEvent *mallocEvent;
	mallocEvent = (struct MallocEvent *)malloc(sizeof(struct MallocEvent));
	mallocEvent->eventtype = MALLOC;
	mallocEvent->size = size;
	mallocEvent->handle = handle;
	mallocEvent->frameSize = backtrace (mallocEvent->array, MAXFRAMESIZE);
/* was testing for hotspot
	if(size == 1600)
	{
		 printBacktrace(mallocEvent->array,mallocEvent->frameSize);
		printf("framesize %d \n\n",mallocEvent->frameSize);
	}
*/
	mallocEvent->ThreadId = MapIndex;
	clock_gettime(CLOCK_REALTIME, &mallocEvent->tmstart_alloc);
	mallocEvent->tmstop_alloc.tv_sec = 0;
	mallocEvent->tmstop_alloc.tv_nsec = 0;

	struct MallocEventLinkedList* MallocEventPtr;
	MallocEventPtr = (struct MallocEventLinkedList *)malloc(sizeof(struct MallocEventLinkedList));
	MallocEventPtr->this = mallocEvent;
	MallocEventPtr->totalPEBSEventsWithInMemRange = 0;
	if(0==ThreadEventMap[MapIndex].totalMallocEvents)
	{
		MallocEventPtr->prev = NULL;
		MallocEventPtr->next = NULL;
		ThreadEventMap[MapIndex].mallocEventLinkedList = MallocEventPtr;
	}
	else
	{
		((struct MallocEventLinkedList *)ThreadEventMap[MapIndex].mallocEventLinkedList)->prev = MallocEventPtr;
		MallocEventPtr->next = ((struct MallocEventLinkedList *)ThreadEventMap[MapIndex].mallocEventLinkedList);
		MallocEventPtr->prev = NULL;
		ThreadEventMap[MapIndex].mallocEventLinkedList = MallocEventPtr;
	}
	ThreadEventMap[MapIndex].totalMallocEvents++;
}
#if 0
#define RANDOM_ALLOC_MONITOR 1
#ifdef RANDOM_ALLOC_MONITOR
void
monitor_pre_malloc(size_t size)
{
#if 1
	if(size < 0)
		return;
        if(MapIndex>=0)
        {
	//	pthread_mutex_lock(&init_mutex);
//DoItAgain:
		if(0 != pthread_mutex_trylock(&init_mutex))
		{
			//if(size == 1600)
			//printf("returning as failed at trylock %d\n",size);
                	return;
		}
                if( 1==ThreadEventMap[MapIndex].unWindZone || 0==ThreadEventMap[MapIndex].unWindZone || 4== ThreadEventMap[MapIndex].unWindZone) // donot reenter if the processing is busy or have not finished assigned job or already asked to record something.
                {
			if(size == 1600)
                        printf("returning as condition did not met %d  unwindzone %d \n",size,ThreadEventMap[MapIndex].unWindZone);
			pthread_mutex_unlock(&init_mutex);
			return;
		/*	if(0==ThreadEventMap[MapIndex].unWindZone)
			{
				pthread_mutex_unlock(&init_mutex);
				goto DoItAgain;
			}
			else
			{
				pthread_mutex_unlock(&init_mutex);
                                return;
			}
		*/
                }
	//	printf("ThreadEventMap[%d].unWindZone: %d\n",MapIndex,ThreadEventMap[MapIndex].unWindZone);
                ThreadEventMap[MapIndex].unWindZone = 1;
                int randval = rand() % 100;
                if(randval<0) //9998
                {
                        ThreadEventMap[MapIndex].unWindZone = 2; //2 = donot record
			pthread_mutex_unlock(&init_mutex);
			if(size == 1600)
			printf("returning anyway %d\n",size);
                        return;
                }
                else
                {
                        int retval= 0;
                        long long (values[2])[2];

                        if(isPapiInitialized==1 && ThreadEventMap[MapIndex].isPapiRunning==1)
                        {
                                if ( (retval=PAPI_stop (thisEventSet, values[0])) != PAPI_OK)
                                {
                                        ERROR_RETURN(retval);
                                }
                                ThreadEventMap[MapIndex].isPapiRunning= -1;
                        }

                        ThreadEventMap[MapIndex].unWindZone = 4;
		 }
        }
        //printf("Withing malloc\n");
	pthread_mutex_unlock(&init_mutex);
#endif
}
#else

void
monitor_pre_malloc(size_t size)
{
#if 1
	if(MapIndex>=0)
	{
		if( 1==ThreadEventMap[MapIndex].unWindZone)
		{	
			if(size == 1600)
			printf("returning with size %d",size);		
			return;
		}
		
		ThreadEventMap[MapIndex].unWindZone = 1;
		int retval= 0;
		long long (values[2])[2];
			
		if(isPapiInitialized==1 && ThreadEventMap[MapIndex].isPapiRunning==1)
		{
			if ( (retval=PAPI_stop (thisEventSet, values[0])) != PAPI_OK)
	   		{
				ERROR_RETURN(retval);
	   		}
			//printf("*");
			ThreadEventMap[MapIndex].isPapiRunning= -1;
		}
		
		//printf("^");
		ThreadEventMap[MapIndex].unWindZone = 0;
	}
#endif
}
#endif
#ifdef RANDOM_ALLOC_MONITOR
void
monitor_post_malloc(size_t size, void *handle)
{
#if 1
        if(MapIndex>=0)
        {
		if(0 != pthread_mutex_trylock(&init_mutex))
                        return;
                if(1==ThreadEventMap[MapIndex].unWindZone || 2==ThreadEventMap[MapIndex].unWindZone || 3==ThreadEventMap[MapIndex].unWindZone) // || (randval<90))
                {
			pthread_mutex_unlock(&init_mutex);
                        return;
                }
        if(4==ThreadEventMap[MapIndex].unWindZone) //0 = asked to record
        {
		//check called for multiple times or not?
#if 1
		unsigned int retAdd = __builtin_return_address(1);
                int i =0 ;
                bool found = false;

                for(;i<current_allocAddreList_ptr;i++)
                {
                        if(allocAddreList[i] == retAdd)
                        {
                                found = true;
                                allocAddreList_counter[i]++;
                                break;
                        }
                }
                if(found)
                {
                        if(allocAddreList_counter[i]>ALLOCTHRESHOLD)
                        {
                                goto safeexit;
                        }
                }	
		else
                {
                        allocAddreList[i] = retAdd;
                        allocAddreList_counter[i]++;
                        current_allocAddreList_ptr++;
                }
	
#endif
		//check end
                ThreadEventMap[MapIndex].unWindZone = 1; // doing processing. donot reenter in pre or post allocation stage 
                recordMallocEvent(size , handle);
        }
	//else
	//	printf("ThreadEventMap[%d].unWindZone: %d\n",MapIndex,ThreadEventMap[MapIndex].unWindZone);
#if 1
safeexit:
                if(isPapiInitialized==1 && ThreadEventMap[MapIndex].isPapiRunning==-1)
                {
                        int retval= 0;
                        if ( (retval=PAPI_start (thisEventSet)) != PAPI_OK)
			{
				pthread_mutex_unlock(&init_mutex);
                                 ERROR_RETURN(retval);
			}
                        ThreadEventMap[MapIndex].isPapiRunning= 1;
                }
#endif
                ThreadEventMap[MapIndex].unWindZone = 3; //3= done recording
        }
	//printf(" malloc ending\n");
	pthread_mutex_unlock(&init_mutex);
#endif
}

#else

void
monitor_post_malloc(size_t size, void *handle)
{	
	if(MapIndex>=0)
	{
	//	pthread_mutex_lock(&init_mutex);
		//int randval = rand() % 100;
		if( 1==ThreadEventMap[MapIndex].unWindZone)// || (randval<90))
		{			
			//printf("Nested Call 2\n");
			//fflush(stdout);
	//		pthread_mutex_unlock(&init_mutex);
			return;
		}
	//	unsigned int retAdd = __builtin_return_address(0);
	//	printf(" 0x%x ",retAdd);
	//	if(retAdd!=0x400bb1)
	//		return;

#if 1
	int retval= 0;
//	   long long (values[2])[2];
	//if ( (retval=PAPI_stop (ThreadEventMap[MapIndex].EventSet, values[0])) != PAPI_OK)
   	{
	//	printf("Error Papi stop reval %d",retval);
	//	ERROR_RETURN(retval);
   	}
#endif

#if 1
		unsigned int retAdd = __builtin_return_address(1);
		int i =0 ;
		bool found = false;
		
		for(;i<current_allocAddreList_ptr;i++)
		{
			if(allocAddreList[i] == retAdd)
			{
				found = true;
				allocAddreList_counter[i]++;
				break;
			}
		}
		if(found)
		{
			//printf("*");
			if(allocAddreList_counter[i]>ALLOCTHRESHOLD)
			{
				//int randval = rand() % 1000;
				//if(randval<980)
					ThreadEventMap[MapIndex].unWindZone = 1;
					goto safeexit;
			}
		}
		else
		{
			//printf(" 0x%x ",retAdd);
			allocAddreList[i] = retAdd;
			allocAddreList_counter[i]++;
			current_allocAddreList_ptr++;
		}
#endif
		ThreadEventMap[MapIndex].unWindZone = 1;
#if 0
		
		if(isPapiInitialized==1 && ThreadEventMap[MapIndex].isPapiRunning==1)
		{
			if ( (retval=PAPI_stop (ThreadEventMap[MapIndex].EventSet, values[0])) != PAPI_OK)
	   		{
				printf("Error Papi stop reval %d",retval);
				ERROR_RETURN(retval);
	   		}
			ThreadEventMap[MapIndex].isPapiRunning= -1;
		}
		
#endif		
		recordMallocEvent(size , handle);
#if 1
safeexit:
		if(isPapiInitialized==1 && ThreadEventMap[MapIndex].isPapiRunning==-1)
		{
			if ( (retval=PAPI_start (thisEventSet)) != PAPI_OK)
     				 ERROR_RETURN(retval);
			ThreadEventMap[MapIndex].isPapiRunning= 1;
		}
#endif
		ThreadEventMap[MapIndex].unWindZone = 0;//replaceing with 3 instead of 0: ready to run
	}
}
#endif
//#endif
#else
void monitor_pre_malloc1(size_t size)
{

}
void monitor_post_malloc1(size_t size, void *handle)
{
}
#endif
void
monitor_pre_realloc(void *ptr, size_t size)
{
	monitor_pre_malloc1(size);
}
void
monitor_pre_calloc(size_t nmemb, size_t size)
{
	//printf("from calloc %d nmemb %d size %d\n",MapIndex, nmemb,size);
	monitor_pre_malloc1(nmemb * size);
}
void 
monitor_pre_memalign(size_t blocksize, size_t bytes)
{
	monitor_pre_malloc1(bytes);
}
void 
monitor_pre_posix_memalign(void** memptr, size_t alignment, size_t size)
{
	//printf("monitor_pre_posix_memalign(void** memptr, size_t alignment, size_t size) \n");
        monitor_pre_malloc1(size);
}
void monitor_post_posix_memalign(void** memptr, size_t alignment, size_t size, int ret)
{
	//printf("monitor_post_posix_memalign(void** memptr, size_t alignment, size_t size, int ret)\n");
	monitor_post_malloc1(size,*memptr);
}
void
monitor_post_realloc(void *ptr, size_t size, void *ptrTmp)
{
	//monitor_pre_free1(ptr);
	monitor_post_malloc1(size,ptrTmp);
}
void
monitor_post_calloc(size_t nmemb, size_t size, void *ptr)
{
	monitor_post_malloc1(nmemb*size,ptr);
}
void
monitor_post_memalign(size_t blocksize, size_t bytes, void *ptr)
{
	monitor_post_malloc1(bytes,ptr);
}
void
monitor_pre_free(void *handle)
{
	//print_trace ();
	//printf("within Free papi test- Probir------------>\n\n\n");
#if 0	
	struct MallocEvent *mallocEvent;
	struct MallocEventLinkedList* mallocEventPtr;
	int index, i;
	if(MapIndex>=0)
	{
	
		if(1==ThreadEventMap[MapIndex].unWindZone)// || (randval<90))
		{			
			return;
		}
		ThreadEventMap[MapIndex].unWindZone = 1;
		for(index = 0; index<=ThreadEventMapCurrentPtr;index++)
		{	
			for(i =0;i<ThreadEventMap[index].totalMallocEvents;i++ )
			{
				if(0==i)
				{
						mallocEventPtr = (struct MallocEventLinkedList *)ThreadEventMap[index].mallocEventLinkedList;
				}
				else
				{
					if(NULL!=mallocEventPtr->next)
					{
						mallocEventPtr = mallocEventPtr->next;					
					}
					else
					{
						printf("\ngoing out probir monitor_pre_free ************************> \n");
						break;
					}
				}
			
				if(mallocEventPtr!=NULL ){
					mallocEvent = mallocEventPtr->this;

					if(handle == mallocEvent->handle)// && mallocEvent->ThreadId!=overflowEvent->EventSet	)
					{
						
						clock_gettime(CLOCK_REALTIME, &mallocEvent->tmstop_alloc);
						ThreadEventMap[MapIndex].unWindZone = 0;
						return;
					}
				}
			}
		}
		ThreadEventMap[MapIndex].unWindZone = 0;
	}
	
	#endif
}

int
main()
{
	
	return 0;
}
