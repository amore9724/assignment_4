#pragma once
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#ifndef COUNTNAMES_H_
#define COUNTNAMES_H_
#define MSIZE 10001 // Maximum number of lines in the program(empty or non empty)
#define MNAME 101   // Maximum number of names.
#define MLINE 31    // Maximum number of characters in a line.
#define MAXLINE 4096    // This is the maximum amount of lines that will be read.
#define PROGRAMNAME "countnames"    // This is the name of the program to be executed.
#define MAXARGS 100  // Maximum number of arguments
#define output_path "output"

typedef struct {
    char name[31];
    int count;
} NameCountData; // Contains the name and the amount of time it appears in a file.

typedef enum {
    TYPE_NAMECOUNT,
    TYPE_B // This enum may be extended with other types in the future.
} MessageType;

typedef struct {
    MessageType type;
    size_t size; // Size of the following payload
} MessageHeader; // Message to be sent through pipes.

int check_in(char *a, char *b[]); // Checks if a string is in the file.
void nprinter(char *nused[], int count[]); // Prints a string and the amount of times it occurs.
void clnup(char *a1[], char *a2[]); // Frees allocated memory.
void ncount(char *arr[], char *nused[], int count[]); // Counts the number of times a string occurs in a file.
#endif
