#include <stdio.h>
#include <string.h>

int main(void) {
    FILE *file = fopen("sample.csv", "r");
    if (file == NULL) {
        printf("Không mở được file!\n");
        return 1;
    }

    char line[256];
    fgets(line, sizeof(line), file);
    while (fgets(line, sizeof(line), file) != NULL) {

        line[strcspn(line, "\n")] = '\0';

        char *date = strtok(line, ",");
        char *desc = strtok(NULL, ",");
        char *amount = strtok(NULL, ",");

        printf("Date: %s | Description: %s | Amount: %s\n", date, desc, amount);
    }
    fclose(file);
    return 0;
}