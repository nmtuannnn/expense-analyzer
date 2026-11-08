#include <stdio.h>
#include <string.h>

#define MAX_TRANSACTIONS 1000                        

typedef struct {
    char date[11];
    char description[64];
    long amount_cents;
} Transaction;

long parse_amount(char *s) {
    long result = 0;
    int seen_dot = 0;
    int decimals = 0;
    int digits = 0;

    for (int i = 0; s[i] != '\0'; i++) {
        char c = s[i];

        if (c >= '0' && c <= '9') {
            result = result * 10 + (c - '0');
            digits++;
            if (seen_dot) {
                decimals++;
            }
        } else if (c == '.') {
            if (seen_dot) {
                return -1;
            }
            seen_dot = 1;
        } else {
            return -1;
        }
    }

    if (digits == 0 || decimals > 2) {
        return -1;
    }

    if (decimals == 0) {
        result = result * 100;
    } else if (decimals == 1) {
        result = result * 10;
    }

    return result;
}

int main(void) {

    FILE *file = fopen("sample.csv", "r");
    if (file == NULL) {
        printf("Invalid!\n");
        return 1;
    }

    char line[256];
    long total = 0;
    Transaction transactions[MAX_TRANSACTIONS];      
    int count = 0;

    fgets(line, sizeof(line), file);
    while (fgets(line, sizeof(line), file) != NULL) {

        line[strcspn(line, "\n")] = '\0';

        char *date = strtok(line, ",");
        char *desc = strtok(NULL, ",");
        char *amount = strtok(NULL, ",");

        if (date == NULL || desc == NULL || amount == NULL) {
            printf("Error!!\n");
            continue;
        }

        long amount_cents = parse_amount(amount);
        if (amount_cents == -1) {
            printf("Error!!\n");
            continue;
        }

        if (count >= MAX_TRANSACTIONS) {               
            printf("Overlimit!\n");
            break;
        }

        snprintf(transactions[count].date, sizeof(transactions[count].date), "%s", date);
        snprintf(transactions[count].description, sizeof(transactions[count].description), "%s", desc);
        transactions[count].amount_cents = amount_cents;
        count++;

        total += amount_cents;                        
    }

    fclose(file);                                      

    // In bảng
    printf("%-12s %-20s %10s\n", "Date", "Description", "Amount");                      
    printf("%-12s %-20s %10s\n", "----------", "--------------------", "----------");  

    for (int i = 0; i < count; i++) {
        printf("%-12s %-20.20s %7ld.%02ld\n",         
               transactions[i].date,
               transactions[i].description,
               transactions[i].amount_cents / 100,
               transactions[i].amount_cents % 100);
    }

    printf("%-12s %-20s %10s\n", "", "", "----------");                                
    printf("%-33s %7ld.%02ld\n", "TOTAL", total / 100, total % 100);                
    printf("Saved %d transactions\n", count);

    return 0;
}