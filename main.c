#include <stdio.h>
#include <string.h>

#define MAX_TRANSACTIONS 1000
#define MAX_RULES 500

typedef struct {
  char date[11];
  char description[64];
  char category[32];
  long amount_cents;
} Transaction;

typedef struct {
  char keyword[32];
  char category[32];
} Rule;

Rule rules[MAX_RULES];
int rule_count = 0;


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


int load_rules(const char *filename) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    printf("Invalid!\n");
    return 0;
  }

  char line[128];
  while (fgets(line, sizeof(line), file) != NULL) {
    line[strcspn(line, "\n")] = '\0';

    
    if (line[0] == '\0' || line[0] == '#') {
      continue;
    }

    char *keyword = strtok(line, ",");
    char *category = strtok(NULL, ",");
    if (keyword == NULL || category == NULL) {
      printf("Error!!\n");
      continue;
    }

    if (rule_count >= MAX_RULES) {
      printf("Overlimit!\n");
      break;
    }

    snprintf(rules[rule_count].keyword, sizeof(rules[rule_count].keyword), "%s", keyword);
    snprintf(rules[rule_count].category, sizeof(rules[rule_count].category), "%s", category);
    rule_count++;
  }

  fclose(file);
  return rule_count;
}


const char *categorize(const char *description) {
  for (int i = 0; i < rule_count; i++) {
    if (strstr(description, rules[i].keyword) != NULL) {
      return rules[i].category;
    }
  }
  return "Other";
}

int main(void) {
  int loaded = load_rules("rules.txt");
  printf("Loaded %d rules\n", loaded);

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
    snprintf(transactions[count].category, sizeof(transactions[count].category), "%s",
             categorize(desc));
    transactions[count].amount_cents = amount_cents;
    count++;

    total += amount_cents;
  }

  fclose(file);

 
  printf("%-12s %-20s %-17s %10s\n", "Date", "Description", "Category", "Amount");
  printf("%-12s %-20s %-17s %10s\n", "----------", "--------------------",
         "-----------------", "----------");

  for (int i = 0; i < count; i++) {
    printf("%-12s %-20.20s %-17s %7ld.%02ld\n",
           transactions[i].date,
           transactions[i].description,
           transactions[i].category,
           transactions[i].amount_cents / 100,
           transactions[i].amount_cents % 100);
  }

  printf("%-12s %-20s %-17s %10s\n", "", "", "", "----------");
  printf("%-51s %7ld.%02ld\n", "TOTAL", total / 100, total % 100);
  printf("Saved %d transactions\n", count);

 
  char box_names[50][32];
  long box_totals[50];
  int box_count = 0;

  for (int i = 0; i < count; i++) {
  
    int found = -1;
    for (int j = 0; j < box_count; j++) {
      if (strcmp(box_names[j], transactions[i].category) == 0) {
        found = j;
      }
    }

   
    if (found == -1) {
      if (box_count >= 50) {
        printf("Overlimit!\n");
        break;
      }
      snprintf(box_names[box_count], sizeof(box_names[box_count]), "%s",
               transactions[i].category);
      box_totals[box_count] = 0;
      found = box_count;
      box_count++;
    }

 
    box_totals[found] += transactions[i].amount_cents;
  }

 
  printf("\n%-17s %12s\n", "Category", "Amount");
  printf("%-17s %12s\n", "-----------------", "----------");
  for (int j = 0; j < box_count; j++) {
    printf("%-17s %9ld.%02ld\n", box_names[j], box_totals[j] / 100,
           box_totals[j] % 100);
  }

  return 0;
}

