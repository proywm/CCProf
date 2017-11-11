#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>		/* This needs to be included every time you use PAPI */
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <list>
#include <map>
#include <stdarg.h>
#define MAXMEMACCESSINLOOP 20
#define TOTALTYPESOFEVENTS 25  
#define MAXFRAMESIZE 10
#define TOTALTHREAD 100
#define ARRAY_SIZE(x) (sizeof(x) / sizeof(*(x)))
/* type of opcode (load/store/prefetch,code) */
#define PERF_MEM_OP_NA          0x01 /* not available */
#define PERF_MEM_OP_LOAD        0x02 /* load instruction */
#define PERF_MEM_OP_STORE       0x04 /* store instruction */
#define PERF_MEM_OP_PFETCH      0x08 /* prefetch */
#define PERF_MEM_OP_EXEC        0x10 /* code (execution) */
/* memory hierarchy (memory level, hit or miss) */
#define PERF_MEM_LVL_NA         0x01  /* not available */
#define PERF_MEM_LVL_HIT        0x02  /* hit level */
#define PERF_MEM_LVL_MISS       0x04  /* miss level  */
#define PERF_MEM_LVL_L1         0x08  /* L1 */
#define PERF_MEM_LVL_LFB        0x10  /* Line Fill Buffer */
#define PERF_MEM_LVL_L2         0x20  /* L2 hit */
#define PERF_MEM_LVL_L3         0x40  /* L3 hit */
#define PERF_MEM_LVL_LOC_RAM    0x80  /* Local DRAM */
#define PERF_MEM_LVL_REM_RAM1   0x100 /* Remote DRAM (1 hop) */
#define PERF_MEM_LVL_REM_RAM2   0x200 /* Remote DRAM (2 hops) */
#define PERF_MEM_LVL_REM_CCE1   0x400 /* Remote Cache (1 hop) */
#define PERF_MEM_LVL_REM_CCE2   0x800 /* Remote Cache (2 hops) */
#define PERF_MEM_LVL_IO         0x1000 /* I/O memory */
#define PERF_MEM_LVL_UNC        0x2000 /* Uncached memory */
/* snoop mode */
#define PERF_MEM_SNOOP_NA       0x01 /* not available */
#define PERF_MEM_SNOOP_NONE     0x02 /* no snoop */
#define PERF_MEM_SNOOP_HIT      0x04 /* snoop hit */
#define PERF_MEM_SNOOP_MISS     0x08 /* snoop miss */
#define PERF_MEM_SNOOP_HITM     0x10 /* snoop hit modified */
/* locked instruction */
#define PERF_MEM_LOCK_NA        0x01 /* not available */
#define PERF_MEM_LOCK_LOCKED    0x02 /* locked transaction */
/* TLB access */
#define PERF_MEM_TLB_NA         0x01 /* not available */
#define PERF_MEM_TLB_HIT        0x02 /* hit level */
#define PERF_MEM_TLB_MISS       0x04 /* miss level */
#define PERF_MEM_TLB_L1         0x08 /* L1 */
#define PERF_MEM_TLB_L2         0x10 /* L2 */
#define PERF_MEM_TLB_WK         0x20 /* Hardware Walker*/
#define PERF_MEM_TLB_OS         0x40 /* OS fault handler */

int datafile = 1;
std::map<unsigned long, int> threadIndexMap;

typedef enum {
   MALLOC = 0,
   FREE = 1,
   PEBSOVERFLOW = 2,
   LOOP = 3
}EVENTTYPE;
union perf_mem_data_src {
         unsigned long long val;
         struct {
                 unsigned long long   mem_op:5,       /* type of opcode */
                         mem_lvl:14,     /* memory hierarchy level */
                         mem_snoop:5,    /* snoop mode */
                         mem_lock:2,     /* lock instr */
                         mem_dtlb:7,     /* tlb access */
                         mem_rsvd:31;
         };
 };
enum { OP, LVL, SNP, LCK, TLB };
static const struct {
		unsigned long long   bit;
		int64_t    field;
		const char *name;
	} decode_bits[] = {
	{ PERF_MEM_OP_LOAD,       OP,  "LOAD"     },
	{ PERF_MEM_OP_STORE,      OP,  "STORE"    },
	{ PERF_MEM_OP_NA,         OP,  "OP_NA"    },
	{ PERF_MEM_OP_PFETCH,     OP,  "PFETCH"    },
	{ PERF_MEM_OP_EXEC,       OP,  "EXEC"    },
	{ PERF_MEM_LVL_LFB,       LVL, "LFB"      },
	{ PERF_MEM_LVL_L1,        LVL, "L1"       },
	{ PERF_MEM_LVL_L2,        LVL, "L2"       },
	{ PERF_MEM_LVL_L3,        LVL, "LCL_LLC"  },
	{ PERF_MEM_LVL_LOC_RAM,   LVL, "LCL_RAM"  },
	{ PERF_MEM_LVL_REM_RAM1,  LVL, "RMT_RAM"  },
	{ PERF_MEM_LVL_REM_RAM2,  LVL, "RMT_RAM"  },
	{ PERF_MEM_LVL_REM_CCE1,  LVL, "RMT_LLC"  },
	{ PERF_MEM_LVL_REM_CCE2,  LVL, "RMT_LLC"  },
	{ PERF_MEM_LVL_IO,        LVL, "I/O"	  },
	{ PERF_MEM_LVL_UNC,       LVL, "UNCACHED" },
	{ PERF_MEM_LVL_NA,        LVL, "LVL_N"        },
	{ PERF_MEM_LVL_HIT,       LVL, "HIT"      },
	{ PERF_MEM_LVL_MISS,      LVL, "MISS"     },
	{ PERF_MEM_SNOOP_NONE,    SNP, "SNP NONE" },
	{ PERF_MEM_SNOOP_HIT,     SNP, "SNP HIT"  },
	{ PERF_MEM_SNOOP_MISS,    SNP, "SNP MISS" },
	{ PERF_MEM_SNOOP_HITM,    SNP, "SNP HITM" },
	{ PERF_MEM_SNOOP_NA,      SNP, "SNP NA"   },
	{ PERF_MEM_LOCK_LOCKED,   LCK, "LOCKED"   },
	{ PERF_MEM_LOCK_NA,       LCK, "LOCK_NA"  },
	{ PERF_MEM_TLB_NA,        TLB, "TLB_N"    },
	{ PERF_MEM_TLB_HIT,       TLB, "TLB_HIT"  },
	{ PERF_MEM_TLB_MISS,      TLB, "TLB_MISS" },
	{ PERF_MEM_TLB_L1,        TLB, "TLB_L1"   },
	{ PERF_MEM_TLB_L2,        TLB, "TLB_L2"   },
	{ PERF_MEM_TLB_WK,        TLB, "TLB_WK"   },
	{ PERF_MEM_TLB_OS,        TLB, "TLB_OS"   },
};

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
  //struct timespec tmevent;
  unsigned long tmevent;
};
struct loopStruct
{
	int loopId;
	int startLineLoop;
	int endLineLoop;
	long int MinAddRange;
	long int MaxAddRange;
};
struct loopStructLinkedList
{
	struct loopStruct *loopitself;
	struct loopStructLinkedList *nextLoop;
	struct loopStructLinkedList *prevLoop;
};
struct OverFlowEventLinkedList
{
  struct OverFlowEventData *this_Elem;
  struct loopStruct *loop;
  struct OverFlowEventLinkedList *prev;
  struct OverFlowEventLinkedList *next;
};//*overFlowEventLinkedList;
struct MallocEventLinkedList
{
  struct MallocEventData *this_Elem;
  struct MallocEventLinkedList *prev;
  struct MallocEventLinkedList *next;
};

struct MallocEventPEBSEventMap
{
  struct MallocEventData *this_Elem;
  struct OverFlowEventLinkedList *startPtr;
  int totalPEBSEvents;
  struct MallocEventPEBSEventMap *prev;
  struct MallocEventPEBSEventMap *next;
  unsigned int *sorted_data_src;
  unsigned int *Inter_data_src_distance;
  double *Normalized_data_src;
};

//*mallocEventLinkedList;
//int mallocEventCounter = 0;
//int OverFlowEventCounter = 0;
int ThreadEventMapCurrentPtr = -1;

typedef struct
{
  pid_t threadid;
  int PAPI_event; /* a place holder for an event preset */
  int EventSet;

  int total; /* we use total to track the amount of overflows that occured */

  struct MallocEventLinkedList *mallocEventLinkedList;
  int mallocEventCounter;

  struct OverFlowEventLinkedList *overFlowEventLinkedList;
  int OverFlowEventCounter;

  struct MallocEventPEBSEventMap *mallocPEBSMap;
  int mallocPEBSMapCounter;

} ThreadEvent;
ThreadEvent ThreadEventMap[TOTALTHREAD];
bool doesThreadIdExist[TOTALTHREAD];
int *mallocAccessed[TOTALTHREAD];
struct loopStructLinkedList *loopLinkedListHead;

void printDataSRC(unsigned long long val)
{
	union perf_mem_data_src dsrc;
        dsrc.val = val;
	int i;
	printf("\t");
	for (i = 0; i < ARRAY_SIZE(decode_bits); i++) {
		int bitval;

		switch (decode_bits[i].field) {
		case OP:  bitval = decode_bits[i].bit & dsrc.mem_op;    break;
		case LVL: bitval = decode_bits[i].bit & dsrc.mem_lvl;   break;
		case SNP: bitval = decode_bits[i].bit & dsrc.mem_snoop; break;
		case LCK: bitval = decode_bits[i].bit & dsrc.mem_lock;  break;
		case TLB: bitval = decode_bits[i].bit & dsrc.mem_dtlb;  break;
		default: bitval = 0;					break;
		}

		if (!bitval)
			continue;
		printf("%s, ",decode_bits[i].name);
		//first_present = false;
	}
	//printf("\n");
}

void FindLoopWhereItBelongs(struct OverFlowEventLinkedList *overflowEvent)
{
	struct loopStructLinkedList *temp = loopLinkedListHead;
	int range = -1;
	while(NULL != temp)
	{
		struct OverFlowEventData* a = ((struct OverFlowEventData*)overflowEvent->this_Elem);
		int b = ((struct loopStruct *)temp->loopitself)->MinAddRange;
		int c = ((struct loopStruct *)temp->loopitself)->MaxAddRange;
		if(b <= ((struct OverFlowEventData*)overflowEvent->this_Elem)->address && c >= ((struct OverFlowEventData*)overflowEvent->this_Elem)->address)
		{
			if((-1==range)||(range>(c-b)))
			{
				range = c-b;
				overflowEvent->loop = temp->loopitself;
			}
			//return;
		}
		temp = temp->nextLoop;
	}
}
struct MallocEvent *findMemoryAllocForMemLocation(struct OverFlowEventData *overflowEvent)
{
	return NULL;
}
void findPEBSInsideMallocRegion()
{
	struct MallocEventData *mallocEvent;
	struct OverFlowEventData *overflowEvent;
	struct MallocEventLinkedList* mallocEventPtr;
	struct OverFlowEventLinkedList* overFlowEventptr;
	
	int index, i;
	#pragma omp for
	for(index = 0; index<=ThreadEventMapCurrentPtr;index++)
	{
		mallocAccessed[index] = (int *)malloc(sizeof(int)*ThreadEventMap[index].mallocEventCounter);
		int k=0;
		for(k=0;k<ThreadEventMap[index].mallocEventCounter;k++)
			mallocAccessed[index][k]= 0;
	}
	
#if 1
	for(index = 0; index<=ThreadEventMapCurrentPtr;index++)
	{	
		for(i =0;i<ThreadEventMap[index].OverFlowEventCounter;i++ )
		{
			if(0==i)
			{
				overFlowEventptr = (struct OverFlowEventLinkedList *)ThreadEventMap[index].overFlowEventLinkedList;
			}
			else
			{
				if(NULL!=overFlowEventptr->next)
				{
					overFlowEventptr = overFlowEventptr->next;					
				}
				else
				{
					printf("\ngoing out probir************************> \n");
					return;
				}
			}
			overflowEvent = overFlowEventptr->this_Elem;
			if(NULL!=overflowEvent)
			{
				findMemoryAllocForMemLocation(overflowEvent);//overflowEvent->data_addr
				fflush(stdout); 
			}
			
		}
	}
#endif
}
int threadIndex_c = 0;
/* Obtain a backtrace and print it to stdout. */
void
restoreData (FILE* input, EVENTTYPE eventType)
{
	int threadIndex = 0;
	switch(eventType)
	{
		case MALLOC:
		{
			struct MallocEventData tempData;
		  	while(0!=fread(&tempData, sizeof(tempData), 1, input))
			{
				struct MallocEventLinkedList *tempMallocEventLinkedList;
				struct MallocEventData *data = (struct MallocEventData *)malloc(sizeof(struct MallocEventData));
				tempMallocEventLinkedList = (struct MallocEventLinkedList *)malloc(sizeof(struct MallocEventLinkedList));
				memcpy(data,&tempData,sizeof(tempData));//why did I do this_Elem copying?
				std::map<unsigned long,int>::iterator it_c;
                                it_c = threadIndexMap.find(data->ThreadId);
                                if(it_c != threadIndexMap.end())
                                {
                                       threadIndex = threadIndexMap.find(data->ThreadId)->second;
                                }
                                else
                                {
                                        threadIndex = threadIndex_c;
                                        threadIndexMap.insert( std::pair<unsigned long, int>(data->ThreadId, threadIndex));
                                        threadIndex_c++;
					ThreadEventMapCurrentPtr++;
                                }
				tempMallocEventLinkedList->this_Elem = data;
				if(0==ThreadEventMap[threadIndex].mallocEventCounter)
				{
					tempMallocEventLinkedList->prev = NULL;
					tempMallocEventLinkedList->next = NULL;
					ThreadEventMap[threadIndex].mallocEventLinkedList = tempMallocEventLinkedList;
				}
				else
				{
					((struct MallocEventLinkedList *)ThreadEventMap[threadIndex].mallocEventLinkedList)->prev = tempMallocEventLinkedList;
					tempMallocEventLinkedList->next = ((struct MallocEventLinkedList *)ThreadEventMap[threadIndex].mallocEventLinkedList);
					tempMallocEventLinkedList->prev = NULL;
					ThreadEventMap[threadIndex].mallocEventLinkedList = tempMallocEventLinkedList;	
				}
				ThreadEventMap[threadIndex].mallocEventCounter++;	
				if( data->eventtype == FREE)
					printf("33\t444\t%d\t%d\t%lu\t%d\t%lu\t%d\n",threadIndexMap.find(data->ThreadId)->second, data->size,data->handle,data->frameSize,data->MallocCodeAddress,(data->tmstart_alloc).tv_sec);
				else
					 printf("33\t111\t%d\t%d\t%lu\t%d\t%lu\t%d\n", threadIndexMap.find(data->ThreadId)->second, data->size,data->handle,data->frameSize,data->MallocCodeAddress,(data->tmstart_alloc).tv_sec);
			}
			if(0!=feof(input))
			{
					return;
			}
		}
		break;
		case PEBSOVERFLOW:
		{
			struct OverFlowEventData tempData;
		  	while(0!=fread(&tempData, sizeof(tempData), 1, input))
			{
				struct OverFlowEventLinkedList *tempOverFlowEventLinkedList;
				struct OverFlowEventData *data = (struct OverFlowEventData *)malloc(sizeof(struct OverFlowEventData));
				tempOverFlowEventLinkedList = (struct OverFlowEventLinkedList *)malloc(sizeof(struct OverFlowEventLinkedList));
				memcpy(data,&tempData,sizeof(tempData));
				threadIndex = data->EventSet;

				std::map<unsigned long,int>::iterator it_c;
                                it_c = threadIndexMap.find(data->EventSet);
                                if(it_c != threadIndexMap.end())
                                {
                                       threadIndex = threadIndexMap.find(data->EventSet)->second;
                                }
                                else
                                {
                                        threadIndex = threadIndex_c;
                                        threadIndexMap.insert( std::pair<unsigned long, int>(data->EventSet, threadIndex));
                                        threadIndex_c++;
                                }

				tempOverFlowEventLinkedList->this_Elem = data;
				if(0==ThreadEventMap[threadIndex].OverFlowEventCounter)
				{
					tempOverFlowEventLinkedList->prev = NULL;
					tempOverFlowEventLinkedList->next = NULL;
					ThreadEventMap[threadIndex].overFlowEventLinkedList = tempOverFlowEventLinkedList;
				}
				else
				{
					((struct OverFlowEventLinkedList *)ThreadEventMap[threadIndex].overFlowEventLinkedList)->prev = tempOverFlowEventLinkedList;
					tempOverFlowEventLinkedList->next = ((struct OverFlowEventLinkedList *)ThreadEventMap[threadIndex].overFlowEventLinkedList);
					tempOverFlowEventLinkedList->prev = NULL;
					ThreadEventMap[threadIndex].overFlowEventLinkedList = tempOverFlowEventLinkedList;	
				}
				ThreadEventMap[threadIndex].OverFlowEventCounter++;
				printf("32\t%d\t%lu\t%lu\t%d\t%lu\t%d\t%lu\t%lu\t%lu\n",threadIndexMap.find(data->EventSet)->second, data->address,data->data_addr,data->weight,data->data_src,data->cpu,data->phys, data->cacheSet,data->tmevent);
				//printf("\n\nindex:%d EventType %d, EventSet %d, address %p, data_addr %p weight %d data_src %p cpu %d\n",data->EventSet, data->eventtype,data->EventSet,data->address,data->data_addr,data->weight,data->data_src,data->cpu);
			}
			if(0!=feof(input))
			{
				return;
			}
		}
		break;
		case LOOP:
		{
			printf("%d loop loop : \n");
			struct loopStruct tempData;
		  	while(0!=fread(&tempData, sizeof(tempData), 1, input))
			{
				struct loopStruct *data = (struct loopStruct *)malloc(sizeof(struct loopStruct));
				struct loopStructLinkedList *dataList = (struct loopStructLinkedList *)malloc(sizeof(struct loopStructLinkedList));
				memcpy(data,&tempData,sizeof(tempData));
				dataList->loopitself = data;				
				//printf("%d loop Start %x  end %x   loop : %d,%d\n",data->loopId,data->MinAddRange,data->MaxAddRange, data->startLineLoop, data->endLineLoop);
				if(NULL == loopLinkedListHead)//0==ThreadEventMap[threadIndex].OverFlowEventCounter)
				{
					dataList->prevLoop = NULL;
					dataList->nextLoop = NULL;
					loopLinkedListHead = dataList;
				}
				else
				{
					((struct loopStructLinkedList *)loopLinkedListHead)->prevLoop = dataList;
					dataList->nextLoop = ((struct loopStructLinkedList*)loopLinkedListHead);
					dataList->prevLoop = NULL;
					loopLinkedListHead = dataList;	
				}
			}
			if(0!=feof(input))
			{
				return;
			}
		}
		break;
	}
}
void initThreadEventMap()
{
	int threadIndex=0;
#pragma omp for
	for(threadIndex=0;threadIndex<TOTALTHREAD;threadIndex++)
	{
		ThreadEventMap[threadIndex].mallocEventCounter = 0;
		ThreadEventMap[threadIndex].OverFlowEventCounter = 0;
		ThreadEventMap[threadIndex].mallocPEBSMapCounter = 0;

		doesThreadIdExist[threadIndex] = false;
	}
}
void calculateDistanceFromBase(unsigned int base, unsigned int *list, unsigned int * Inter_data_src_distance_list, int count)
{
	int m=0;
	while(count-m>0)
	{
		Inter_data_src_distance_list[m]=list[m]-base;
		m++;
	}
	//printf("\ncount %d \n",count);
//	m=0;
//		while(count-m>1)
//		{
			//printf(" %u",Inter_data_src_distance_list[m]);
//			m++;
//		}
	//printf("\n",count);
}

void calculateDistance(unsigned int *list, unsigned int * Inter_data_src_distance_list, int count)
{
	int m=0;
	while(count-m>0)
	{
		if(list[m]>list[m+1])
			Inter_data_src_distance_list[m]=list[m]-list[m+1];
		else
			Inter_data_src_distance_list[m]=list[m+1] - list[m];
		m++;
	}
	//printf("\ncount %d \n",count);
//	m=0;
//		while(count-m>1)
//		{
			//printf(" %u",Inter_data_src_distance_list[m]);
//			m++;
//		}
	//printf("\n",count);
}
void sort(unsigned int *list, int count)
{
	float length , index;
	length = count;
	int m = 0;	
	//printf("\n before sorting :\n");
	//m=0;
	//	while(count-m>0)
	//		printf(" %u",list[m++]);
	//printf("\n ");
	//m=0;	
	while(length>1)
	{
		bool DoneSorting = false;
		
		while(!DoneSorting) //max Heapyfy
		{
			index = length;
			DoneSorting = true;
			while(index>1)
			{
				float nextIndex = index - 1;
				if(0!=((int)index)%2)
				{
					if(list[(int)(2*floor(index/2))-1]>list[(int)(2*floor(index/2)+1)-1])
					{
						unsigned int temp = list[(int)(2*floor(index/2))-1];
						list[(int)(2*floor(index/2))-1] = list[(int)(2*floor(index/2)+1)-1];
						list[(int)(2*floor(index/2)+1)-1] = temp;
					}
					nextIndex = index - 2;
				}
				if(list[(int)(2*floor(index/2)-1)]<list[(int)(floor(index/2))-1])
				{
					//printf("came here %d < %d \n",list[(int)(2*floor(index/2)-1)],list[(int)(floor(index/2))-1]);
					unsigned int temp = list[(int)(2*floor(index/2))-1];
					list[(int)(2*floor(index/2))-1] = list[(int)(floor(index/2))-1];
					list[(int)(floor(index/2))-1] = temp;
					DoneSorting = false;
					//printf("came here \n");
				}
				index = nextIndex;
			}
		}
		unsigned int temp1 = list[1-1];
		list[1-1] = list[(int)length-1];
		list[(int)length-1] = temp1;
		length=length-1;
	}
	//	printf("\n sorted :\n");
	//m=0;
		//while(count-m>0)
		//	printf(" %u",list[m++]);
	//printf("\n ");
	return;
}
int 
gcd ( unsigned int a, unsigned int b )
{
  unsigned int c;
  while ( a != 0 ) {
     c = a; a = b%a;  b = c;
  }
  return b;
}
unsigned int calculateGCD(unsigned int * Inter_data_src_distance_list, int count)
{
	int m=0;
	unsigned int a = Inter_data_src_distance_list[m];
	if(count>1)
	{
		
		unsigned int b = Inter_data_src_distance_list[m+1];
		do
		{
			b = Inter_data_src_distance_list[m+1];
			a = gcd(a,b);
			m++;
		}while(count-m>1);
	}
	return a;
}
void calculateNormal(unsigned int *list, int count,unsigned int start,unsigned int end, double *listNormal)
{
	int m = 0;
	printf("\n Normalized\n");
	while(count-m>0)
	{
		listNormal[m] = (double)(list[m]-start)/(double)(end-start);
		printf("%f ",listNormal[m]);
		m++;	
	}
	printf("\n");
}
int calulatePosition(unsigned int handle,unsigned int lowestAddress, int gcdValue)
{
	unsigned int sub = lowestAddress - handle;
	if(gcdValue == 0)
		return -1;
	else
		return (int)(((int)sub) % gcdValue);
}
#define MAXMALLOC 10000
unsigned int visitedList[MAXMALLOC];
int nextEntry = 0;

struct mallocRecordsInLoop
{
	unsigned int* listOfAccessPerAllocAddr;
	unsigned int* distListOfAccessPerAllocAddr;
	int accessCount;	
	int count;
	unsigned int handle;
	unsigned int lowestAddr;
	unsigned long long weightInLoop;
	unsigned int address;
	bool visited;
	struct mallocRecordsInLoop* prev;
};

void findPEBSInsideLoopRegion()
{
	struct OverFlowEventData *overflowEvent;
	struct OverFlowEventLinkedList* overFlowEventptr;
	
	int index, i;
	
#if 1
	for(index = 0; index<=ThreadEventMapCurrentPtr;index++)
	{	
		for(i =0;i<ThreadEventMap[index].OverFlowEventCounter;i++ )
		{
			if(0==i)
			{
				overFlowEventptr = (struct OverFlowEventLinkedList *)ThreadEventMap[index].overFlowEventLinkedList;
			}
			else
			{
				if(NULL!=overFlowEventptr->next)
				{
					overFlowEventptr = overFlowEventptr->next;					
				}
				else
				{
					printf("\ngoing out probir************************> \n");
					return;
				}
			}
			overflowEvent = overFlowEventptr->this_Elem;
			if(NULL!=overflowEvent)
			{
				FindLoopWhereItBelongs(overFlowEventptr);
				fflush(stdout); 
			}
			
		}
	}
#endif
}
void printLoopPEBSMAP()
{
	int index=0;
	struct loopStructLinkedList *tempLoopList = loopLinkedListHead;
	while(tempLoopList)
	{
		printf("\n\n Loop:	Line %d-%d: \n",tempLoopList->loopitself->startLineLoop,tempLoopList->loopitself->endLineLoop);
		for(index = 0; index<=ThreadEventMapCurrentPtr;index++)
		{
			struct OverFlowEventLinkedList *tempPEBS = ThreadEventMap[index].overFlowEventLinkedList;
			if(0==index)
				printf("\t Process :\n");
			else
				printf("\t Thread Index %d:\n",index);
			
			unsigned int* tempEventsList = (unsigned int*)malloc(sizeof(unsigned int) * ThreadEventMap[index].OverFlowEventCounter);
			unsigned int* tempEventsListPerCache = NULL;//malloc(sizeof(unsigned int) * ThreadEventMap[index].OverFlowEventCounter);
			int count = 0;
			int perCachePEBSCount = 0;
			bool hasEvent = false;
			unsigned long data_src = 0;
			while(NULL!=tempPEBS)
			{
				if(tempPEBS->loop)
				{
					//printf("Loop Ids %d --- %d\n",tempLoopList->loopitself->loopId,tempPEBS->loop->loopId);
					if(tempLoopList->loopitself->loopId == tempPEBS->loop->loopId)
					{
						if(!hasEvent)
						{
							printf("\n\n\t\t\t PEBS: ");
						}
						if(data_src != tempPEBS->this_Elem->data_src)
						{
							if(perCachePEBSCount!=0)
							{
								printf("\n\n");
								unsigned int* tempdistanceList = (unsigned int*) malloc(sizeof(unsigned int) * (perCachePEBSCount));
								calculateDistance(tempEventsListPerCache, tempdistanceList, perCachePEBSCount);
								printf("\nGCD With in cache: %d \n",calculateGCD(tempdistanceList, perCachePEBSCount-1));
								perCachePEBSCount = 0;
							}
							tempEventsListPerCache = (unsigned int*)malloc(sizeof(unsigned int) * ThreadEventMap[index].OverFlowEventCounter);
							data_src = tempPEBS->this_Elem->data_src;
							printf("\n\t\t\t data_src: %p\n",data_src);
							printDataSRC(data_src);
						}
						else
						{
							tempEventsListPerCache[perCachePEBSCount] = tempPEBS->this_Elem->data_addr;
							perCachePEBSCount++;
						}
						printf("0x%x : 0x%x, ",tempPEBS->this_Elem->address, tempPEBS->this_Elem->data_addr);
						tempEventsList[count] = tempPEBS->this_Elem->data_addr;
						hasEvent = true;
						count++;
					}
				}
				
				tempPEBS = tempPEBS->next;				
			}
			//GCD
			if(hasEvent)
			{
				printf("\n\n");
				unsigned int* tempdistanceList = (unsigned int*)malloc(sizeof(unsigned int) * (count));
				calculateDistance(tempEventsList, tempdistanceList, count);
				printf("\nGCD: %d \n",calculateGCD(tempdistanceList, count-1));
			}
		}
		tempLoopList = tempLoopList->nextLoop;
	}
}
void ListMallocPositions()
{
	struct MallocEventLinkedList *mallocEventPtr;
	int k = 0;
	int i = 0;
	for(k = 0; k<=ThreadEventMapCurrentPtr;k++)
	{
		mallocEventPtr = (struct MallocEventLinkedList *)ThreadEventMap[k].mallocEventLinkedList;
		i=0;
		bool found = false;
		
		do
		{
			//printf("mallocEventPtr 0x%x\n",mallocEventPtr->this_Elem->MallocCodeAddress);
			found = false;
			for(i=0;i<nextEntry;i++)
			{
					//printf("Malloc Position : 0x%x, number 0x%x i%d\n",visitedList[i],number,i);
				if(visitedList[i]==mallocEventPtr->this_Elem->MallocCodeAddress)
				{
					found = true;
					
					break;
				}
			}
			if(!found)
			{
				visitedList[i] = mallocEventPtr->this_Elem->MallocCodeAddress;
				nextEntry++;
			}
			mallocEventPtr = mallocEventPtr->next;
		}while(mallocEventPtr!=NULL);
	}
	i=0;
	while(i<nextEntry)
	{	
		printf("i:%d visitedList[i]: 0x%x\n",i,visitedList[i]);
		i++;
	}
	
}
void CountTotalEvents()
{
	struct OverFlowEventData *overflowEvent;
	struct OverFlowEventLinkedList* overFlowEventptr;
	unsigned long tempEventTypesIndex[TOTALTYPESOFEVENTS];
	int tempEventTypesCount[TOTALTYPESOFEVENTS];
	int eventIndex = 0;
	int index, i;
	int total = 0;
#if 1
	for(index = 0; index<=ThreadEventMapCurrentPtr;index++)
	{	
		overFlowEventptr = (struct OverFlowEventLinkedList *)ThreadEventMap[index].overFlowEventLinkedList;
		while(NULL!=overFlowEventptr){
			bool found = false;
			overflowEvent = overFlowEventptr->this_Elem;
			for( i =0;i<eventIndex;i++)
			{
				//printf("Count : %d \n",i);
				if(tempEventTypesIndex[i]==overflowEvent->data_src)
				{
					found = true;
					tempEventTypesCount[i]++;
				}
			}
			if(found==false)
			{
				tempEventTypesIndex[i] = overflowEvent->data_src;
				tempEventTypesCount[i] = 1;
				eventIndex++;
			}
			total++;
			overFlowEventptr = overFlowEventptr->next;	
		}
	}
	printf("\n------------------------- Counting ---------------------- \n");
	
	for(i =0;i<eventIndex;i++)
	{
		float ratio = (float )((float)tempEventTypesCount[i]/total);
		printDataSRC(tempEventTypesIndex[i]);
		printf("Count : %d ratio: %f and data_src %d \n",tempEventTypesCount[i], ratio, tempEventTypesIndex[i]);
	}
#endif
}
/* A dummy function to make the backtrace more interesting. */
void
readFromFile (char *DataMalloc,char *DataPEBS)
{
  //ThreadEventMapCurrentPtr = 0;
  int i =0;
  initThreadEventMap();
  FILE* input;
  if(datafile == 1)
  {
    input = fopen(DataMalloc, "rb");
    restoreData(input,MALLOC);
    fclose(input);
  }
  else
  {
    input = fopen(DataPEBS, "rb");
    restoreData(input,PEBSOVERFLOW);
    fclose(input);
  }
}

int
main (int argc, char* argv[])
{
	if (argc == 4) { // We expect 3 arguments: the program name, the malloc data file and the overflow data path
        datafile = atoi(argv[3]);
        readFromFile(argv[1],argv[2]);
    }
  return 0;
}
