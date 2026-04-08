# Names

Arya More, Michael Chen

# Compilation

First do
```cc countnames.c helper_functions.c -o countnames -Wall -Werror``` for countnames

Then do ```cc shell1.c helper_functions.c -o shell1 -Wall -Werror``` for shell1.

# Running the program

First run ./shell1. (WARNING: MAKE SURE TO USE CHMOD +X OR THE PROGRAM WILL NOT RUN.)
If you want to test countnames, copy and paste this to the terminal to test every text file in test.
`./countnames test/names.txt test/names1.txt test/names2.txt test/names_long.txt test/names_long_redundant.txt test/names_long_redundant1.txt test/names_long_redundant2.txt test/names_long_redundant3.txt test/namesB.txt test/custom_testcase_1.txt test/custom_testcase_2.txt test/custom_testcase_3.txt`
# Lessons Learned:

Learned how to use "sprintf" to print a string into an allocated block of memory.

Learned not to declare variables in a header.

Learned that memory mapped files can be accessed like normal memory.

Learned more about how memory actually works.

Learned how to properly clean up memory and handle signals.

# Acknowledgments

https://stackoverflow.com/questions/190229/where-is-the-itoa-function-in-linux
https://stackoverflow.com/questions/7757234/how-do-i-understand-this-compiler-error-multiple-definition-of