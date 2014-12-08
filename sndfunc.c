/*
 * sndfunc.c
 * 
 * Author: Logan Jewett
 *
 */


#include "sndfunc.h"

	void calDur(long int *i, float *d){
		float sps = (float)i[3] / (float)i[0];
		d[0] = sps/3600.0;
		d[1] = fmod(sps, 3600.0) / 60.0;
		d[2] = fmod(sps, 60.0);
	}
	
	int num(char *line, int size){
		return atoi(&line[size]);
	}
	
	int calSam(FILE* input){
		rewind(input);
		int flip = 0;
		int count = -1;
		char line[1000];
		char key[] = "StartData";
		while(fgets(line, sizeof line, input) != NULL){
			if((line[0] != '#') && (strstr(line, key) != NULL))flip++;
			if(flip > 0)count++;
		}
		return count;
	}
	
	int setAIFFInfo(FILE* input, long int *i){
		fseek(input, 0, SEEK_END);
		int fileSize = ftell(input);
		unsigned char *buffer = (unsigned char*) malloc(fileSize);
		rewind(input);
		
		fread(buffer, fileSize, 1, input);
		int j;
		int index = -1;
		unsigned char key[4] = "COMM";
		for(j = 0; j < fileSize; j++){
			if(strncmp(key, &buffer[j], 4) == 0){
				index = j + 8;
			}
		}
		if(index < 0) return index;
		
		unsigned char channel[2];
		unsigned char samples[4];
		unsigned char size[2];
		unsigned char rate[10];
		
		int k;
		
		for(k = 0; k < sizeof channel; k++)channel[k] = buffer[index + k];
		for(k = 0; k < sizeof samples; k++)samples[k] = buffer[index + 2 + k];
		for(k = 0; k < sizeof size; k++)size[k] = buffer[index + 6 + k];
		for(k = 0; k < sizeof rate; k++)rate[k] = buffer[index + 8 + k];
		i[0] = Byte2LD(rate);
		i[1] = Byte2Short(size);
		i[2] = Byte2Short(channel);
		i[3] = Byte2Int(samples);
				
	}
	
	void setCS229Info(char *keyword[], long int info[], FILE* f){
		char line[1000];
		while(fgets(line, sizeof line, f) != NULL){
			int i;
			for(i = 0; i < 4; i++){
				if((strstr(line, keyword[i]) != NULL) && (line[0] != '#')){
					info[i] = num(line, strlen(keyword[i]));
					if(i == 1){
						info[4] = (pow(2, (info[1] -1)));
					}
				}
			}
		}
	}

	
	unsigned short Byte2Short(unsigned char *buff){
		unsigned char* byte = buff;
		return ((byte[0]<<8)|(byte[1]));
	}
	
	unsigned int Byte2Int(unsigned char *buff){
		unsigned char* byte = buff;
		return ((byte[0]<<24)|(byte[1]<<16)|(byte[2]<<8)|(byte[3]));
	}

	short swap16(short input){
		return (input >> 8) | (input << 8);
	}
	
	int swap32(int input){
		return ((input>>24)&0xff) | ((input<<8)&0xff0000) | ((input>>8)&0xff00) | ((input<<24)&0xff000000);
	}
	
	long Byte2LD(unsigned char *buff){
		unsigned int i = (((buff[0]<<8)|(buff[1])) - 16383);
		long j = 0;
		int k;
		int count = 0;
		for(k = i; k >= 0; k--){
			int power = pow(2, k);
			char place = buff[2 + (count/8)];
			char offset = (7 - (count%8));
			
			j += power * ((place >> offset)&0x1);
			count++;
		}
		return j;
	}
	
	long double LD2Byte10(long int num){
		long double out;
		long double temp = (long double) num;
		unsigned char *convert = (unsigned char*) &temp;
		unsigned char *output = (unsigned char*) &out;
		int i;
		for(i = 0; i < 10; i++){
			output[i] = convert[9-i];
		}
		return out;
	}
	
	void parse8(char *line, char *output){
		int i = 0;
		char *add = strtok(line, " \t");
		while(add != NULL){
			output[i++] = (char) atoi(add);
			add = strtok(NULL, " \t");
		}
	}
	
	void parse16(char *line, short *output){
		int i = 0;
		char *add = strtok(line, " \t");
		while(add != NULL){
			output[i++] = (short) atoi(add);
			add = strtok(NULL, " \t");
		}
	
	}
	
	void parse32(char *line, int *output){
		int i = 0;
		char *add = strtok(line, " \t");
		while(add != NULL){
			output[i++] = (int) atoi(add);
			add = strtok(NULL, " \t");
		}
	}
	
	int cSwitch(int argc, char *argv[], int *switches, int mode){
		int i;
		int check = 0;
		char key[6] = {'h', 'l', 'a', 'c', 'w', 'z'};
		for(i = 1; i < argc; i++){
			if(argv[i][0] == '-'){
				if(strlen(argv[i]) > 2)return -i;
				int j;
				for(j = 0; j < 6; j++){
					if(strchr(argv[i], key[j])){
						if((j != 4) && (j != 5) && ((j != 3) || (mode == 0))){
							switches[j]++;
						}else{
							if(argc > i + 1){
								if(j == 4){
									if((atoi(argv[i+1])) < 20){
										return -(i + 1);
									}
									if(((atoi(argv[i+1])) % 2) != 0){
										switches[j] = atoi(argv[i+1]) - 1;
									}else{
										switches[j] = atoi(argv[i+1]);
									}
								}else{
									switches[j] = atoi(argv[i+1]);
								}
								
							}else{
								return -i;
							}
						}
					}
				}
				if((switches[0] == 0) && (switches[1] == 0) && (switches[2] == 0) && (switches[3] == 0) && (switches[4] == 0) && (switches[5] == 0)){
					return -i;
				}
			}
		}
		return check;
	}
	
	int nSwitch(char arg[]){
		return (arg[0] != '-');
	}
	
	void writeAIFFHeader(FILE *f, long int *info){
		rewind(f);
		fprintf(f, "FORM");
		unsigned int data = info[3] * info[2] * info[1]/8 + 46;
		data = swap32(data);
		fwrite(&data, 4, 1, f);
		fprintf(f, "AIFFCOMM");
		
		unsigned short channel = (unsigned short) info[2];
		channel = swap16(channel);
			
		unsigned short bitdepth = (unsigned short) info[1];
		bitdepth = swap16(bitdepth);
				
		unsigned int samples = (unsigned int) info[3];
		samples = swap32(samples);
		unsigned int size = 18;
		size = swap32(size);
				
		long double temp = LD2Byte10(info[0]);
			
		fwrite(&size, 4, 1, f);
		fwrite(&channel, 2, 1, f);
		fwrite(&samples, 4, 1, f);
		fwrite(&bitdepth, 2, 1, f);
		fwrite(&temp, 10, 1, f);
	
		fprintf(f, "SSND");
		int i;
		unsigned int size2 = info[3] * info[2] * info[1]/8 + 8;
		size2 = swap32(size2);
		fwrite(&size2, 4, 1, f);
			
		for(i = 0; i < 8; i++)putc(0x0, f);
	}
	
	void writeCS229Header(FILE *f, long int *info){
		fprintf(f, "CS229\n\nSamples %d\n\nChannels %d\n\nBitDepth %d\n\nSampleRate %d\n\nStartData\n",
					info[3], info[2], info[1], info[0]);
	}
	
	int fformat(FILE* f){
		char format[6];
		fscanf(f, "%5s", format);
		char c[] = "CS229";
		char a[] = "FORM";
		rewind(f);
		if(strncmp(format, c, 5) == 0)return 1;
		if(strncmp(format, a, 4) == 0)return -1;
		else return 0;
	}
	
	
