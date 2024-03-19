#ifndef PLAYER_H
#define PLAYER_H
#include<stdio.h>
#include<stdlib.h>
#include"video_decoder.h"
#include<getopt.h>
#include<assert.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/time.h>
#include<termios.h>
#include<pthread.h>
#include<malloc.h>
#define col 3000
#define row 3000
typedef struct _IMAGE
{
	int r,g,b,gray;
}IMAGE;

typedef struct _NODE
{
	IMAGE image[row][col];
	IMAGE sumimage[row][col];
	int width,height;
	struct _NODE* next;
}node;

int init(char* filename);
void Color(node*);
void Gray(node*);
void resize(int w,int h, node*);
int getf(node*);
int kbhit();
void changemode(int dir);
int end();
void print();
void* scan(void*);
#endif
