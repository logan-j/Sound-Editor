#include "sndfunc.h"

main(int argc, char *argv[]){
		int switches[6] = {0};
		int check = cSwitch(argc, argv, switches, 0);
		if(check < 0){
			fprintf(stderr, "\nInvalid Switch Operator: %s\n", argv[-check]);
			return;
		}
		if((switches[4] != 0) && (switches[5] != 0) && (switches[1] != 0)){
			fprintf(stderr, "\nInvalid switch operator/value for %s use -h for more info.\n", argv[0]);
			return;
		}
		
		/*Help window*/
		if(switches[0] > 0){
			fprintf(stderr, "\n\nDESCRIPTION\n\nsndcat takes sound files and concatenates them into a single sound file of specified output.\
				\nFor default usage, use the following format:\
				\n\n\t$ sndcat [switch] [file] [file] > [file]\
				\n\nOPTIONS\n\t-h\tPrint a help message and exit.\
				\n\n\t-a\tOutput is in AIFF format.\
				\n\n\t-c\tOutput is in CS229 format.\n\n");
				return;
		}
		
		if((switches[2] + switches[3]) == 0 || (switches[2] + switches[3]) == 2){
			fprintf(stderr, "\nUse -a or -c to specify which file format to output to.\n");
			return;
		}
		
		long int indinfo[4] = {-1, -1. -1, -1};
		long int info[4] = {-1, -1, -1, -1};
		int i, j, k;
		int *sound = NULL;
		for(i = 1; i < argc; i++){
			if(nSwitch(argv[i])){
				FILE* f = fopen(argv[i], "rb");
	
				if(f == NULL){
					fprintf(stderr, "File %s Not Found!", argv[i]);
					return;
				}
				
				int type = fformat(f);
				rewind(f);
				/*input file is cs229*/
				if(type > 0){
					char *keyword[4] = {"SampleRate", "BitDepth", "Channels", "Samples"};
					setCS229Info(keyword, indinfo, f);
					
					
					if((indinfo[0] < 0) || (indinfo[1] < 0) || (indinfo[2] < 0) || (indinfo[3] < 0)){
						fprintf(stderr, "File %s Missing Relevant Information!", argv[i]);
						return;
					}
					if(info[0] < 0){
						for(k = 0; k < 4; k++)info[k] = indinfo[k];
					}else{
						info[3] += indinfo[3];
					}
					for(k = 0; k < 3; k++){
						if(info[k] != indinfo[k]){
							fprintf(stderr, "File Encoding For %s Does Not Match!", argv[i]);
							return;
						}
					}
					sound = (int *) realloc (sound, info[3] * info[2] * 4);
					int arrIndex = (info[3] - indinfo[3]) * info[2];
					
					
					int i = 0;
					rewind(f);
					char line[1000];
					int flip = 0;
					char *skey = "StartData";
					int output[info[2]];
					while(fgets(line, sizeof line, f) != NULL){
						if((line[0] != '#') && (strstr(line, skey) != NULL))flip++;
						if((flip > 0) && (strstr(line, skey) == NULL)){
							parse32(line, output);
							for(k = 0; k < info[2]; k++){
								sound[arrIndex + i++] = output[k];
							}
						}
					}
				}	
			
			
			/*input file is aiff*/
				else if(type < 0){
					setAIFFInfo(f, indinfo);
					
					if((indinfo[0] < 0) || (indinfo[1] < 0) || (indinfo[2] < 0) || (indinfo[3] < 0)){
						fprintf(stderr, "File %s Missing Relevant Information!", argv[i]);
						return;
					}
					if(info[0] < 0){
						for(k = 0; k < 4; k++)info[k] = indinfo[k];
					}else{
						info[3] += indinfo[3];
					}
					if(i > 1){
						for(k = 0; k < 3; k++){
							if(info[k] != indinfo[k]){
								fprintf(stderr, "File Encoding For %s Does Not Match!", argv[i]);
								return;
							}
						}
					}
					
					sound = (int *) realloc (sound, info[3] * info[2] * 4);
					int arrIndex = (info[3] - indinfo[3]) * info[2];
					int samplesTch = indinfo[3] * indinfo[2];
					
					fseek(f, 0, SEEK_END);
					int fileSize = ftell(f);
					rewind(f);
					unsigned char *buffer = (unsigned char*) malloc (fileSize);
					fread(buffer, fileSize, 1, f);
					unsigned char key[4] = "SSND";
					int index = -1;
					for(j = 0; j < fileSize; j++){
						if(strncmp(key, &buffer[j], 4) == 0){
							index = j + 16;
						}
					}
					switch(info[1]){
						case 8:
							for(j = 0; j < samplesTch; j++){
								int add = (int) buffer[index];
								index++;
								sound[arrIndex + j] = add;
							}
							break;
						case 16:
							for(j = 0; j < samplesTch; j++){
								int add = (int) Byte2Short(&buffer[index]);
								index +=2;
								sound[arrIndex + j] = add;
							}
							break;
						case 32:
							for(j = 0; j < samplesTch; j++){
								int add = Byte2Int(&buffer[index]);
								index += 4;
								sound[arrIndex + j] = add;
							}
							break;
						default:
							fprintf(stderr, "\nInvalid Bit Depth = %d. Only 8/16/32 Supported", info[1]);
							exit(0);
					}
					free(buffer);
				}
				else{
					fprintf(stderr, "\nInvalid File Type: %s\n", argv[i]);
					return;
				}
			fclose(f);
			}
		}
		
		
		/*output is aiff*/
		if(switches[2] > 0){
			writeAIFFHeader(stdout, info);
			switch (info[1]){
				case 8:
					for(i = 0; i < info[3] * info[2]; i++){
						char out = (char)sound[i];
						fwrite(&out, 1, 1, stdout);
					}
					break;
				case 16:
					for(i = 0; i < info[3] * info[2]; i++){
						short out = (short)sound[i];
						out = swap16(out);
						fwrite(&out, 2, 1, stdout);
					}
					break;
				case 32:
					for(i = 0; i < info[3] * info[2]; i++){
						int out = sound[i];
						out = swap32(out);
						fwrite(&out, 4, 1, stdout);
					}
					break;
				default:
					fprintf(stderr, "Invalid Bit Depth = %d. Only 8/16/32 Supported.", info[1]);
					exit(0);
			}
			
			
		}
		
		
		/*output is cs229*/
		else if(switches[3] > 0){
			writeCS229Header(stdout, info);
			k = 0;
			for(i = 0; i < info[3]; i++){
				for(j = 0; j < info[2]; j++){
					fprintf(stdout, "%d", sound[k++]);
					if(j < info[2]-1)fprintf(stdout, " ");
				}
				fprintf(stdout, "\n");
			}
			
		}
		free(sound);
}