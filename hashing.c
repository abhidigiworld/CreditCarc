#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>

#define PASSWORD "Abhi@9988"
#define HASH_SIZE 4096

// Card details
typedef struct card {
    char cardType[7];
    char cardNumber[17];
    unsigned short int expiryMonth : 4;
    unsigned short int expiryYear : 12;
    unsigned short int issueMonth : 4;
    unsigned short int issueYear : 12;
    unsigned short int cvv : 10;
} Card;

// Customer details
typedef struct customer {
    char firstName[20];
    char lastName[20];
    unsigned short int birthDay : 5;
    unsigned short int birthMonth : 4;
    unsigned short int birthYear : 12;
    char bankName[9];
    char address[200];
    char identification[15];
    char email[100];
    Card *cardDetails;
} Customer;


// Linked list node for chaining
typedef struct customerNode {
    Customer *data;
    struct customerNode *next;
} CustomerNode;

// Chained hash tables
CustomerNode *hashByCard[HASH_SIZE] = {NULL};
CustomerNode *hashByName[HASH_SIZE] = {NULL};


unsigned int hash(const char *str) {
    unsigned long hash = 5381;
    while (*str) {
        hash = hash * 33 + *str;
        str++;
    }
    return hash % HASH_SIZE;
}

// Add to chained hash table with both by card or card no
void addToHash(CustomerNode **table, unsigned int index, Customer *cust) {
    CustomerNode *newNode = (CustomerNode *)malloc(sizeof(CustomerNode));
    newNode->data = cust;
    newNode->next = table[index];
    table[index] = newNode;
}

// Search in hash table
Customer *searchHash(CustomerNode *head, const char *key, int byCard) {
    while (head) {
        if (byCard) {
            if (strcmp(head->data->cardDetails->cardNumber, key) == 0)
                return head->data;
        } else {
            char temp[50];
            sprintf(temp, "%s%s", head->data->firstName, head->data->lastName);
            if (strcmp(temp, key) == 0)
                return head->data;
        }
        head = head->next;
    }
    return NULL;
}

// Delete from hash table
void deleteFromHash(CustomerNode **table, unsigned int index, const char *key, int byCard) {
    CustomerNode *curr = table[index], *prev = NULL;
    while (curr) {
        if ((byCard && strcmp(curr->data->cardDetails->cardNumber, key) == 0) ||
            (!byCard && strcmp(curr->data->firstName, key) == 0)) {
            if (prev) prev->next = curr->next;
            else table[index] = curr->next;
            free(curr->data);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void writeCustomerToFile(Customer *cust) {
    FILE *fp = fopen("customerdata.txt", "a");
    if (!fp) {
        perror("File opening failed");
        return;
    }

    fprintf(fp, "===========================\n");
    fprintf(fp, "NAME: %s %s\n", cust->firstName, cust->lastName);
    fprintf(fp, "DOB: %02d/%02d/%d\n", cust->birthDay, cust->birthMonth, cust->birthYear);
    fprintf(fp, "Bank: %s\n", cust->bankName);
    fprintf(fp, "Address: %s\n", cust->address);
    fprintf(fp, "ID: %s\n", cust->identification);
    fprintf(fp, "Email: %s\n", cust->email);
    fprintf(fp, "CARD NO: %s\n", cust->cardDetails->cardNumber);
    fprintf(fp, "CVV: %hd\n", cust->cardDetails->cvv);
    fprintf(fp, "Issue: %02hd/%d\n", cust->cardDetails->issueMonth, cust->cardDetails->issueYear);
    fprintf(fp, "Expiry: %02hd/%d\n", cust->cardDetails->expiryMonth, cust->cardDetails->expiryYear);
    fprintf(fp, "Card Type: %s\n", cust->cardDetails->cardType);
    fprintf(fp, "===========================\n\n");

    fclose(fp);
}

Card *generateRandomCard(char *bankName) {
    Card *newCard = (Card *)malloc(sizeof(Card));
    
    if (strcmp(bankName, "AMERICAN") == 0) {
        strcpy(newCard->cardType, "AMEX");
        for (int i = 0; i < 15; i++)
            newCard->cardNumber[i] = '0' + (rand() % 10);
        newCard->cardNumber[15] = '\0';
    } else if (strcmp(bankName, "MeToo") == 0) {
        int len = (rand() % 2 == 0) ? 15 : 16;
        strcpy(newCard->cardType, (len == 15) ? "Me15" : "Me16");
        for (int i = 0; i < len; i++)
            newCard->cardNumber[i] = '0' + (rand() % 10);
        newCard->cardNumber[len] = '\0';
    } else {
        strcpy(newCard->cardType, (rand() % 2 == 0) ? "Visa" : "Master");
        for (int i = 0; i < 16; i++)
            newCard->cardNumber[i] = '0' + (rand() % 10);
        newCard->cardNumber[16] = '\0';
    }

    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    newCard->issueMonth = tm_info->tm_mon + 1;
    newCard->issueYear = tm_info->tm_year + 1900;
    newCard->expiryYear = newCard->issueYear + 5;
    newCard->expiryMonth = newCard->issueMonth;
    newCard->cvv = rand() % 900 + 100;

    return newCard;
}

int isValidEmail(const char *email) {
    int at = 0, dot = 0;
    for (int i = 0; email[i]; i++) {
        if (email[i] == '@') at++;
        if (email[i] == '.') dot++;
    }
    if (!(at == 1 && dot >= 1)) printf("\nInvalid Email id\n");
    return at == 1 && dot >= 1;
}

int isValidDate(int d, int m, int y) {
    if (y < 1900 || y > 2100 || m < 1 || m > 12 || d < 1 || d > 31)
        return 0;
    int daysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (m == 2 && ((y % 4 == 0 && y % 100 != 0) || y % 400 == 0))
        daysInMonth[2] = 29;
    if (d > daysInMonth[m]) printf("\nInvalid Date\n");
    return d <= daysInMonth[m];
}

int isValidName(const char *name) {
    if (strlen(name) < 1 || strlen(name) > 19){
        printf("\nInvalid Name \n");
        return 0;
    };
    for (int i = 0; name[i]; i++) {
        if (!isalpha(name[i])){
            printf("\nInvalid Name \n");
            return 0;
        }
    }
    return 1;
}

void printcard(Customer *nw) {
    printf("\a\n===========================\n");
    printf("|       CARD DETAILS       |\n");
    printf("===========================\n");
    printf("| CARD NO: %s\n", nw->cardDetails->cardNumber);
    printf("| CVV: %hd\n", nw->cardDetails->cvv);
    printf("| EXP: %hd/%hd\t%s\n", nw->cardDetails->expiryMonth, nw->cardDetails->expiryYear, nw->cardDetails->cardType);
    printf("| NAME: %s %s\n", nw->firstName, nw->lastName);
    printf("===========================\n");
}

void addFunction(short int choice) {
    Customer *cust = (Customer *)malloc(sizeof(Customer));
    do {
        printf("Enter First Name: ");
        scanf("%s", cust->firstName);
    } while (!isValidName(cust->firstName));
    do {
        printf("Enter Last Name: ");
        scanf("%s", cust->lastName);
    } while (!isValidName(cust->lastName));
    int d, m, y;
    do {
        printf("Enter DOB (DD MM YYYY): ");
        scanf("%d %d %d", &d, &m, &y);
    } while (!isValidDate(d, m, y));
    cust->birthDay = d; cust->birthMonth = m; cust->birthYear = y;

Bank:
    printf("SELECT BANK: 1.CITI 2.ICICI 3.AMERICAN 4.MeToo\n");
    int b; scanf("%d", &b);
    switch (b) {
        case 1: strcpy(cust->bankName, "CITI"); break;
        case 2: strcpy(cust->bankName, "ICICI"); break;
        case 3: strcpy(cust->bankName, "AMERICAN"); break;
        case 4: strcpy(cust->bankName, "MeToo"); break;
        default: printf("Invalid. Try again.\n"); goto Bank;
    }

    printf("Enter Address: "); getchar();
    fgets(cust->address, sizeof(cust->address), stdin);
    cust->address[strcspn(cust->address, "\n")] = '\0';
    printf("Enter ID: "); scanf("%s", cust->identification);
    do {
        printf("Enter Email: "); scanf("%s", cust->email);
    } while (!isValidEmail(cust->email));

    cust->cardDetails = generateRandomCard(cust->bankName);
    printcard(cust);

    unsigned int cardIndex = hash(cust->cardDetails->cardNumber);
    addToHash(hashByCard, cardIndex, cust);

    char fullName[50];
    sprintf(fullName, "%s%s", cust->firstName, cust->lastName);
    unsigned int nameIndex = hash(fullName);
    addToHash(hashByName, nameIndex, cust);

    writeCustomerToFile(cust);
    printf("Customer added & card generated successfully.\n");
}

void displayCardDetails(const char *cardNumber) {
    unsigned int index = hash(cardNumber);
    Customer *cust = searchHash(hashByCard[index], cardNumber, 1);
    if (cust) printcard(cust);
    else printf("Card not found.\n");
}

void displayCardDetailsByName(const char *first, const char *last) {
    char fullName[50];
    sprintf(fullName, "%s%s", first, last);
    unsigned int index = hash(fullName);
    Customer *cust = searchHash(hashByName[index], fullName, 0);
    if (cust) printcard(cust);
    else printf("Customer not found.\n");
}

void viewAll() {
    char pass[20];
    printf("Enter Password: "); scanf("%s", pass);
    if (strcmp(pass, PASSWORD) != 0) {
        printf("Incorrect password.\n"); return;
    }

    int found = 0;
    for (int i = 0; i < HASH_SIZE; i++) {
        CustomerNode *head = hashByCard[i];
        while (head) {
            printcard(head->data);
            head = head->next;
            found = 1;
        }
    }

    if (!found) {
        printf("Nothing to print.\n");
    }
}

void deletebyCardno(const char *cardNumber) {
    unsigned int cardIndex = hash(cardNumber);
    Customer *cust = searchHash(hashByCard[cardIndex], cardNumber, 1);
    if (cust) {
        char fullName[50];
        sprintf(fullName, "%s%s", cust->firstName, cust->lastName);
        unsigned int nameIndex = hash(fullName);
        deleteFromHash(hashByCard, cardIndex, cardNumber, 1);
        deleteFromHash(hashByName, nameIndex, cust->firstName, 0);
        printf("Deleted Customer: %s %s, Card No: %s\n", cust->firstName, cust->lastName, cust->cardDetails->cardNumber);
        printf("Customer deleted successfully.\n");
    } else {
        printf("Card not found.\n");
    }
}

void deletebyName(const char *first, const char *last) {
    char fullName[50];
    sprintf(fullName, "%s%s", first, last);
    unsigned int nameIndex = hash(fullName);
    Customer *cust = searchHash(hashByName[nameIndex], fullName, 0);
    if (cust) {
        unsigned int cardIndex = hash(cust->cardDetails->cardNumber);
        deleteFromHash(hashByCard, cardIndex, cust->cardDetails->cardNumber, 1);
        deleteFromHash(hashByName, nameIndex, first, 0);
        printf("Deleted Customer: %s %s, Card No: %s\n", cust->firstName, cust->lastName, cust->cardDetails->cardNumber);
        printf("Customer deleted successfully.\n");
    } else {
        printf("Customer not found.\n");
    }
}

void printCustomerFile() {
    FILE *fp = fopen("customerdata.txt", "r");
    if (!fp) {
        printf("No file found or cannot open file.\n");
        return;
    }

    char line[256];
    printf("\n========== FILE CONTENT ==========\n");
    while (fgets(line, sizeof(line), fp)) {
        printf("%s", line);
    }
    fclose(fp);
}

void menu() {
    int val;
    char cardNo[17], firstName[40], lastName[40];
    do {
        printf("\n========= CARD MANAGEMENT =========\n");
        printf("1. Add Customer\n2. Search by Card\n3. Search by Name\n4. View All\n5. Delte by card no\n6. Delet by name\n7. Read Customer Data from File\n8. Exit\nEnter: ");
        scanf("%d", &val);
        system("clear");
        switch (val) {
            case 1: {
                short ch;
                printf("1. Insert by Card\n2. Insert by Name\n");
                scanf("%hd", &ch);
                addFunction(ch);
                break;
            }
            case 2:
                printf("Enter Card No: "); scanf("%s", cardNo);
                displayCardDetails(cardNo);
                break;
            case 3:
                printf("Enter First Name: "); scanf("%s", firstName);
                printf("Enter Last Name: "); scanf("%s", lastName);
                displayCardDetailsByName(firstName, lastName);
                break;
            case 4:
                viewAll();
                break;
            case 5:
                printf("Enter Card No: "); 
                scanf("%s", cardNo);
                deletebyCardno(cardNo);
                break;
            case 6:
                printf("Enter First Name: "); 
                scanf("%s", firstName);
                printf("Enter Last Name: "); 
                scanf("%s", lastName);
                deletebyName(firstName,lastName);
                break;
            case 7:
                printCustomerFile(); 
                break;
            case 8:
                printf("Exiting...\a\n"); 
                break;
            default:
                printf("Invalid input.\n");
        }
    } while (val != 8);
}

int main() {
    srand(time(0));
    menu();
    return 0;
}