# POK Lab work 2, part 2
## Usage the library from part 1 (3 points)

## Main goal
In this part, you receive a randomly-assigned library written by your classmates.<br>
There are two main tasks:
- **Task 1**: Using the library, implement the following program
- **Task 2**: Recommend the point for your college's library (from 1 to 10) + write a feedback

## :skull_and_crossbones: Important remarks
- You can add some small fixes (BUT explain in the feedback why you did it)
- Do not replace the library implementation with yours (or any other)
- Forbidden to add new functions or rename existed
- Forbidden to fix bugs in the library (only if bugs are such that make the library uncompilable)

### :beginner: The team should be the same as in part 1
#### Exception could be in case one of the team members were not involved in the previous part (discuss it with the teacher directly)

## Evaluating of the library should take into account:
- Quality of the package: documentation, description of "how to use", README, CMakeLists.txt
- If the library is done according to the task: functions prototypes, library interface
- Implementation quality: how many fixes have you done, how many PVS, sanitiser's, and Valgrind errors are there if algorithms are not optimised (a.e. n^3 ore even worse)
- Code quality: if the code is easily readable, how variables or additional function's names are suitable, etc.

### :x: Do not communicate with original library authors even if you find them.

## Program (Task 1)
- Read the file **word by word**. The word - sequence of characters, separated by space delimiters (tab, space, newline etc.)
- Store words in a **dynamic array** from your strings. Yes, you should implement an array of dynamic strings in the `array_lib` folder and modify your `CMakeLists.txt`
- The final array size can be found in two ways:
	- read the whole file in memory, count words by reading them with `sscanf()` 
	- dynamically increase its size with the same approach as for strings (+ 0.5 points)
	- :skull_and_crossbones: In case of reading the file more than once maximum point for this task is 0
- Consider that there are no words bigger than 100 characters (just crop if it is bigger). (+0.25 points for concatenation bigger words by hands. Specify in README!)
- Apply one of the following operations to each word
- Save the result in another file, each word in a new line
- Both filenames are command-line arguments (argv)

## Variants:
1. Duplicate each big letter two times, each small letter - three times a.e.
```
AbcD --> AAbbbcccDD
```
2. Replace each of `.,:;!?` punctuations with its English words according to [this](https://grammar.yourdictionary.com/punctuation/what/fourteen-punctuation-marks.html) names, a.e.
```
Her son, John Jones Jr., was born on Dec. 6, 2008! -->
	Her son comma John Jones Jr period comma was born on Dec period 6 comma 2008 exclamation point
```
3. Concatenate each pair of words by adding `:` between them, a.e.
```
before: 
words[1] -- "One"
words[2] -- "Two"
words[3] -- "Three"
words[4] -- "Five"

after:
words[1] -- "One:Two"
words[2] -- "Three:Five"
words[3] -- ""
words[4] -- ""
```
In case the last word is 'alone, just leave it alone =)
4. Replace symbols on its [ASCII codes in HEX](https://en.wikipedia.org/wiki/ASCII), space-separated a.e.
```
Ab; --> 41 62 3B
```
5. Increase each word's size to be the same, as the longest one, by adding a non-stopping alphabet sequence to it (ABC...XYZabc..xyzABC...XYZabc...XYZ) a.e.
```
before:
words[1] -- Hello
words[2] -- where
words[3] -- is
words[4] -- Jayawardenepura

after:
words[1] -- HelloABCDEFGHIJ
words[2] -- whereKLMNOPQRST
words[3] -- isUVWXYZabcdefg
words[4] -- Jayawardenepura
```

## Task number - Your number in the list of negative points modulo 5 plus 1. If tasks in the team are different, but two are the same - do it. If all differ - chose one of them by yourself. 

#### **Specify in the README all modulos**

## Grading
- 0.25 if the project can be compiled (with GCC, clang, msvc compilers), dynamically link `main.c` with both string and array libraries.
- 0.75 for `feedback.md`
- 1 for running the array and the program in main.c (also checked with PVS and Valgrind, other tools)
- 1 from the teacher assistant or teacher for code style, documentation, variable names, project structure and other things that can not be checked automatically
- **0.75 additional points specified above**

## Submission

```
git pull
git checkout -b lab2_task2_dev lab2_task2
```
Here you should make all changes<br>
Program should be in the `src/main.c` file<br>
Array lib in `array_lib` folder<br>
Recommended points and feedback in the `feedback.md` file (see the structure)<br>

:skull_and_crossbones: Wrong or incomplete `README.md` can be a reason of lower grades.<br>

**RECOMMENDED** to do not one, but multiple commits!

When the work is done:

```
git add .
git commit -m "message here."
git push origin lab2_task2_dev

gh pr create -B lab2_task2 -H lab2_task2_dev --title "Completed lab 2 part 2"
```

# Additional tasks
- (up to 3 points) compare given library efficiency with both **yours implementation** and standard library strings from **<string.h>**. For comparison, `google benchmark` is suitable
- (up to 3 points) ONLY IF completed additional task from part 1: write a C++ wrapper for the array, use your wrapper from previous additional task and write your program in `src/maic.cpp` file, c++ language. Compare efficiency with stdlib, `std::vector<std::string>` instance



