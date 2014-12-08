/*
 * sndfunc.h
 * 
 * Author: Logan Jewett
 *
 */


#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#ifndef SNDFUNC_H_
#define SNDFUNC_H_

void calDur(long int *i, float *d);

int num(char *line, int size);

int calSam(FILE* input);

int setAIFFInfo(FILE* input, long int *i);

void setCS229Info(char *keyword[], long int info[], FILE* f);

unsigned int Byte2Int(unsigned char* buff);

unsigned short Byte2Short(unsigned char* buff);

long Byte2LD(unsigned char* buff);

short swap16(short input);

int swap32(int input);

long double LD2Byte10(long int num);

void parse8(char *line, char *output);

void parse16(char *line, short *output);

void parse32(char *line, int *output);

int cSwitch(int argc, char *argv[], int *switches, int mode);

int nSwitch(char arg[]);

void writeAIFFHeader(FILE *f, long int *info);
	
void writeCS229Header(FILE *f, long int *info);

int fformat(FILE* f);

#endif