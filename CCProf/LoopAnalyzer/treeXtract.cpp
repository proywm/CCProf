/**
 * section: Tree
 * synopsis: Navigates a tree to print element names
 * purpose: Parse a file to a tree, use xmlDocGetRootElement() to
 *          get the root element, then walk the document and print
 *          all the element name in document order.
 * usage: tree1 filename_or_URL
 * test: tree1 test2.xml > tree1.tmp && diff tree1.tmp $(srcdir)/tree1.res
 * author: Dodji Seketeli
 * copy: see Copyright for the status of this software.
 */
#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <string.h>

#ifdef LIBXML_TREE_ENABLED

/*
 *To compile this file using gcc you can type
 *gcc `xml2-config --cflags --libs` -o xmlexample libxml2-example.c
 */

static int depth = 0;
static int globalIndex = 0;
#if 1
static void parseAddressString(char *addressString, int currDepth, char *type,  char* filename, char* lineNumber)
{
	if(!(!strcmp("L",type) || !strcmp("S",type)))
		return;
	int iterator = 0;
	char* a = strtok(addressString,"{[) -");
	while(NULL!=a){
                long int la = strtol(a, NULL, 16);
#if 1
                if(iterator%2==0)
                {
			if(!strcmp("L",type))
				printf("\nID %d Level %d 1 File:%s %s",globalIndex++,currDepth, filename, lineNumber);
			else
				printf("\nID %d Level %d 0 File:%s %s",globalIndex++,currDepth, filename, lineNumber);
		}
#endif
                printf(" %llu ",la);
		a = strtok(NULL,"{[)} -");

                iterator++;
         }
}
#endif
/**
 * print_element_names:
 * @a_node: the initial xml node to consider.
 *
 * Prints the names of the all the xml elements
 * that are siblings or children of a given xml node.
 */
static void
print_element_names(xmlNode * a_node, int currDepth)
{
    xmlNode *cur_node = NULL;
//    depth++;
    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
//            printf("node type: Element, name: %s level %d\n", cur_node->name,currDepth);
	    char * addressString = (char *)xmlGetProp(cur_node,(const xmlChar*)"v");
	    char * filename = (char *)xmlGetProp(cur_node,(const xmlChar*)"f");
	    char * lineNumberchar = (char *)xmlGetProp(cur_node,(const xmlChar*)"l");
	//    if(lineNumberchar)
	//	strtol(lineNumberchar, NULL, 16);
		
	    parseAddressString(addressString,currDepth,(char*)cur_node->name, filename, lineNumberchar);
        }
	//printf("current Node %s Entering to child level %d\n",cur_node->name,currDepth);
        print_element_names(cur_node->children,currDepth+1);
	//printf("current Node %s Exiting from child level %d\n",cur_node->name,currDepth);
    }
}


/**
 * Simple example to parse a file called "file.xml", 
 * walk down the DOM, and print the name of the 
 * xml elements nodes.
 */
int
main(int argc, char **argv)
{
    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;

    if (argc != 2)
        return(1);

    /*
     * this initialize the library and check potential ABI mismatches
     * between the version it was compiled for and the actual shared
     * library used.
     */
    LIBXML_TEST_VERSION

    /*parse the file and get the DOM */
    doc = xmlReadFile(argv[1], NULL, 0);

    if (doc == NULL) {
        printf("error: could not parse file %s\n", argv[1]);
    }

    /*Get the root element node */
    root_element = xmlDocGetRootElement(doc);

    print_element_names(root_element,0);

    /*free the document */
    xmlFreeDoc(doc);

    /*
     *Free the global variables that may
     *have been allocated by the parser.
     */
    xmlCleanupParser();
    printf("\n ");

    return 0;
}
#else
int main(void) {
    fprintf(stderr, "Tree support not compiled in\n");
    exit(1);
}
#endif
