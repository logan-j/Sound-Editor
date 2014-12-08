#include "sndfunc.h"

void printLine(int width, int value, int max, int channels, int index);

main(int argc, char *argv[]){
		int switches[6] = {0};
		int check = cSwitch(argc, argv, switches, 1);
		if(check < 0){
			fprintf(stderr, "\nInvalid Switch Operator: %s\n", argv[-check]);
			return;
		}
		
		/*Help window*/
		if(switches[0] > 0){
			fprintf(stderr, "\n\nDESCRIPTION\n\nsndshow takes a sound file and displays an ASCII art representation of the information.\
				\nFor default usage, use the following format:\
				\n\n\t$ sndshow -z 1 -c 0 -w 80 < [file]\
				\n\nOPTIONS\n\t-h\tPrint a help message and exit.\
				\n\n\t-z #\tSet the zoom factor, where output is maximum value of # samples. 1 by default.\
				\n\n\t-w #\tSet the width of the output in # of characters. Min of 20, set to 80 by default.\
				\n\n\t-c #\tSet a specific channel to view in the output. # must be between 0 and number of channels.\n\t\t0 displays all channels\n");
				return;
		}
		
		if(switches[1] != 0){
			fprintf(stderr, "\nInvalid Switch Operator: -l\n");
			return;
		}
		if(switches[4] == 0)switches[4] = 80;
		if(switches[5] == 0)switches[5] = 1;
		long int info[4] = {-1, -1, -1, -1};
		char *keyword[4] = {"SampleRate", "BitDepth", "Channels", "Samples"};
		int i, j, k;
		
		int type = fformat(stdin);
		 /*cs229*/
		 if(type > 0){
			rewind(stdin);
			setCS229Info(keyword, info, stdin);
			if((switches[3] > info[2]) || (switches[3] < 0)){
				fprintf(stderr, "Invalid Channel Switch at %d. Must be between 0 and %d", switches[3], info[2]);
				return;
			}
			for(i = 0; i < 4; i++){
				if(info[i] < 0){
					fprintf(stderr, "Missing Relevant File Info: %s", keyword[i]);
					return;
				}
			}
			
			int maxVal = (pow(2,(info[1] - 1)));
			char line[1000];
			int flip = 0;
			rewind(stdin);
			char *skey = "StartData";
			int sample[info[2]];
			j = 0;
			i = 0;
			while(fgets(line, sizeof line, stdin) != NULL){
				if((line[0] != '#') && (strstr(line, skey) != NULL))flip++;
				if((flip > 0) && (strstr(line, skey) == NULL)){
					int comp[info[2]];
					parse32(line, comp);
					if(j == 0){
						for(k = 0; k < info[2]; k++)sample[k] = comp[k];
					}else{
						for(k = 0; k < info[2]; k++){
							if(abs(sample[k]) < abs(comp[k])){
								sample[k] = comp[k];
							}
						}	
					}
					j++;
					i++;
					j = j % switches[5];
					if(j == 0 || i == (info[3] - 1)){
						int align = switches[5];
						if(i == (info[3] - 1)) align = i % switches[5];
						for(k = 0; k < info[2]; k++){
							if(switches[3] == 0){
								printLine(switches[4], sample[k], maxVal, info[2], (((i - align)*info[2]) + k));
							}else{
								if(k == (switches[3] - 1)){
									printLine(switches[4], sample[k], maxVal, info[2], ((i - align)*info[2]));
								}
							}
						}
					}
				}
				
			}
		 }
		 
		 /*aiff*/
		 else if(type < 0){
			rewind(stdin);
			setAIFFInfo(stdin, info);
			if((switches[3] > info[2]) || (switches[3] < 0)){
				fprintf(stderr, "Invalid Channel Switch at %d. Must be between 0 and %d", switches[3], info[2]);
				return;
			}
			for(i = 0; i < 4; i++){
				if(info[i] < 0){
					fprintf(stderr, "Missing Relevant File Info: %s", keyword[i]);
					return;
				}
			}
			/*get sound data*/
			fseek(stdin, 0, SEEK_END);
			int fileSize = ftell(stdin);
			rewind(stdin);
			unsigned char *buffer = (unsigned char*) malloc (fileSize);
			fread(buffer, fileSize, 1, stdin);
			unsigned char key[4] = "SSND";
			int index = -1;
			for(j = 0; j < fileSize; j++){
				if(strncmp(key, &buffer[j], 4) == 0){
					index = j + 16;
				}
			}
			int maxVal = (pow(2,(info[1] - 1)));
			switch(info[1]){
				case 8:
					i = 0;
					while(i < info[3]){
						int place = i;
						char sample[info[2]];
						for(k = 0; k < info[2]; k++){
							sample[k] = 0;
						}
						for(j = 0; j < switches[5]; j++){
							for(k = 0; k < info[2]; k++){
								if(index < fileSize){
									char input = (char) buffer[index];
									if(abs(sample[k]) < abs(input)){
										sample[k] = input;
									}
									index++;
								}
								
							}							
							i++;
						}
						for(j = 0; j < info[2]; j++){
							if(switches[3]==0){
								printLine(switches[4], sample[j], maxVal, info[2], ((place*info[2]) + j));
							}else{
								if(j == (switches[3] - 1)){
									printLine(switches[4], sample[j], maxVal, info[2], place*info[2]);
								}
							}
						}
					}
					break;
					
				case 16:
					i = 0;
					while(i < info[3]){
						int place = i;
						short sample[info[2]];
						for(k = 0; k < info[2]; k++)sample[k] = 0;
						for(j = 0; j < switches[5]; j++){
							for(k = 0; k < info[2]; k++){
								if(index < fileSize){
									short input = Byte2Short(&buffer[index]);
									if(abs(sample[k]) < abs(input)){
										sample[k] = input;
									}
									index+=2;
								}
							}							
							i++;
						}
						for(j = 0; j < info[2]; j++){
							if(switches[3]==0){
								printLine(switches[4], sample[j], maxVal, info[2], place*info[2] + j);
							}else{
								if(j == (switches[3] - 1)){
									printLine(switches[4], sample[j], maxVal, info[2], place*info[2]);
								}
							}
						}
					}
					break;
				case 32:
					i = 0;
					
					while(i < info[3]){
						int place = i;
						int sample[info[2]];
						for(k = 0; k < info[2]; k++)sample[k] = 0;
						for(j = 0; j < switches[5]; j++){
							for(k = 0; k < info[2]; k++){
								if(index < fileSize){
									int input = Byte2Int(&buffer[index]);
									if(abs(sample[k]) < abs(input)){
										sample[k] = input;
									}
									index+=4;
								}
							}							
							i++;
						}
						for(j = 0; j < info[2]; j++){
							if(switches[3]==0){
								printLine(switches[4], sample[j], maxVal, info[2], place*info[2] + j);
							}else{
								if(j == (switches[3] - 1)){
									printLine(switches[4], sample[j], maxVal, info[2], place*info[2]);
								}
							}
						}
					}
					break;
				default:
					fprintf(stderr, "\nInvalid Bit Depth = %d. Only 8/16/32 Supported.\n", info[1]);
					exit(0);
			}
		 }
		 
		 else{
			fprintf(stderr, "File Format Not Supported");
			return;
		 }
}

void printLine(int width, int value, int max, int channels, int index){	
	int pos;
	char out[9];
	if(index % channels == 0){
		pos = index / channels;
		sprintf(out, "%d", pos);
	}else{
		out[0] = '\0';
	}
	int i = 0;
	while(i < 9){
		if(i < (9 - strlen(out))){
			printf(" ");
			i++;
		}
		else {
			printf("%s", out);
			i += strlen(out);
		}
	}
	printf("|");
	int half = (width - 12)/2;
	
	if(value < 0){
		double size = (double)(value * half) / (max + 1);
		size = round(size);
		for(i = 0; i < half; i++){
			if(i < size + half)printf(" ");
			else printf("-");
		}
		printf("|");
		for(i = 0; i < half; i++)printf(" ");
	}
	else if(value > 0){
		for(i = 0; i < half; i++)printf(" ");
		printf("|");
		double size = (double)(value * half) / max;
		size = round(size);
		for(i = 0; i < half; i++){
			if(i < size)printf("-");
			else printf(" ");
		}
	}else{
		for(i = 0; i < half; i++)printf(" ");
		printf("|");
		for(i = 0; i < half; i++)printf(" ");
	}
	printf("|\n");
}