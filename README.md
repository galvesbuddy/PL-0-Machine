# PL-0-Machine
Includes a:  
- Scanner
- Parser that also generates assembly
- VM

## Grammar to be followed
<img width="700" height="918" alt="image" src="https://github.com/user-attachments/assets/ebe906d4-469a-42c9-9bf7-be1ee737f7c5" />  

## Scanner  
### Imporant to Note
Numbers must be no longer than 5 digits, identifiers no longer than 11 characters, and all valid tokens are listed within the Token enum in either the scanner or parser file.  

### How it Works
Takes an input file (there should be an example, something like "scannersample.txt"), converts all tokens read, valid or not, while ignoring comments. Invalid tokens are marked with a 1 (skipsym in the enum). Then the converted tokens are printed on an output file named "token_list.txt" in a single line, 2 (id) is followed by the identifiers name and 3 (number) is followed by its number value.

## Parser and Code Generator
