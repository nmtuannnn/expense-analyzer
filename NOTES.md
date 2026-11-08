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

## What I learned
Real data is never clean. Code that works on a perfect sample file is not enough. It should also refuse bad input and say clearly where the problem is.