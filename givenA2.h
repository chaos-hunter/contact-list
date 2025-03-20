#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_LENGTH 100

struct contact {
   long empIdPosn;
   long firstNamePosn;
   long lastNamePosn;
   long emailPosn;
   long next;
};

// Helper to dynamically allocate and read a string based on a given prompt
char *readDynamicString(const char *prompt);
void createContacts (char * fileName);

void readContacts (char * fileName);
// Helper function to read a string from the file
char *readStringFromFile(FILE *fp, long position);
int searchContacts (char * fileName, int keyId);
