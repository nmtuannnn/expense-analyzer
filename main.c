#include <stdio.h>
#include <string.h>

int main(void) {
    FILE *file = fopen("sample.csv", "r");
    if (file == NULL) {
        printf("Không mở được file!\n");
        return 1;
    }

    char line[256];
    long total = 0;

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

        long dollars, cents;
        int n = sscanf(amount, "%ld.%ld", &dollars, &cents);
        long amount_cents = dollars * 100 + cents;
        total += amount_cents;

        printf("Date: %s | Description: %s | Amount: %s\n", date, desc, amount);
    }
    printf("Total amount of money: %ld.%02ld\n", total / 100, total % 100);
    fclose(file);
    return 0;
}