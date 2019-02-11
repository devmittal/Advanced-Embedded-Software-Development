/********************************************************************
 *@file: fileio.c
 *@brief: This program executes multiple operations on files.
 *@author: Devansh Mittal
 *@date: 02/10/2019
 *@version: 1.0
**********************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<sys/stat.h>

#define FILE_NAME "fileio.txt"

int main(void)
{
	char *s;
	char c;
	char sread[20]; 
	printf("There are 20,323,379 lines of code in the Linux kernel!");
	FILE* fileptr = fopen(FILE_NAME, "w"); //Create file
	chmod(FILE_NAME, 00400 | 00200); //Modify file permission to be read and write
	fileptr = fopen(FILE_NAME, "w"); //Open file for writing
	fputc('c', fileptr); //write a character to file
	fclose(fileptr); //close file
	fileptr = fopen(FILE_NAME, "a"); //open file in append mode
	if(!(s = malloc(100 * sizeof(char)))) //Dynamically allocate memory
		printf("Memory could not be allocated"); 
	printf("\nEnter string: ");
	scanf("%s",s); //Read string from command line
	fputs(s, fileptr); //Write string to file
	fflush(fileptr); //flush file output
	fclose(fileptr); //Close file
	fileptr = fopen(FILE_NAME, "r"); //open file for reading	
	c = getc(fileptr); //Read a single character
	printf("Character: %c",c);
	printf("\n");
	fgets(sread,20,fileptr); //Read a string of characters
	printf("String: %s",sread); 		 
	printf("\n");	
	fclose(fileptr); //Close file
	free(s); //Free memory
	return 0;
}
