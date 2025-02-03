#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main()
{
	char delim[] = " \t\n";
	char **tokens;
	char input[256];
	int numtokens;
	int run = 1;
	
	while(run) {
		printf("mysh >");
		fgets(input, 256, stdin);

		numtokens = tokenize(input, delim, &tokens);
		if (numtokens <= 0) {
			continue;
		}
		
		if (strcmp(tokens[0], "exit") == 0) {
			free(tokens);
			break;
		}
		pid_t pid = fork();
		if (pid < 0) {
			printf("Fork failed\n");
			free(tokens);
			continue;
		} else if (pid == 0) {
			execvp(tokens[0], tokens);
		} else {
			wait(NULL);
		}
		free(tokens);
	}
	return 0;
}

int tokenize(char *string, char *delimiters, char ***arrayOfTokens)
{

	char *token;
	int numtokens;
	int i;
	
	/* skip the beginning delimiters */
	string += strspn(string, delimiters);
	if ((token = malloc(strlen(string) + 1)) == NULL)
		return -1;
	
	/* count tokens */
	strcpy(token, string);
	numtokens = 0;
	if (strtok(token, delimiters) != NULL)
		for (numtokens = 1; strtok(NULL, delimiters) != NULL;
	numtokens++);
	
	/* create array of pointers to tokens */
	if ((*arrayOfTokens = malloc((numtokens+1)*sizeof(char *))) == NULL) {
		free(token);
		return -1;
	}
	
	/* fill pointers to tokens into the array */
	if (numtokens == 0)
		free(token);
	else {
		strcpy(token, string);
		(*arrayOfTokens)[0] = strtok(token, delimiters);
		for (i = 1; i < numtokens; i++)
			(*arrayOfTokens)[i] = strtok(NULL, delimiters);
		(*arrayOfTokens) [numtokens] = NULL;
	}
	
	return numtokens;
}
