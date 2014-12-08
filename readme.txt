Project 1 Part 2
Logan Jewett
Readme File
9/28/13
UPDATED: 10/9/13

*******************************************************************
About This Document:

Not really sure how to write a readme. The instructions were kind
of ambiguous, so I'm kind of just basing this off of a couple
random ones I found on my computer. I didn't do a lot of comments
(basically none) since the specs say documentation is based on
this, at least for part one.


*******************************************************************

THE FILES

1.	SNDFUNC.C
2.	SNDINFO.C
3.	SNDCONV.C
4.	SNDCAT.C
5.	SNDCUT.C
6.	SNDSHOW.C

-----------------------
1. SNDFUNC.C
-----------------------
This file is basically what it /sounds/ like. "Sound Functions".
I noticed that there ended being a lot of the same operations
between SNDCONV and SNDINFO that I was going to need pretty
often so I just tried to encapsulate most of them into one file.
And I thought it'd be a good opportunity to learn how to
incorporate functions from other files.

So instead of including the header file "SNDFUNC.H" in this
readme since it's pretty self explanatory I figure I'll just
go over what all of the functions do briefly.

*******************************************************************
	void calDur(long int *i, float *d);

This just takes a pointer to the int array I use to store
basic info about the files (that gets processed in a different
function) and calculates the duration from that info.

*******************************************************************
	int num(char *line, int size);

This is used by setCS229Info for reading .cs229 files. It takes
a line as a char array and extracts and int after a specific index,
which is a size of the keyword.

*******************************************************************
	int calSam(FILE* input);
	
CALculate SAMples. This is used later for verifications purposes. It
calculates the number of samples in a .cs229 file and then checks it
with whatever the reported size is.

*******************************************************************
	int setAIFFInfo(FILE* input, long int *i);

Takes a pointer to an AIFF file as well as a pointer to the int
array used to store information about a file and then reads the
file to determine the number of samples/samplerate/etc. information
and then sets the appropriate values in the int array. Returns
an int of the index in the file of the COMM chunk (set to -1) 
otherwise. Used for verifying if there's missing chunks.

*******************************************************************
	void setCS229Info(char *keyword[], long int info[], FILE* f);

Similar	to setAIFFInfo in that it takes a long int pointer for
storing info and a file pointer for calculating the info. Also
takes a keyword array that it uses for checking around in .cs229
files to find the relevant information.

*******************************************************************
	unsigned int Byte2Int(unsigned char* buff);
	
Takes a char array and converts the byte information into a 4 byte
integer. Used for reading the binary info in .aiff files.

*******************************************************************
	unsigned short Byte2Short(unsigned char* buff);

Same idea as Byte2Int. Takes a char array and converts the byte
information	into a 2 byte short. Used for reading the binary info
in .aiff files.

*******************************************************************
	long Byte2LD(unsigned char* buff);
	
Same idea as the other two similar functions. This is for reading
that 10 byte floating point number and converts the binaries into an
8 byte long (since it's used to store the sample rate I didn't think
it needed to be floating point either).

*******************************************************************
	short swap16(short input);
	
The endianness problem was a pain. Wrote this function to switch
endianness for a 2 byte short.

*******************************************************************
	int swap32(int input);
	
Same as the swap16 function except written for 4 byte integers. Just
switches the endianness of the input.

*******************************************************************
	long double LD2Byte10(long int num);
	
Function for converting the long int number into the 10 byte 
floating point and change its endianness. Luckily for me I found
out this was an easier operation since all it required was a cast
to a long double of 16 bytes and then since the long double just
had 6 more bytes of precision it just involved taking the first 10
bytes of information and flipping it around.

*******************************************************************
	void parse8(char *line, char *output);
	
Takes a pointer to a line in the cs229 file and returns a char
representation of all of the numbers that were extracted from the
line. Only delimits using spaces and tabs though. Took me a while
before I found the strtok function. Obviously the line is fed in
through the char pointer, and it just manipulates a pointer given
to it. I feel like I ran into trouble trying to return a char
pointer and opted for that mechanism instead.

*******************************************************************
	void parse16(char *line, short *output);

Same as the parse 8 function except rewritten to handle/return 2
byte shorts. So it parses the numbers from a line from the .cs229
file and parses it into shorts. Obviously the line is fed in
through the char pointer.

*******************************************************************
	void parse32(char *line, int *output);
	
Same as the other two parse functions except rewritten to handle
4 byte integers. Just parses the numbers from a line from the
.cs229 file and parses it into integers. Obviously the line is
fed in through the char pointer.
*******************************************************************

				NEW FUNCTIONS ADDED AROUND 10/9/13

*******************************************************************
	int cSwitch(int argc, char *argv[], int *switches, int mode);
	
This function will take argc, argv, an int array, and a mode value
and place relevant information regarding the switches (if there
are any) in argv. The return value is the index where there's a 
bad switch, and the mode value is for switching between what
the -c switch is used for. Used extensively for handling switch
operations in every other file. Also sets -c, -w, and -z values in
the int array.
*******************************************************************
	int nSwitch(char arg[]);

This simply checks if a char array starts with a "-" which would
indicate whether it was a switch or not. Returns true if the
array is not a switch.
*******************************************************************
	void writeAIFFHeader(FILE *f, long int *info);

This takes a file pointer to output to, as well as an int array
containing all of the relevant information, and then outputs all
of the header info up until the sound information in the SSND
chunk.
*******************************************************************
	void writeCS229Header(FILE *f, long int *info);

Like the writeAIFFHeader function, but for CS229 files. Makes it
a lot easier to get writing to files finished and organized.
*******************************************************************
	int fformat(FILE* f);

This function just takes a file pointer and returns an int that
indicates what kind of file it is. 1 if it's a cs229 file, -1 if
it's aiff, and 0 otherwise. Makes it easier to check formats.
*******************************************************************







-----------------------
2.	SNDINFO.C
-----------------------
With the encapsulation of most of the necessary functions into
SNDFUNC this is pretty basic. Mostly an implementation of those
functions with some pretty low level error checking on top.
Just looks to see if there's missing sound information that it
couldn't find or whether it isn't either a .cs229 or
.AIFF file. Mostly just runs the setAIFFInfo and setCS229Info
and then prints it out.





-----------------------
3. SNDCONV.C
-----------------------
This right now is not pretty at all. Was more worried about getting
it to work than simplification. The idea is pretty basic, just extracts
the info using setAIFFInfo and setCS229Info and then outputs it into
the specified file into the opposite format, then runs a variety of
loops to process the sound data and convert it. The majority of the
meat of the code is in SNDFUNC.C

EDIT 10/9/13: Has been cleaned up with the usage of writeAIFFHeader
and writeCS229Header, as well as fformat. Still needs some work.






-----------------------
4. SNDCAT.C
-----------------------
*note:
There's some overlapped code that needs to get conglomerated, but
there's still issues with the machine being used to write this code
not allowing shared allocated memory between sndfunc and any
other associated files. So until that time there's some overlap
between snd(cat/cut/show). It's mostly dealing with reading in
sound information.

Sndcat reads in files, checks them for compatibility, and calculates 
what the resulting concatenated sound information size will be.
Then it uses a dynamically allocated int array that it reads the 
sound information into for each file (not file type dependent). 
After each file has been handled it outputs it into the specified
type.






-----------------------
5. SNDCUT.C
-----------------------
There are a couple of sndcut specific functions that I've included.
I'd considered adding them to sndfunc but they're only going to be
used by sndcut. The design behind sndcut is that it reads in the
sound information, calculates what the resulting number of samples
will be given the ranges, then outputs that information and as it
reads out the sound information, skips over indexes between those
ranges. However at this time, when calculating number of samples,
it assumes that there is no overlap between ranges.


*******************************************************************
	int range(long int *bot, long int *top, long int index, int size)

This takes two long int arrays, an index, and the size of the long
int arrays and then checks to see index is in between any of the 
ranges, then returns an int representation of its finding.
*******************************************************************
	int parseArg(char *input, long int *bot, long int *top, int index);

Takes two long int pointers, a char pointer, and an index. The char
pointer is for representing a range argument argv[i] that is parsed
and then the values that it pulls out are split into corresponding
locations in the long int arrays.
*******************************************************************





-----------------------
6. SNDSHOW.C
-----------------------
This has one specific local function that's used to calculate/
print out what gets displayed. The program reads in a specified
file's sound info into an array then uses a (clever) series of loops
to encapsulate the the zoom and other related functions.

*******************************************************************
	void printLine(int width, int value, int max, int channels, int index);
	
Takes an representing the width, the value to be print out, the max
value associated with the bit depth, the number of channels, as
well as the location that the value is currently located in the
sound data and then uses that information to calculate and print out
the associated information.
*******************************************************************









