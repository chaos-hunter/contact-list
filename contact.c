#include "givenA2.h"


int main(int argc, char *argv[]) {
    char *fileName = "contactListA2.db";
    char choice[10];
    int option;

    // Initial interface for adding new contacts
    do {
        printf("Do you wish to enter a new contact (Yes or No)? ");
        scanf("%s", choice);
        if (strcmp(choice, "Yes") == 0) {
            createContacts(fileName);
        }
    } while (strcmp(choice, "Yes") == 0);

    // Main menu for interacting with the contact list
    do {
        printf("Enter a choice:\n");
        printf("1 to print, 2 to search, 3 to create more, -1 to exit: ");
        scanf("%d", &option);

        switch (option) {
            case 1:
                readContacts(fileName);
                break;
            case 2: {
                int keyId;
                printf("Search for Employee Id? ");
                scanf("%d", &keyId);
                if (!searchContacts(fileName, keyId)) {
                    printf("No match found.\n");
                }
                break;
            }
            case 3:
                createContacts(fileName);
                break;
            case -1:
                printf("Exiting\n");
                return 0;
            default:
                printf("Invalid choice. Please enter 1, 2, 3, or -1.\n");
        }
    } while (option != -1);

    return 0;
}

// Helper to dynamically allocate and read a string based on a given prompt
char *readDynamicString(const char *prompt) {
    char buffer[MAX_LENGTH];
    printf("%s", prompt);
    if (fgets(buffer, MAX_LENGTH, stdin)) {
        // Remove newline character
        buffer[strcspn(buffer, "\n")] = 0;
        char *str = malloc(strlen(buffer) + 1);
        if (str) {
            strcpy(str, buffer);
            return str;
        }
    }
    return NULL;
}

void createContacts(char *fileName) {
    FILE *fp = fopen(fileName, "a+b"); // Open in append plus
    if (!fp) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    // Seek to the end of the file to get the position for the new record
    fseek(fp, 0, SEEK_END);
    long recordStartPos = ftell(fp);
    // Initialize a contact structure set to 5
    struct contact newContact = {0, -1, -1, -1, -1};

    // Initialize and validate the employee ID
    long empId = 0;
    do {
        printf("Employee Id: ");
        scanf("%ld", &empId);
        // Clear the input buffer
        while (getchar() != '\n');
        //check if ID is between 1 to 9999 and ask again if not
        if (empId < 1 || empId > 9999) {
            printf("Invalid Employee ID. Please enter a number between 1 and 9999.\n");
        }
    } while (empId < 1 || empId > 9999);

    // Correctly assign the position values for each field
    newContact.empIdPosn = recordStartPos + sizeof(struct contact);
    long currentPosition = newContact.empIdPosn + sizeof(long);

    // Dynamically allocate and read strings for First Name, Last Name, and Email
    char *firstName = readDynamicString("First Name: ");
    if (firstName) {
        newContact.firstNamePosn = currentPosition;
        // Adjust for the length of firstName
        currentPosition += strlen(firstName) + 1;
    }

    char *lastName = readDynamicString("Last Name: ");
    if (lastName) {
        newContact.lastNamePosn = currentPosition;
        // Adjust for the length of lastName
        currentPosition += strlen(lastName) + 1;
    }

    char *email = readDynamicString("Email: ");
    if (email) {
        newContact.emailPosn = currentPosition;
        // Adjust for the length of email
        currentPosition += strlen(email) + 1;
    }
    // Next record starts after the current record
    newContact.next = currentPosition;

    // Write the contact structure
    fseek(fp, recordStartPos, SEEK_SET);
    fwrite(&newContact, sizeof(struct contact), 1, fp);

    // Write the employee ID
    fwrite(&empId, sizeof(long), 1, fp);

    // Write the strings to the file(first, last name and email)
    if (firstName) fwrite(firstName, strlen(firstName) + 1, 1, fp);
    if (lastName) fwrite(lastName, strlen(lastName) + 1, 1, fp);
    if (email) fwrite(email, strlen(email) + 1, 1, fp);

    // Free dynamically allocated memory
    free(firstName);
    free(lastName);
    free(email);

    // Close file
    fclose(fp);
}

void readContacts(char *fileName) {
    // Open the file in read binary mode
    FILE *fp = fopen(fileName, "rb");
    if (!fp) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    struct contact readContact;
    while (fread(&readContact, sizeof(struct contact), 1, fp) == 1) {
        // Read and print Employee ID
        fseek(fp, readContact.empIdPosn, SEEK_SET);
        long empId;
        fread(&empId, sizeof(long), 1, fp);
        printf("****************************\n");
        printf("Employee Id: %ld\n", empId);

        // Read and print First Name if it exists
        if (readContact.firstNamePosn != -1) {
            fseek(fp, readContact.firstNamePosn, SEEK_SET);
            char firstName[100]; // Assuming a max length for simplicity
            fgets(firstName, 100, fp);
            printf("First Name: %s\n", firstName);
        }

        // Read and print Last Name if it exists
        if (readContact.lastNamePosn != -1) {
            fseek(fp, readContact.lastNamePosn, SEEK_SET);
            char lastName[100]; // Same assumption as First Name
            fgets(lastName, 100, fp);
            printf("Last Name: %s\n", lastName);
        }

        // Read and print Email if it exists
        if (readContact.emailPosn != -1) {
            fseek(fp, readContact.emailPosn, SEEK_SET);
            char email[100];
            fgets(email, 100, fp);
            printf("Email: %s\n", email);
        }

        printf("****************************\n");

        // Move to the next contact record
        fseek(fp, readContact.next, SEEK_SET);
    }

    // Close file
    fclose(fp);
}

// Helper function to read a string from the file
char *readStringFromFile(FILE *fp, long position) {
    // Allocate memory for the string
    char *str = (char *)malloc(100 * sizeof(char));
    if (str == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    fseek(fp, position, SEEK_SET);
    // Read the string
    fgets(str, 100, fp);
    // Ensure null-termination
    str[strcspn(str, "\n")] = 0;
    return str;
}

int searchContacts(char *fileName, int keyId) {
    FILE *fp = fopen(fileName, "rb");
    if (!fp) {
        perror("Failed to open file");
        return 0;
    }

    struct contact readContact;
    int found = 0;
    while (!found && fread(&readContact, sizeof(struct contact), 1, fp) == 1) {
        long empId;
        fseek(fp, readContact.empIdPosn, SEEK_SET);
        fread(&empId, sizeof(long), 1, fp);
        if (empId == keyId) {
            found = 1; // Mark as found
            printf("****************************\n");
            printf("Employee Id: %ld\n", empId);

            if (readContact.firstNamePosn != -1) {
                char *firstName = readStringFromFile(fp, readContact.firstNamePosn);
                printf("First Name: %s\n", firstName);
                free(firstName);
            }

            if (readContact.lastNamePosn != -1) {
                char *lastName = readStringFromFile(fp, readContact.lastNamePosn);
                printf("Last Name: %s\n", lastName);
                free(lastName);
            }

            if (readContact.emailPosn != -1) {
                char *email = readStringFromFile(fp, readContact.emailPosn);
                printf("Email: %s\n", email);
                free(email);
            }

            printf("****************************\n");
        }

        // Move to the next contact record
        if (!found && readContact.next != -1) {
            fseek(fp, readContact.next, SEEK_SET);
        } else {
            break;
        }
    }

    fclose(fp);
    // Return 1 if found, 0 if not
    return found;
}
