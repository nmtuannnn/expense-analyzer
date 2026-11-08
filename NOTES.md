# Development Notes

These notes record the problems I ran into while building this tool and why I chose each fix. I want to remember not only what the code does, but how I got there.

## 1. Storing money as cents
At first I planned to store amounts as `double`. Yet when I printed `0.1 + 0.2` with 20 decimal places, the result was not exactly 0.3. A small error like this looks harmless, but it adds up over thousands of transactions, and with money even one cent is wrong.
- I store every amount as a `long` number of cents (12.50 -> 1250).
- I use `long` instead of `int` so that large totals do not overflow.
- When printing, I use `%ld.%02ld`. Without the `02`, 1205 cents would print as "12.5" instead of "12.05".

## 2. Reading the CSV file
- `fgets` keeps the newline `\n` at the end of each line. If I do not remove it, the amount becomes "12.50\n". I remove it with `strcspn` right after reading.
- The first line is the header (`Date,Description,Amount`), not a transaction. I skip it by calling `fgets` once before the loop.

## 3. Empty columns and `strtok`
I split each line with `strtok`. Then I tested the line `09/11/2026,,7.00`, which has no description. I expected an error, but `strtok` skipped the empty field: the amount moved into the description, and the amount became NULL. The program did not crash, which makes this worse, because the data was wrong without any warning.
- I now check that the date, description and amount are not NULL, and skip the line if one is missing.

## 4. Parsing amounts
My first version used `sscanf("%ld.%ld", ...)`. It worked for "12.50", but failed in two cases:
1) "3.6" was read as 3 dollars and 6 cents (3.06) instead of 3.60.
2) "7" has no decimal point, so `sscanf` only filled the dollars and left the cents with a random value.

I looked at `strtod` as a shorter option. Yet it is too loose for money: it accepts "5.999", "1e3" and "inf" without any complaint. If a bank file contains "5.999", something is wrong, and I want the program to tell me instead of guessing.

So I wrote my own parser. It reads the string one character at a time and turns each digit into a number with `ch - '0'`.
- It accepts only digits and at most one decimal point.
- It counts the digits after the point and pads them to two places ("3.6" -> 360 cents, "7" -> 700 cents).
- It rejects letters, a second dot, more than two decimal digits, and empty strings.

## 5. Keeping every transaction
My first program only kept a running total. Each new line overwrote the old one, so after the loop I could not ask questions like "how much did I spend on Grab?".
- I now store each transaction in a `struct` (date, description, category, amount) inside an array.
- I have to copy the text with `snprintf`. If I only saved the pointers from `strtok`, every transaction would point into the same `line` buffer, and all of them would show the last line of the file.
- The array has a fixed size (`MAX_TRANSACTIONS`), so I check the count before saving and stop with "Overlimit!" instead of writing outside the array.

I also found a small bug here. I added each amount to the total before checking the limit. If the file was too long, the last transaction would be counted in the total but not saved in the array, so the two would not match. I moved the total so that it only adds an amount after the transaction is saved.

## 6. Printing a readable table
- I use fixed widths in `printf`: `%-12s` for text (left aligned) and `%7ld.%02ld` for money (right aligned), so the dots in the amounts line up.
- `%-20.20s` cuts long descriptions to 20 characters, so one long name does not push the whole row out of line.
- At one point I printed the total with `%d`. The result looked correct, but `gcc` warned me, because `total` is a `long`. It only worked by luck. Since then I compile with `-Wall -Wextra` and read every warning, not just whether the program runs.

## 7. Sorting transactions into categories
I match each description against a list of keywords with `strstr` (for example GRAB -> Transport, NTUC -> Groceries). If nothing matches, the category is "Other".
- The first rule that matches wins. So "GRABFOOD KFC" was first sorted as Transport, because GRAB came before any food rule. I fixed this by putting specific keywords (GRABFOOD, SHOPEEFOOD) above general ones (GRAB, SHOPEE).
- The main weakness of this method is that someone has to write the rules by hand, and any new shop falls into "Other". Yet most personal spending repeats: a small number of rules already covers most of my money.

## 8. Moving the rules into `rules.txt`
At first the rules were written inside the code, so changing one rule meant compiling again. I moved them into `rules.txt`, with about 150 common shops in Singapore and Vietnam.
- The file uses the same format as the CSV (`KEYWORD,Category`), so I read it with the same `fgets` + `strtok` method. Lines starting with `#` are comments, so I can group the rules by country and type.
- Short keywords are risky with `strstr`. "TEA" also matches "STEAK HOUSE", and "BUS" also matches "BUSINESS CLASS". So I use full brand names, for example "M1 LIMITED" instead of "M1".
- I left out payment apps such as MoMo, ZaloPay and PayLah on purpose. They show how I paid, not what I bought.
-git add NOTES.md
git commit -m "Update development notes"
git push