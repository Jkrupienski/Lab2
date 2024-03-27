//parses the input file into Process and event
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <stdbool.h>
#pragma warning(disable : 4996)

struct queue {
	char* request;
};

struct queue queues[3];

struct process {
	char* name;
	char* state;
	bool new;
};

struct process processes[20];

int main()
{
	int i, processCount;
	char* rch;
	char str[200];
	char LineInFile[40][300];
	int lineP, lineQ;
	char* sch;
	char tokenizedLine[10][10];
	int choice = 0;
	float percent = 0;
	int blockedCount;
	
	while (choice < 1) {
		printf("% %%%%%%%Select Percentage of Processes in Blocked State%%%%%%%%\n");
		printf("1: 80%, 2: 90%, 3: 100%\n");
		scanf("%d", &choice);
		printf("\n");

		if (choice == 1) {
			percent = .8;
		}
		else if (choice == 2) {
			percent = .9;
		}
		else if (choice == 3) {
			percent = 1;
		}

	}
	
	
	printf("%d\n", choice);

	FILE* fp1;
	FILE* fp2;
	fp1 = fopen("inp1.txt", "r");			//open the original input file
	fp2 = fopen("inp1_parsed.txt", "w");	//output the Process ID and event to another file. 
											//You can store in variables instead of printing to file

	lineP = 0;
	i = 0;
	processCount = 0;

	queues[0].request = "";
	queues[1].request = "";
	queues[2].request = "";


	printf("Started parsing...\n");

	//copy first line of the original file to the new filefile
	if (fgets(str, sizeof(str), fp1) != NULL) {
		char* endPtr = strstr(str, "end");
		if (endPtr != NULL) {
			*endPtr = '\0';
		}

		char* pch = strtok(str, " ");
		while (pch != NULL && processCount < 20) {
			processes[processCount].name = strdup(pch);
			pch = strtok(NULL, " ");
			processes[processCount].state = strdup(pch);
			pch = strtok(NULL, " ");
			processes[processCount].new = false;
			processCount++;
		}
	}

	for (int i = 0; i < processCount; i++) {
		fprintf(fp2, "%s %s ", processes[i].name, processes[i].state);
	}
	fprintf(fp2, "\n\n");

	//parse each remaining line into Process event
	//while loop with fgets reads each line
	while (fgets(str, sizeof(str), fp1) != NULL)
	{
		lineP = 0;


		rch = strtok(str, ":;.");					// use strtok to break up the line by : or . or ; This would separate each line into the different events
		while (rch != NULL)
		{
			strcpy(LineInFile[lineP], rch);			//copy the events into an array of strings
			lineP++;								//keep track of how many events are in that line
			rch = strtok(NULL, ":;.");				//needed for strtok to continue in the while loop
		}

		fprintf(fp2, "%s: ", LineInFile[0]); // Writes at time X to file
		//printf("%s: \n", LineInFile[0]);	// at time debugging line
		//for each event (e.g. Time slice for P7 expires) pull out process number and event
		for (i = 1; i < lineP - 1; i++)
		{
			lineQ = 0;
			sch = strtok(LineInFile[i], " ");

			while (sch != NULL)
			{
				strcpy(tokenizedLine[lineQ], sch);		//use strtok to break up each line into separate words and put the words in the array of strings
				lineQ++;								//count number of valid elements
				sch = strtok(NULL, " ");
			}

			//tokenizedLine has the event separated by spaces (e.g. Time slice for P7 expires)
			if (strcmp(tokenizedLine[1], "requests") == 0)						//Process requests an I/O device
			{
				for (int j = 0; j < processCount; j++) {
					if (strcmp(processes[j].name, tokenizedLine[0]) == 0) {
						processes[j].state = "Blocked";
						processes[j].new = true;
						break;
					}
				}

				//fprintf(fp2, "%s %s ", tokenizedLine[0], tokenizedLine[1]);
				fprintf(fp2, "%s %s %s ", tokenizedLine[0], tokenizedLine[1], tokenizedLine[3]);

				if (strcmp(tokenizedLine[3], "disk") == 0) {
					//printf("%s\n", tokenizedLine[0]);
					queues[0].request = malloc(strlen(tokenizedLine[0]) + 1);
					strcpy(queues[0].request, tokenizedLine[0]);
					//printf("%s\n", queues[0].request);
				}
				else if (strcmp(tokenizedLine[3], "printer") == 0) {
					//printf("%s\n", tokenizedLine[0]);
					queues[1].request = malloc(strlen(tokenizedLine[0]) + 1);
					strcpy(queues[1].request, tokenizedLine[0]);
					//printf("%s\n", queues[1].request);
				}
				else if (strcmp(tokenizedLine[3], "keyboard") == 0) {
					//printf("%s\n", tokenizedLine[0]);
					queues[2].request = malloc(strlen(tokenizedLine[0]) + 1);
					strcpy(queues[2].request, tokenizedLine[0]);
					//printf("%s\n", queues[2].request);
				}

			}
			else if ((strcmp(tokenizedLine[2], "dispatched") == 0))				//Process is dispatched
			{
				for (int j = 0; j < processCount; j++) {
					if (strcmp(processes[j].name, tokenizedLine[0]) == 0) {
						processes[j].state = "Running";
						processes[j].new = true;
						break;
					}
				}

				fprintf(fp2, "%s %s ", tokenizedLine[0], tokenizedLine[2]);
			}
			else if (strcmp(tokenizedLine[0], "Time") == 0)						//Process has timed off
			{
				for (int j = 0; j < processCount; j++) {
					if (strcmp(processes[j].name, tokenizedLine[3]) == 0) {
						processes[j].state = "Ready";
						processes[j].new = true;
						break;
					}
				}

				fprintf(fp2, "%s %s ", tokenizedLine[3], tokenizedLine[4]);
			}
			else if (strcmp(tokenizedLine[3], "out") == 0)						//Process is swapped out
			{
				for (int j = 0; j < processCount; j++) {
					if (strcmp(processes[j].name, tokenizedLine[0]) == 0) {
						processes[j].state = "Ready";
						processes[j].new = true;
						break;
					}
				}

				fprintf(fp2, "%s %s ", tokenizedLine[0], tokenizedLine[3]);
			}
			else if (strcmp(tokenizedLine[3], "in") == 0)						//Process is swapped in
			{
				for (int j = 0; j < processCount; j++) {
					if (strcmp(processes[j].name, tokenizedLine[0]) == 0) {
						processes[j].state = "Ready";
						processes[j].new = true;
						break;
					}
				}

				fprintf(fp2, "%s %s ", tokenizedLine[0], tokenizedLine[3]);
			}
			else if (strcmp(tokenizedLine[1], "interrupt") == 0)				//An interrupt has occured
			{
				for (int j = 0; j < processCount; j++) {
					if (strcmp(processes[j].name, tokenizedLine[0]) == 0) {
						processes[j].state = "Ready";
						processes[j].new = true;
						break;
					}
				}

				fprintf(fp2, "%s %s ", tokenizedLine[4], tokenizedLine[1]);
			}
			else																//Process has been terminated
			{
				fprintf(fp2, "%s %s ", tokenizedLine[0], tokenizedLine[2]);



			}


		}

		fprintf(fp2, "\n");
		for (int i = 0; i < processCount; i++) {
			fprintf(fp2, "%s %s%s ", processes[i].name, processes[i].state, (processes[i].new ? "*" : ""));
			processes[i].new = false;
		}

		fprintf(fp2, "\n");
		fprintf(fp2, "disk queue: %s\n", queues[0].request);
		fprintf(fp2, "printer queue: %s\n", queues[1].request);
		fprintf(fp2, "keyboard queue: %s\n", queues[2].request);
		fprintf(fp2, "\n");

	}

	printf("Parsing complete\n\n");

	fclose(fp1);
	fclose(fp2);

	return 0;
}