# SHELL COMMANDS

## 1) prompt.c handles all the display requirements including the neccessary initialisations like setting initial and previous directories

## 2) Built - in - commands are made in their respective files, "echo" doesn't handle multiline strings and prints the string as it is with a single space between words. All the built-in-commands are implemented without using fork , i.e parent shell itself runs these commands

## 3) ls command is present in ls.c and ls.h with necessary flag handling printing files in alphabetical order with color coding as mentioned in assignment pdf

## 4) All the necessary tokenisation symbolising background and foreground processes is done in main.c with forking. Obviously we are waiting for foreground at that time only while the child(background process) itself prints its exit status after execution. Time taken by foreground process is calculated using ``` time() ``` function in time.h

## 5) pinfo command (found in pinfo.c and pinfo.h) extracts information from "/proc/<pid>/status" and "/proc/<pid>/exe" by opening them ( runs only in linux environment ). Foreground or Background process is detected using matching "pgrp" 


## 7) Discover command recursively searches for files using a recursive function (listFilesRecursively) calculating filename along with its basepath from target directory , then after neccessary flag handling , found files are printed

## 8) history.c and history.h contain queue data structure to store 20 (default) history commands  which are also stored in history.txt in main.c , After 20 commands history.txt is rewritten 

## 9) Piping is not fully inplemented , errors out "Inappropriate ioctl for device" 

## 10) There is no sense of putting our written routines in background as they are very short
