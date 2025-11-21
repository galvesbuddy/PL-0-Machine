# PL-0-Machine
Includes a:  
- Scanner
- Parser that also generates assembly
- VM

## Grammar to be followed
<img width="700" height="918" alt="image" src="https://github.com/user-attachments/assets/ebe906d4-469a-42c9-9bf7-be1ee737f7c5" />  

## PM/0 Instruction Set
<img width="622" height="674" alt="image" src="https://github.com/user-attachments/assets/0e41e8b7-1f3b-451f-9c55-af00ced5a091" />
<img width="623" height="693" alt="image" src="https://github.com/user-attachments/assets/fc63eb39-9ba0-48ca-899b-79d0d0224727" />


## Scanner  
### Imporant to Note
Numbers must be no longer than 5 digits, identifiers no longer than 11 characters, and all valid tokens are listed within the Token enum in either the scanner or parser file.  

### How it Works
Takes an input file (there should be an example, something like "scannersample.txt") passed as an argument, converts all tokens read, valid or not, while ignoring comments. Invalid tokens are marked with a 1 (skipsym in the enum). Then the converted tokens are printed on an output file named "token_list.txt" in a single line, 2 (id) is followed by the identifiers name and 3 (number) is followed by its number value.

## Parser and Code Generator
### Important to Note
As soon as an error is detected it will stop immediately and provide the error in both the terminal and on its output file ("elf.txt"). Errors can either be a 1 (skipsym) token from the scanner or more of a parser error like did not follow the grammar rules.

### How it Works
Takes a hardcoded input file generated from the scanner, "token_list.txt" and goes through each token going through if statements to see if it matches the token type to go down that line. When it is done with its task and it matches an 
