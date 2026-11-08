#include <stdio.h>

int main(void) {
    FILE *file = fopen("sample.csv", "r");   
    if (file == NULL) {
        printf("Không mở được file!\n");
        return 1;
    }

    char line[256];   

    fgets(line, sizeof(line), file); 

    while (fgets(line, sizeof(line), file) != NULL){
        printf("%s", line);
    }
    fclose(file);
    return 0;
}