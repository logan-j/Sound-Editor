#include "sndfunc.h"

int range(long int *bot, long int *top, long int index, int size);

int parseArg(char *input, long int *bot, long int *top, int index);

main(int argc, char *argv[]){
	int switches[6] = {0};
		int check = cSwitch(argc, argv, switches, 0);
		if(check < 0){
			fprintf(stderr, "\nInvalid Switch Operator: %s\n", argv[-check]);
			return;
		}
		if((switches[4] != 0) && (switches[5] != 0) && (switches[1] != 0) && (switches[2] != 0) && (switches[3] != 0)){
			fprintf(stderr, "\nInvalid switch operator/value for %s use -h for more info.\n", argv[0]);
			return;
		}
		
		/*Help window*/
		if(switches[0] > 0){
			fprintf(stderr, "\n\nDESCRIPTION\n\nsndcut takes a sound file from stdin, removes samples,\
				\nbetween specified ranges and outputs it to stdout. Under default usage:\
				\n\n\t$ sndcut [low..high] [low..high] [low..high] ... < [file]\
				\n\nOPTIONS\n\t-h\tPrint a help message and exit.\n\n");
				return;
		}
		
		int type = fformat(stdin);
		long int info[4] = {-1,-1,-1,-1};
		rewind(stdin);
		char *keyword[4] = {"SampleRate", "BitDepth", "Channels", "Samples"};
		if(type > 0)setCS229Info(keyword, info, stdin);
		else if(type < 0)setAIFFInfo(stdin, info);
		else{
			fprintf(stderr, "\nUnrecognized File Format.\n");
		}
		if(argc > 1){
			int i;
			long int bot[argc - 1];
			long int top[argc - 1];
			for(i = 0; i < argc - 1; i++) {
				char *copy = (char *) malloc(sizeof(argv[i]));
				strcpy(copy, argv[i + 1]);
				parseArg(copy, bot, top, i);
				if(top[i] < bot[i]){
					fprintf(stderr, "\nInvalid range at argument %s\n", argv[i]);
					return;
				}
				if(bot[i] < 1 || top[i] > info[3]){
					fprintf(stderr, "\nInvalid range at argument %s\n", argv[i]);
					return;
				}
			}
			/*calculate new number of samples*/
			for(i = 0; i < argc - 1; i++){
				info[3] += bot[i];
				info[3] -= (top[i] +1);
			}

			int size = argc - 1;	
			
			/*output for CS229*/
			if(type > 0){
				rewind(stdin);
				char line[1000];
				int flip = 0;
				int sample = 1;
				char *skey = "StartData";
				writeCS229Header(stdout, info);
				while(fgets(line, sizeof line, stdin) != NULL){
					if((line[0] != '#') && (strstr(line, skey) != NULL))flip++;
					if((flip > 0) && (strstr(line, skey) == NULL)){
						if(!range(bot, top, sample++, size)){
							fprintf(stdout, line);
						}
					}
					
				}
			}
			
			/*output for AIFF*/
			if(type < 0){
				writeAIFFHeader(stdout, info);
				
				fseek(stdin, 0, SEEK_END);
				int fileSize = ftell(stdin);
				rewind(stdin);
				unsigned char *buffer = (unsigned char*) malloc (fileSize);
				fread(buffer, fileSize, 1, stdin);
				int j;
				unsigned char key[4] = "SSND";
				int index = -1;
				for(j = 0; j < fileSize; j++){
					if(strncmp(key, &buffer[j], 4) == 0){
						index = j + 16;
					}
				}
				int sample = 1;
				while(index < fileSize){
					if(!range(bot, top, sample++, size)){
						fwrite(&buffer[index], (info[2] * info[1] / 8), 1, stdout);
					}
					index += (info[2] * info[1] / 8);
				}
			}
			
			
		}else{
			fseek(stdin, 0, SEEK_END);
				int fileSize = ftell(stdin);
				unsigned char *buffer = (unsigned char*) malloc(fileSize);
				rewind(stdin);
				fread(buffer, fileSize, 1, stdin);
				int i;
				while(i < fileSize){
					fputc(buffer[i], stdout);
					i++;
				}
		}
}

int range(long int *bot, long int *top, long int index, int size){
	int i;
	for(i = 0; i < size; i++){
		if(bot[i] <= index && index <= top[i])return 1;
	}
	return 0;
}

int parseArg(char *input, long int *bot, long int *top, int index){
	int i = 0;
	char *add = strtok(input, "..");
	while(add != NULL){
		if(i == 0)bot[index] = (long int) atoi(add);
		if(i == 1)top[index] = (long int) atoi(add);
		add = strtok(NULL, "..");
		i++;
	}
}

