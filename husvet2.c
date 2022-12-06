#define DATA "jelentkezok.txt"

#define _POSIX_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>


typedef struct
{
    int id;
    char name[50];
    char addr[100];
    char apply[10];
	int competition;
}jelentkezo;

const char* place[] = { "Baratfa", "Lovas", "Szula", "Kigyos-patak", "Malom telek" , "Paskom" , "Kaposztas kert"};

void readFile(const char* fn, jelentkezo* db)
{
    FILE* fp = fopen(fn, "r");
    char buff[1023];
    int i = 0;
    while (fgets(buff, 1023, fp) != NULL)
    {
        db[i].id = atoi(strtok(buff, ";"));
        strcpy(db[i].name, strtok(NULL, ";"));
        strcpy(db[i].addr, strtok(NULL, ";"));
        strcpy(db[i].apply, strtok(NULL, ";"));
        
        i++;
    }
    fclose(fp);
}
void areagroup(jelentkezo* db, int nOA, int placeID)
{
    printf("Ezen a teruleten:\n");
    for (int i = 0; i < nOA; i++)
    {
        if (strcmp(db[i].addr, place[placeID]) == 0)
        {
            printf("%d | %s | %s | %s ", db[i].id, db[i].name, db[i].addr, db[i].apply);
            printf("\n");
        }
    }
}

int lineCount(const char* fn)
{
    FILE* fp = fopen(fn, "r");
    char buff[1023];
    int N = 0;
    while (fgets(buff, 1023, fp) != NULL)
    {
        N++;
    }
    fclose(fp);
    return N;
}



void listByArea(char **place, int length, FILE *fnew)
{
    FILE *fp = fopen("jelentkezok.txt", "r");

    char line[100];
    jelentkezo a;

    while (fgets(line, 100, fp) != NULL)
    {
        sscanf(line, "%*d;%[^;];%[^;];%[^\n]", a.name, a.addr, a.apply);
        for (int i = 0; i < length; i++)
        {
            if (strcmp(place[i], a.addr) == 0)
            {
                fprintf(fnew, "%s\n", a.name);
                break;
            }
        }
    }
}

void Simulation()
{
    srand(getpid());

    FILE* f = fopen("jelentkezok.txt", "r");
    FILE* f1 = fopen("first.txt", "w");
    FILE* f2 = fopen("second.txt", "w");

    if (f == NULL || f1 == NULL || f2 == NULL)
    {
        printf("Error reading file \n");
        exit(1);
    }

    int pipefd1[2];
    int pipefd2[2];
    int pipefd3[2];
    int pipefd4[2];
    pid_t child1;
	pid_t child2;

    if (pipe(pipefd1) == -1)
    {
        perror("ERROR!");
        exit(EXIT_FAILURE);
    }

    if (pipe(pipefd2) == -1)
    {
        perror("ERROR!");
        exit(EXIT_FAILURE);
    }

    if (pipe(pipefd3) == -1)
    {
        perror("ERROR!");
        exit(EXIT_FAILURE);
    }

    if (pipe(pipefd4) == -1)
    {
        perror("ERROR!");
        exit(EXIT_FAILURE);
    }

    child1 = fork();
    if (child1 < 0)
    {
        perror("fork ERROR!");
        exit(EXIT_FAILURE);
    }
    else if (child1 == 0)
    { 
        // CHILD1 -> PARENT 
        //---------------------------------------
        printf("------------------------------Child1 started----------\n");
        char line[100000];
        read(pipefd1[0], line, sizeof(line));
        printf("child1 read the message:\n%s\n", line);
        printf("\n");

        char writeBuf[20];
        FILE* fp = fopen("first.txt", "r");
        FILE* out = fopen("out.txt", "w");
        int rnum1;
		srand(time(NULL));
		
        while (fgets(writeBuf, 100, fp) != NULL)
        {
            rnum1 = rand() % 100;
            fprintf(out, "%d\n", rnum1);
        }
        fclose(out);
        fclose(fp);
        char source1[100000];
        FILE* fp2 = fopen("out.txt", "r");
        if (fp2 != NULL)
        {
            size_t newLen = fread(source1, sizeof(char), 100000, fp2);
            if (ferror(fp2) != 0)
            {
                fputs("Error reading file\n", stderr);
            }
            else
            {
                source1[newLen++] = '\0';
            }

            fclose(fp2);
        }
        write(pipefd2[1], source1, sizeof(source1));
        printf("child1 sends:\n%sback to the parent...\n", source1);

        printf("------------------------------Child1 ended----------\n");
    }
    else
    { 
        // parent
        //-----------------------------------
		sleep(1);
        child2 = fork();
        if (child2 < 0)
        {
            perror("fork ERROR2!");
            exit(EXIT_FAILURE);
        }
        else if (child2 == 0)
        { 
            // CHILD2->PARENT 
            // -------------------------------------------
            printf("------------------------------Child2 started----------\n");
            char line[100000];
            read(pipefd3[0], line, sizeof(line));
            printf("child2 read the message:\n%s\n", line);
            printf("\n");

            char writeBuf[20];
            FILE* fp = fopen("second.txt", "r");
            FILE* out = fopen("out2.txt", "w");
			int rnum2;
			sleep(1);
			srand(time(NULL));

            while (fgets(writeBuf, 100, fp) != NULL)
            {
                rnum2 = rand() % 100;
                fprintf(out, "%d\n", rnum2);
            }
            fclose(out);
            fclose(fp);


            char source2[100000];
            FILE* fp2 = fopen("out2.txt", "r");
            if (fp2 != NULL)
            {

                size_t newLen = fread(source2, sizeof(char), 100000, fp2);
                if (ferror(fp2) != 0)
                {
                    fputs("Error reading file\n", stderr);
                }
                else
                {
                    source2[newLen++] = '\0';
                }

                fclose(fp2);
            }

            write(pipefd4[1], source2, sizeof(source2));

            printf("child2 sends\n%s\nback to the parent...\n", source2);

            printf("------------------------------Child2 ended----------\n");
        }
        else
            //PARENT -> CHILD1-nek adatküldés
        {
            char* Area1[4] = { "Baratfa", "Lovas", "Szula", "Kigyos-patak" };
            char* Area2[3] = { "Malom telek", "Paskom", "Kaposztas kert" };

            listByArea(Area1, 4, f1);
            listByArea(Area2, 3, f2);
            fclose(f1);
            fclose(f2);
        

            jelentkezo winner;
            strcpy(winner.name, "Noone");
            winner.competition = 0;

            char source[100000];
            FILE* fp1 = fopen("first.txt", "r");

            if (fp1 != NULL)
            {

                size_t newLen = fread(source, sizeof(char), 100000, fp1);
                if (ferror(fp1) != 0)
                {
                    fputs("Error reading file", stderr);
                }
                else
                {
                    source[newLen++] = '\0';
                }

                fclose(fp1);
            }

            write(pipefd1[1], source, sizeof(source));
            printf("%ssent to child1\n", source);
            // ---------------------------------------------------------------------------
            //CHILD1->PARENT visszaolvas
            printf("---------------------\n");
            read(pipefd2[0], source, sizeof(source));
            printf("parent read back from child1:\n%s\n", source);
            printf("-----------------------------------------------------\n");
            //MAXIMUM SZÁMOLÁS
            int numOflines = lineCount("first.txt");
            int nums[numOflines];
            int cnt = 0;
            int maxNum = -1;
            int maxInd = 0;


            char* p, * temp;
            p = strtok_r(source, "\n", &temp);
            do
            {
                nums[cnt] = atoi(p);
                if (nums[cnt] > maxNum)
                {
                    maxNum = nums[cnt];
                    maxInd = cnt;
                }

                cnt++;
            } while ((p = strtok_r(NULL, "\n", &temp)) != NULL);

            FILE* fp = fopen("first.txt", "r");
            cnt = 0;
            while (fgets(source, 20, fp) != NULL)
            {

                if (cnt == maxInd)
                {
                    strcpy(winner.name, source);
                    winner.competition = maxNum;
                }
                cnt++;
            }
            fclose(fp);

            // -------------------------------------------------------------------------------
            //PARENT -> CHILD2 adatküldés
            char source2[100000];
            FILE* fp2 = fopen("second.txt", "r");
            if (fp2 != NULL)
            {

                size_t newLen = fread(source2, sizeof(char), 100000, fp2);
                if (ferror(fp2) != 0)
                {
                    fputs("Error reading file\n", stderr);
                }
                else
                {
                    source2[newLen++] = '\0';
                }

                fclose(fp2);
            }

            write(pipefd3[1], source2, sizeof(source2));
            printf("%ssent to child2\n", source2);
            // ---------------------------------------------------------------------------
            //CHILD2-> PARENT
            printf("---------------------\n");
            char line2[100000];
            read(pipefd4[0], line2, sizeof(line2));
            printf("parent read back from child2:\n%s\n", line2);
            printf("\n");

            // ---------------------------------------------------------------
            //GYŐZTES(maximum)
            int numOflines2 = lineCount("second.txt");
            int nums2[numOflines2];
            cnt = 0;
            int maxNum2 = -1;
            int maxInd2 = 0;
            bool isUpdated = false;

            char* p2, * temp2;
            p2 = strtok_r(line2, "\n", &temp2);
            do
            {
                nums2[cnt] = atoi(p2);
                if (nums2[cnt] > maxNum)
                {
                    isUpdated = true;
                    maxNum2 = nums2[cnt];
                    maxInd2 = cnt;
                }

                cnt++;
            } while ((p2 = strtok_r(NULL, "\n", &temp2)) != NULL);

            if (isUpdated)
            {
                FILE* fp = fopen("second.txt", "r");
                cnt = 0;
                while (fgets(source, 20, fp) != NULL)
                {

                    if (cnt == maxInd2)
                    {
                        strcpy(winner.name, source);
                        winner.competition = maxNum2;
                    }
                    cnt++;
                }
                // fclose(fp);
            }

            printf("the winner is %s\n", winner.name);
        }
    }

	close(pipefd1[0]);
	close(pipefd2[1]);
	close(pipefd3[0]);
	close(pipefd4[1]);
	close(pipefd1[1]);
	close(pipefd2[0]);
	close(pipefd3[1]);
	close(pipefd4[0]);
     //fclose(f);
     //fclose(f2);
     //fclose(f1);
}


int main()
{
    const char* filename = DATA;
    
    int numOfApplicant = lineCount(filename);
    if (numOfApplicant == 0)
    {
        printf("HIBA: ures adatfajl!");
        return 1;
    }
    jelentkezo* db = malloc(numOfApplicant * sizeof(jelentkezo));
    readFile(filename, db);

    char option;
    while (option > 7 || option < 1)
    {
		printf("Husvet program - menu\n");
		printf("1 - uj jelentkezo hozzaadasa\n");
		printf("2 - egy jelentkezo szerkesztese\n");
		printf("3 - egy jelentkezo eltavolitasa\n");
		printf("4 - jelentkezok listazasa\n");
		printf("5 - teruleti jelentkezesek\n");
		printf("6 - locsolás szimuláció\n");
		printf("7 - kilepes\n");
		printf("_ - barmilyen mas opcio eseten a menu ujra kiirasra kerul.\n");
		
        printf(": ");
        scanf(" %d", &option);

		switch (option)
		{
		case 1:
		{
			jelentkezo new;
			new.id = numOfApplicant + 1;
			printf("Add meg az uj jelentkezo nevet [50]\n:: ");
			fgets(new.name, 50, stdin);
			fgets(new.name, 50, stdin);
			strtok(new.name, "\n");
			char tempAddr[100];
			int validPlace = 0;
			do
			{
				printf("Add meg az uj jelentkezo teruletet [100]\n:: ");
				fgets(tempAddr, 100, stdin);
				strtok(tempAddr, "\n");
				for(int i = 0; i < 7; i++)
				{
					if(strcmp(tempAddr, place[i]) == 0)
					{
						strcpy(new.addr, tempAddr);
						validPlace = 1;
					}
				}
			} while (validPlace == 0);

			printf("Add meg az uj jelentkezo hanyadjara jelentkezik\n:: ");
			fgets(new.apply, 10, stdin);
			strtok(new.apply, "\n");
		 
			numOfApplicant += 1;

			db = realloc(db, numOfApplicant * sizeof(jelentkezo));
			db[numOfApplicant - 1] = new;
			break;
		}
		case 2:
		{
			int idToModify;
			printf("Add meg id szerint, hogy melyik jelentkezot szeretned szerkeszteni\n:: ");
			scanf("%d", &idToModify);
			printf("\nMelyik adatat szeretned %s-nak szerkeszteni?\nn - nev\nc - cim\nm - hanyadszor\n:: ", db[idToModify - 1].name);
			char chosenPlaceID;
			scanf(" %c", &chosenPlaceID);
			if (chosenPlaceID == 'n')
			{
				printf("Add meg az uj jelentkezo nevet [50]\n:: ");
				fgets(db[idToModify - 1].name, 50, stdin);
				fgets(db[idToModify - 1].name, 50, stdin);
				strtok(db[idToModify - 1].name, "\n");
			}
			else if (chosenPlaceID == 'c')
			{
				char tempAddr[100];
				int validPlace = 0;
				fgets(tempAddr, 100, stdin);
				do
				{
					printf("Add meg az uj jelentkezo terulete [100]\n:: ");
					fgets(tempAddr, 100, stdin);
					strtok(tempAddr, "\n");
					for(int i = 0; i < 7; i++)
					{
						if(strcmp(tempAddr, place[i]) == 0)
						{
							strcpy(db[idToModify - 1].addr, tempAddr);
							validPlace = 1;
						}
					}
				} while (validPlace == 0);
			}
			else if (chosenPlaceID = 'm')
			{
				printf("Add meg az uj jelentkezo hanyadjara jelentkezik [100]\n:: ");
				fgets(db[idToModify - 1].apply, 10, stdin);
				fgets(db[idToModify - 1].apply, 10, stdin);
				strtok(db[idToModify - 1].apply, "\n");
			}
			else
			{
				printf("\nNem elerheto opciot valasztottal, nem tortent adatmodositas\n");
			}
			break;
		}
		case 3:
		{
			int idToDelete;
			printf("Add meg id szerint, hogy melyik jelentkezot szeretned torolni\n:: ");
			scanf("%d", &idToDelete);
			idToDelete -= 1;
			for (int i = idToDelete; i < numOfApplicant - 1; i++)
			{
				db[i] = db[i + 1];
				db[i].id = i + 1;
			}
			numOfApplicant -= 1;
			db = realloc(db, numOfApplicant * sizeof(jelentkezo));
			break;
		}
		case 4:
		{
			printf("id|  nev  |    cim   | hanyadjára\n");
			for (int i = 0; i < numOfApplicant; i++)
			{
				printf("%d | %s | %s | %s", db[i].id, db[i].name, db[i].addr, db[i].apply);
				printf("\n");
			}
			break;
		}
		case 5:
		{
			int listChosenPlaceID;
			printf("Melyik terulet jelentkezoi erdekelnek?\n"
				"0 - Barátfa\n"
				"1 - Lovas\n"
				"2 - Szula\n"
				"3 - Kígyós-patak\n"
				"4 - Malom telek\n"
				"5 - Páskom\n"
				"6 - Káposztás kert\n");
			printf(":: ");
			scanf("%d", &listChosenPlaceID);
			areagroup(db, numOfApplicant, listChosenPlaceID);
			break;
		}
		case 6:
		{
			Simulation();
			break;
		}
		case 7:
		{
			exit(0);
			break;
		}
		}
	}

    FILE* fp = fopen(DATA , "w+");

    for (int i = 0; i < numOfApplicant; i++)
    {
        char string[1023];
        char id[2];
        sprintf(id, "%d", db[i].id);
        strcpy(string, id);
        strcat(string, ";");
        strcat(string, db[i].name);
        strcat(string, ";");
        strcat(string, db[i].addr);
        strcat(string, ";");
        strcat(string, db[i].apply);
        strcat(string, ";\n");
        fputs(string, fp);
    }
    fclose(fp);
	fflush(stdin);
    return 0;
}