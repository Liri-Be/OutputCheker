# OutputCheker
## Summary
The program grades students' exe files output.  
Each student has a directory with their name, and in it, they have an exe file of their code.  
The program will run each exe file, give it input, and check if the output is correct.  
If the output of the exe file is correct, the program will grade it 100, else 0.  
The program saves the grades in csv file called ```results.csv```. This file will be in the directory of the program.  
The input, the correct output, and the path to the directory with the students' directories will be in the configuration file. That file will be passed to the program as an argument.

## How to run?
There are a few steps -
1. Clone into the project using git bush or terminal with this commend -
    ```
    git clone https://github.com/Liri-Be/OutputCheker.git
    ```
2. The names directory have temp directories and exe files. You may use them, but you will probably want to create your own students directories and exe files.  
    Same for the ```input.txt```, ```output.txt``` and the ```config.txt``` files, they are all temp.  
    If you are creating new ones, just remeber to use the correct format for the ```config.txt``` file -
      ```
      path to students' names directory
      path to input file
      path to output file
      ```
    And remember that all the students' directories have to be in spesific directory which its path will be the first line in the config file.
3. Open the terminal in the code directory.
4. Run this command to create executable file from the C file -
    ```
    gcc -o checker.out checker.c
    ```
    You may use any name you want instead of ```checker.out```.
5. Run the program - 
    ```
    ./cheker.out
    ```

## checker.c
This code is the main code of the program.  
It - 
1. reads the data from the config file.
2. open the students' directory.
3. for each student - 
    - run the exe file of the student and save its output in a temp file.
    - make ```comp.out``` - the executable file from ```comp.c```.
    - run comp.out to check if the output of the student is the same as the correct one
    - add the student's name and grade (according to the output) to the ```results.csv``` file.
   
For each operation that needs to use ```exec()``` or use "new" input/output (for example, taking input from a file), the code creates a child and waits for it to finish.

## comp.c
This code checks if two files are the same or not.  
If they are the same, it exits with code 2 else with 1.
