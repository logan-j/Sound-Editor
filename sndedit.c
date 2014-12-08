#include "sndfunc.h"
#include <ncurses.h>

main(int argc, char *argv[]){
	FILE * f;
	char *fname = (char *) malloc (sizeof(strlen(argv[1])) + 1);
	fname = strcpy(fname, argv[1]);

	if(argc != 2){
		fprintf(stderr, "Invalid Number of Arguments!");
		return;
	}else{
		f = fopen(fname, "rwb");
	}
	if(f == NULL){
		fprintf(stderr, "File Not Found!");
		return;
	}
	
	int format = fformat(f);

	if(format == 0){
		fprintf(stderr, "Invalid Filetype! Only .CS229 and .AIFF Supported");
	}else{
		initscr();
		keypad(stdscr, TRUE);
		int maxw = getmaxx(stdscr);
		int maxh = getmaxy(stdscr);
		noecho();	
		WINDOW *one = newwin(10, 10, 0, 0);
		WINDOW *two = newwin(10, 10, maxh - 10, maxw - 10);
		WINDOW **out = &one;
		char ch;
		while(ch != 'q'){
			ch = getch();
			
			if(ch == '1'){
				out = &one;
			}
			else if(ch == '2'){
				out = &two;
			}else{
			
				wprintw(*out, "%c", ch);
				wrefresh(*out);
			}	


		}
	




		
		
		endwin();
	}

}
