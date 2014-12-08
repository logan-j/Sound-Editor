/*
 * sndinfo.c
 * 
 * Author: Logan Jewett
 *
 */

#include "sndfunc.h"

	void getInfo(char *input);

	void main(int argc, char *argv[]){
		int switches[6] = {0};
		int check = cSwitch(argc, argv, switches, 0);
		if(check < 0){
			fprintf(stderr, "\nInvalid Switch Operator: %s\n", argv[-check]);
			return;
		}
		if((switches[2] != 0) && (switches[3] != 0) && (switches[4] != 0) && (switches[5] != 0)){
			fprintf(stderr, "\nInvalid switch operator/value for %s use -h for more info.\n", argv[0]);
			return;
		}
		
		/*Help window*/
		if(switches[0] > 0){
			fprintf(stderr, "\n\nDESCRIPTION\n\nsndinfo takes sound file(s) as arguments and displays their respective information regarding\
				\nnumber of channels, duration, samples, sound format, and bit depth. The default usage is:\
				\n\n\t$ sndinfo [file] [file] ...\
				\n\nOPTIONS\n\t-h\tPrint a help message and exit.\
				\n\n\t-l\tDisplays a prompt and takes the following as input and outputs information about the file.\n\n");
				return;
		}
		
		/*Part 1 mode*/
		if(switches[1] > 0){
			printf("\nEnter the (path)name of a sound file:\n\n");
			
			fflush(stdout);
			FILE *f;
			char input[100];
			scanf("%99s", input);
			int j = 0;
			while(j++ < 60)printf("-");
			getInfo(input);
			return;
		}
		
		/*Part 2 mode*/
		else{
			int j = 0;
			int i = 0;
			if(argc < 2){
				fprintf(stderr, "\nPlease Input Arguments. Use -h for more information.\n");
				return;
			}
			printf("\n");
			while(i++ < 60)printf("-");
			printf("\n");
			fflush(stdout);

			for(j = 1; j < argc; j++){
				if(nSwitch(argv[j])){
					i = 0;
					getInfo(argv[j]);
					printf("\n");
					while(i++ < 60)printf("-");
					printf("\n");
					fflush(stdout);
				}
			}
			return;
		}
	}
	
	/*Reads info from input file*/
	void getInfo(char *fileName){
		FILE *f = fopen(fileName, "r");
		
		if(f == NULL){
			fprintf(stderr, "\nFile Not Found: %s\n", fileName);
			return;
		}
		
		char *format;
		long int info[4] = {-1,-1,-1,-1};
		float duration[3] = {0};
		int type = fformat(f);
		char *keyword[4] = {"SampleRate", "BitDepth", "Channels", "Samples"};
		if(type > 0){
			format = "CS229";
			
			char line[1000];
			setCS229Info(keyword, info, f);
			
			
			int samples = calSam(f);
			if(info[3] < 0){
				info[3] = samples;
			}
			
			if(info[3] != samples){
				fprintf(stderr, "\nNumber of Samples Does Not Match!\n");
				return;
			}else{
				calDur(info, duration);
			}
			
		}else if(type < 0){
			format = "AIFF";
			freopen(fileName, "rb", f);
			int chunk = setAIFFInfo(f, info);
			if(chunk < 0){
				fprintf(stderr, "\nMissing Chunk: %s", fileName);
				return;
			}
			calDur(info, duration);
			
		}else{
			printf("\nUnrecognized File Format.\n");
			return;
		}
			if(!((info[1] == 16) || (info[1] == 8) || (info[1] ==32))){
				fprintf(stderr, "\nInvalid Bit Depth = %d in %s\nOnly 8/16/32 is supported", info[1], fileName);
				return;
			}
			
			if(info[0] <= 0 || info[1] < 0 || info[2] < 0){
				fprintf(stderr, "\nFile %s Missing Relevant Info:\n", fileName);
				int i = 0;
				for(i; i < 3; i++){
					if(info[i] < 0){
						fprintf(stderr, "%s\n", keyword[i]);
					}
				}
				return;
			}
			
			int i = 0;
			printf("\n   Filename  %s\n", fileName);
			printf("     Format  %s\n", format);
			printf("Sample Rate  %d\n", info[0]);
			printf("  Bit Depth  %d\n", info[1]);
			printf("   Channels  %d\n", info[2]);
			printf("    Samples  %d\n", info[3]);
			printf("   Duration  %.0f : %.0f : %.2f\n", duration[0], duration[1], duration[2]);
			return;
		
		fclose(f);
	}