/*
 * sndconv.c
 * 
 * Author: Logan Jewett
 *
 */



#include "sndfunc.h"

void main(int argc, char *argv[]){
		int switches[6] = {0};
		int check = cSwitch(argc, argv, switches, 0);
		if(check < 0){
			fprintf(stderr, "\nInvalid Switch Operator: %s\n", argv[-check]);
			return;
		}
		if((switches[4] != 0) && (switches[5] != 0) && (switches[2] + switches[3] > 1)){
			fprintf(stderr, "\nInvalid switch operator/value for %s use -h for more info.\n", argv[0]);
			return;
		}
		
		/*Help window*/
		if(switches[0] > 0){
			fprintf(stderr, "\n\nDESCRIPTION\n\nsndconv takes sound file information and converts it to other sound file types.\
				\nUnder default usage, sndconv takes the sound information from stdin and sends it to stdout.\
				\n\n\t$ sndconv [switch] < [file] > [file]\
				\n\nOPTIONS\n\t-h\tPrint a help message and exit.\
				\n\n\t-l\tDisplays a prompt and takes the following files as input and output.\
				\n\n\t-a\tOutput is in AIFF format.\
				\n\n\t-c\tOutput is in CS229 format.\n\n");
				return;
		}
		
		FILE *f;
		FILE *newf;
		
		/*Part 1 mode*/
		if(switches[1] > 0){
			fprintf(stderr, "\nEnter the (path)name of a sound file:\n\n");
			fflush(stdout);
			
			
			char input[100];
			scanf("%99s", input);
			f = fopen(input, "rb");
			
			fprintf(stderr, "\nEnter the (path)name of the new file:\n\n");
			char output[100];
			scanf("%99s", output);
			
			if(f == NULL){
				fprintf(stderr, "\nFile Not Found!\n");
				return;
			}
			newf = fopen(output, "wb");	
		}else{
			f = stdin;
			newf = stdout;
		}
			
			char fformat[6];
			long int info[4] = {-1,-1,-1,-1};
			float duration[3] = {0};
			fscanf(f, "%5s", fformat);
			char *p = fformat;
			*p = toupper(*p);
			char c[] = "CS229";
			char a[] = "FORM";
			char *keyword[4] = {"SampleRate", "BitDepth", "Channels", "Samples"};
				int i;
			
			/*CS229 to AIFF*/
			if((strcmp(p, c) == 0) && (switches[3] == 0)){
				setCS229Info(keyword, info, f);
				
				for(i = 0; i < 4; i++){
					if(info[i] < 0){
						fprintf(stderr, "\nMissing File Data!\n");
						return;
					}
				}
				fprintf(newf, "FORM");
				unsigned int data = info[3] * info[2] * info[1]/8 + 46;
				data = swap32(data);
				
				fwrite(&data, 4, 1, newf);
				fprintf(newf, "AIFFCOMM");
				
				unsigned short channel = (unsigned short) info[2];
				channel = swap16(channel);
				
				unsigned short bitdepth = (unsigned short) info[1];
				bitdepth = swap16(bitdepth);
				
				unsigned int samples = (unsigned int) info[3];
				samples = swap32(samples);
				unsigned int size = 18;
				size = swap32(size);
				
				long double temp = LD2Byte10(info[0]);
				
				fwrite(&size, 4, 1, newf);
				fwrite(&channel, 2, 1, newf);
				fwrite(&samples, 4, 1, newf);
				fwrite(&bitdepth, 2, 1, newf);
				fwrite(&temp, 10, 1, newf);
				
				int i;
				char key[4] = "SSND";
				fwrite(key, 4, 1, newf);
				
				unsigned int size2 = info[3] * info[2] * info[1]/8 + 8;
				size2 = swap32(size2);
				fwrite(&size2, 4, 1, newf);
				
				for(i = 0; i < 8; i++)putc(0x0, newf);
				char line[1000];
				int flip = 0;
				rewind(f);
				char *skey = "StartData";
				if(info[1] == 8){
					char output[info[2]];
					while(fgets(line, sizeof line, f) != NULL){
						if((line[0] != '#') && (strstr(line, skey) != NULL))flip++;
						if((flip > 0) && (strstr(line, skey) == NULL)){
							parse8(line, output);
							for(i = 0; i < info[2]; i++)fputc(output[i], newf);
						}
					}
					
				}else if(info[1] == 16){
					short output[info[2]];
					while(fgets(line, sizeof line, f) != NULL){
						if((line[0] != '#') && (strstr(line, skey) != NULL))flip++;
						if((flip > 0) && (strstr(line, skey) == NULL)){
							parse16(line, output);
							for(i = 0; i < info[2]; i++){
								short add = output[i];
								add = swap16(add);
								fwrite(&add, 2, 1, newf);
							}
						}
					}
					
				}else if(info[1] == 32){
					int output[info[2]];
					while(fgets(line, sizeof line, f) != NULL){
						if((line[0] != '#') && (strstr(line, skey) != NULL))flip++;
						if((flip > 0) && (strstr(line, skey) == NULL)){
							parse32(line, output);
							for(i = 0; i < info[2]; i++){
								int add = output[i];
								add = swap32(add);
								fwrite(&add, 4, 1, newf);
							}
						}
					}
					
				}else{
					fprintf(stderr, "Bit Depth %d Not Supported", info[1]);
					return;
				}
			}
			
			/*AIFF to CS229*/
			else if((strcmp(p, a) == 0) && (switches[2] == 0)){
				rewind(f);
				int chunk = setAIFFInfo(f, info);
				if(chunk < 0){
					fprintf(stderr, "\nMissing Chunk!\n");
					return;
				}
				
				for(i = 0; i < 4; i++){
					if(info[i] < 0){
						fprintf(stderr, "\nMissing File Data!\n");
						return;
					}
					
				}
				fprintf(newf, "CS229\n\nSamples %d\n\nChannels %d\n\nBitDepth %d\n\nSampleRate %d\n\nStartData\n",
					info[3], info[2], info[1], info[0]);
				fseek(f, 0, SEEK_END);
				int fileSize = ftell(f);
				
				rewind(f);
				unsigned char *buffer = (unsigned char*) malloc (fileSize);
				fread(buffer, fileSize, 1, f);
				int j;
				int index = -1;
				unsigned char key[4] = "SSND";
				for(j = 0; j < fileSize; j++){
					
					if(strncmp(key, &buffer[j], 4) == 0){
						index = j + 4;
					}
				}
				if(index < 0 && info[3] != 0){
					fprintf(stderr, "Missing Chunk!");
					return;
				}
				
				int chunksize = Byte2Int(&buffer[index]);
				int offset = Byte2Int(&buffer[index + 4]);
				int blocksize = Byte2Int(&buffer[index + 8]);
				index = (index + 12 + offset);
				
				if(info[1] != 8 && info[1] != 16 && info[1] != 32){
					fprintf(stderr, "\nUnsupported Bit Depth = %d\n", info[1]);
					return;
				}
				int k;
				if(info[1] == 32){
					for(j = 0; j < info[3]; j++){
						for(k = 0; k < info[2]; k ++){
							int o = Byte2Int(&buffer[index]);
							index += 4;
							fprintf(newf, "%d", o);
							if(k < info[2] - 1)fprintf(newf, " ");
						}
						fprintf(newf, "\n");
					}
				}else if(info[1] == 16){
					for(j = 0; j < info[3]; j++){
						for(k = 0; k < info[2]; k ++){
							short o = Byte2Short(&buffer[index]);
							index += 2;
							fprintf(newf, "%d", o);
							if(k < info[2] - 1)fprintf(newf, " ");
						}
						fprintf(newf, "\n");
					}
				}else{
					for(j = 0; j < info[3]; j++){
						for(k = 0; k < info[2]; k ++){
							char o = buffer[index++];
							fprintf(newf, "%d", o);
							if(k < info[2] - 1)fprintf(newf, " ");
						}
						fprintf(newf, "\n");
					}
				}
			}else if((strcmp(p, a) != 0) && (strcmp(p, c) != 0)){
				fprintf(stderr, "\nUnrecognized File Format.\n");
				return;
			}else{
				fseek(f, 0, SEEK_END);
				int fileSize = ftell(f);
				unsigned char *buffer = (unsigned char*) malloc(fileSize);
				rewind(f);
				fread(buffer, fileSize, 1, f);
				int i;
				while(i < fileSize){
					fputc(buffer[i], newf);
					i++;
				}
		
		
			}
			fclose(newf);
			fclose(f);
		
}
