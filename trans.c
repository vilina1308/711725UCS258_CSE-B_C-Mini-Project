#include <stdio.h>
#include <stdlib.h>

struct clientData
{
    unsigned int acctNum;
    char lastName[15];
    char firstName[10];
    double balance;
    int pin;
};

// Function prototypes
unsigned int enterChoice(void);
void textFile(FILE *readPtr);
void updateRecord(FILE *fPtr);
void newRecord(FILE *fPtr);
void deleteRecord(FILE *fPtr);
void viewAccount(FILE *fPtr);
void viewAll(FILE *fPtr);
void transfer(FILE *fPtr);
void addInterest(FILE *fPtr);
int checkPIN(struct clientData client);

// ---------------- PIN CHECK ----------------
int checkPIN(struct clientData client)
{
    int enteredPin;
    printf("Enter PIN: ");
    scanf("%d", &enteredPin);

    if (enteredPin != client.pin)
    {
        printf("Incorrect PIN!\n");
        return 0;
    }
    return 1;
}

// ---------------- MENU ----------------
unsigned int enterChoice(void)
{
    unsigned int choice;
    printf("\n1.Text File\n2.Update Account\n3.New Account\n4.Delete Account\n5.View Account\n6.View All Accounts\n7.Transfer Money\n8.Add Interest\n9.Exit\n? ");
    scanf("%u", &choice);
    return choice;
}

// ---------------- VIEW ACCOUNT ----------------
void viewAccount(FILE *fPtr)
{
    struct clientData client = {0};
    unsigned int acc;

    printf("Enter account number: ");
    scanf("%u", &acc);

    if (acc < 1 || acc > 100)
    {
        printf("Invalid account number!\n");
        return;
    }

    fseek(fPtr, (acc - 1) * sizeof(struct clientData), SEEK_SET);

    if (fread(&client, sizeof(struct clientData), 1, fPtr) != 1 || client.acctNum == 0)
    {
        printf("Account not found\n");
        return;
    }

    if (!checkPIN(client))
        return;

    printf("%-5u %-10s %-10s %.2f\n",
           client.acctNum,
           client.firstName,
           client.lastName,
           client.balance);
}

// ---------------- VIEW ALL ----------------
void viewAll(FILE *fPtr)
{
    struct clientData client;
    rewind(fPtr);

    printf("\n%-5s %-10s %-10s %-10s\n", "Acc", "First", "Last", "Balance");
    printf("---------------------------------------\n");

    while (fread(&client, sizeof(struct clientData), 1, fPtr) == 1)
    {
        if (client.acctNum != 0)
        {
            printf("%-5u %-10s %-10s %-10.2f\n",
                   client.acctNum,
                   client.firstName,
                   client.lastName,
                   client.balance);
        }
    }
}

// ---------------- NEW RECORD ----------------
void newRecord(FILE *fPtr)
{
    struct clientData client = {0};
    unsigned int account;

    printf("Enter new account number: ");
    scanf("%u", &account);

    if (account < 1 || account > 100)
    {
        printf("Invalid account number!\n");
        return;
    }

    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);

    if (fread(&client, sizeof(struct clientData), 1, fPtr) != 1)
    {
        client.acctNum = 0;
    }

    if (client.acctNum != 0)
    {
        printf("Account already exists.\n");
        return;
    }

    printf("Enter lastname firstname balance:\n");
    scanf("%14s %9s %lf",
          client.lastName,
          client.firstName,
          &client.balance);

    client.acctNum = account;
    client.pin = 1234;

    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    fwrite(&client, sizeof(struct clientData), 1, fPtr);

    fflush(fPtr);

    printf("Account created successfully. Default PIN = 1234\n");
}

// ---------------- UPDATE RECORD ----------------
void updateRecord(FILE *fPtr)
{
    struct clientData client = {0};
    unsigned int account;
    double transaction;

    printf("Enter account number: ");
    scanf("%u", &account);

    if (account < 1 || account > 100)
    {
        printf("Invalid account number!\n");
        return;
    }

    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);

    if (fread(&client, sizeof(struct clientData), 1, fPtr) != 1 || client.acctNum == 0)
    {
        printf("Account not found.\n");
        return;
    }

    if (!checkPIN(client))
        return;

    printf("Current Balance: %.2f\n", client.balance);

    printf("Enter amount (+ deposit / - withdraw): ");
    scanf("%lf", &transaction);

    if (transaction < 0 && client.balance < -transaction)
    {
        printf("Insufficient balance!\n");
        return;
    }

    client.balance += transaction;

    fseek(fPtr, -(long)sizeof(struct clientData), SEEK_CUR);
    fwrite(&client, sizeof(struct clientData), 1, fPtr);

    fflush(fPtr);

    printf("Updated Balance: %.2f\n", client.balance);
}

// ---------------- DELETE RECORD ----------------
void deleteRecord(FILE *fPtr)
{
    struct clientData client = {0};
    struct clientData blank = {0};
    unsigned int account;

    printf("Enter account number: ");
    scanf("%u", &account);

    if (account < 1 || account > 100)
    {
        printf("Invalid account number!\n");
        return;
    }

    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);

    if (fread(&client, sizeof(struct clientData), 1, fPtr) != 1 || client.acctNum == 0)
    {
        printf("Account does not exist.\n");
        return;
    }

    if (!checkPIN(client))
        return;

    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    fwrite(&blank, sizeof(struct clientData), 1, fPtr);

    fflush(fPtr);

    printf("Account deleted.\n");
}

// ---------------- TRANSFER ----------------
void transfer(FILE *fPtr)
{
    struct clientData c1 = {0}, c2 = {0};
    unsigned int from, to;
    double amount;

    printf("From account: ");
    scanf("%u", &from);

    printf("To account: ");
    scanf("%u", &to);

    if (from < 1 || from > 100 || to < 1 || to > 100 || from == to)
    {
        printf("Invalid accounts!\n");
        return;
    }

    printf("Amount: ");
    scanf("%lf", &amount);

    if (amount <= 0)
    {
        printf("Invalid amount!\n");
        return;
    }

    fseek(fPtr, (from - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&c1, sizeof(struct clientData), 1, fPtr);

    fseek(fPtr, (to - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&c2, sizeof(struct clientData), 1, fPtr);

    if (c1.acctNum == 0 || c2.acctNum == 0)
    {
        printf("Invalid accounts\n");
        return;
    }

    if (!checkPIN(c1))
        return;

    if (c1.balance < amount)
    {
        printf("Insufficient balance\n");
        return;
    }

    c1.balance -= amount;
    c2.balance += amount;

    fseek(fPtr, (from - 1) * sizeof(struct clientData), SEEK_SET);
    fwrite(&c1, sizeof(struct clientData), 1, fPtr);

    fseek(fPtr, (to - 1) * sizeof(struct clientData), SEEK_SET);
    fwrite(&c2, sizeof(struct clientData), 1, fPtr);

    fflush(fPtr);

    printf("Transfer successful\n");
}

// ---------------- ADD INTEREST ----------------
void addInterest(FILE *fPtr)
{
    struct clientData client;
    rewind(fPtr);

    while (fread(&client, sizeof(struct clientData), 1, fPtr) == 1)
    {
        if (client.acctNum != 0 && client.balance > 0)
        {
            client.balance += client.balance * 0.05;

            fseek(fPtr, -(long)sizeof(struct clientData), SEEK_CUR);
            fwrite(&client, sizeof(struct clientData), 1, fPtr);
        }
    }

    fflush(fPtr);
    printf("Interest added to all accounts.\n");
}

// ---------------- TEXT FILE (FORMATTED) ----------------
void textFile(FILE *readPtr)
{
    FILE *writePtr = fopen("accounts.txt", "w");
    struct clientData client;

    if (writePtr == NULL)
    {
        printf("File could not be created.\n");
        return;
    }

    rewind(readPtr);

    fprintf(writePtr, "===============================================\n");
    fprintf(writePtr, "%-10s %-15s %-15s %-10s\n",
            "Account", "First Name", "Last Name", "Balance");
    fprintf(writePtr, "===============================================\n");

    while (fread(&client, sizeof(struct clientData), 1, readPtr) == 1)
    {
        if (client.acctNum != 0)
        {
            fprintf(writePtr, "%-10u %-15s %-15s %-10.2f\n",
                    client.acctNum,
                    client.firstName,
                    client.lastName,
                    client.balance);
        }
    }

    fprintf(writePtr, "===============================================\n");

    fclose(writePtr);
    printf("Data written to accounts.txt (formatted)\n");
}

// ---------------- MAIN ----------------
int main(void)
{
    FILE *cfPtr;
    unsigned int choice;

    cfPtr = fopen("credit.dat", "rb+");

    if (cfPtr == NULL)
    {
        cfPtr = fopen("credit.dat", "wb+");

        struct clientData blank = {0};

        for (int i = 0; i < 100; i++)
        {
            fwrite(&blank, sizeof(struct clientData), 1, cfPtr);
        }
    }

    while ((choice = enterChoice()) != 9)
    {
        switch (choice)
        {
        case 1: textFile(cfPtr); break;
        case 2: updateRecord(cfPtr); break;
        case 3: newRecord(cfPtr); break;
        case 4: deleteRecord(cfPtr); break;
        case 5: viewAccount(cfPtr); break;
        case 6: viewAll(cfPtr); break;
        case 7: transfer(cfPtr); break;
        case 8: addInterest(cfPtr); break;
        default: printf("Invalid choice\n");
        }
    }

    fclose(cfPtr);
    return 0;
}