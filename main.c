#include <stdio.h>
#include <string.h>

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

        long amount_cents = parse_amount(amount);
        if (amount_cents == -1) {
            printf("Error!!\n");
            continue;
        }
        total += amount_cents;

        printf("Date: %s | Description: %s | Amount: %s\n", date, desc, amount);
    }
    printf("Total amount of money: %ld.%02ld\n", total / 100, total % 100);
    fclose(file);
    return 0;
}