#include"player.h"
#define row 3000
#define col 3000
#define MAX 10000
//#define DEBUG 1
node* head = NULL, *tail = NULL;
Frame f;
char rubbish[MAX];
int need_resize,need_color, window, step;
int end()
{
	if(f.width == 0 && f.height == 0 && f.linesize == 0 && f.data == NULL)
		return 1;
	else return 0;
}

void changemode(int dir)
{
	static struct termios oldt, newt;
  	if( dir == 1 )
  	{
    	tcgetattr( STDIN_FILENO, &oldt);
    	newt = oldt;
    	newt.c_lflag &= ~( ICANON | ECHO );
    	tcsetattr( STDIN_FILENO, TCSANOW, &newt);
  	}
  	else
    	tcsetattr( STDIN_FILENO, TCSANOW, &oldt);
}

int kbhit (void)
{
  	struct timeval tv;
	fd_set rdfs;
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	FD_ZERO(&rdfs);
	FD_SET(STDIN_FILENO, &rdfs);
 	select(STDIN_FILENO+1, &rdfs, NULL, NULL, &tv);
	return FD_ISSET(STDIN_FILENO, &rdfs);
}

int init(char* filename)
{
	int success = decoder_init(filename);
	assert(!success);
	head = (node*)malloc(sizeof(node));
	tail = (node*)malloc(sizeof(node));
	getf(head);
	getf(tail);
        head->next = tail;
	tail->next = NULL;
	return success;
}
int getf(node* new)
{
	f = decoder_get_frame();
	new->width = f.width;
	new->height = f.height;	
	int index = 0;
        for (int i = 1; i <= f.height;i++)
	{
                for (int j = 1; j <= f.width; j++)
                {
                        int r,g,b;
                        r = f.data[index++];
                        g = f.data[index++];
                        b = f.data[index++];
                        new->image[i][j].r = r;
                        new->image[i][j].g = g;
                        new->image[i][j].b = b;
                        new->image[i][j].gray = (r*30 + g*59 + b*11) / 100;
			if(need_resize)
			{
                        	new->sumimage[i][j].r = new->sumimage[i-1][j].r + new->sumimage[i][j-1].r - new->sumimage[i-1][j-1].r + new->image[i][j].r;
                        	new->sumimage[i][j].g = new->sumimage[i-1][j].g + new->sumimage[i][j-1].g - new->sumimage[i-1][j-1].g + new->image[i][j].g;
                        	new->sumimage[i][j].b = new->sumimage[i-1][j].b + new->sumimage[i][j-1].b - new->sumimage[i-1][j-1].b + new->image[i][j].b;
				new->sumimage[i][j].gray = new->sumimage[i-1][j].gray + new->sumimage[i][j-1].gray - new->sumimage[i-1][j-1].gray + new->image[i][j].gray;
			}
                }
                if (f.linesize > 3*f.width)
                {
                        index+=f.linesize - 3*f.width;
		}
	}
	if(need_resize)
                {
                        resize(window,step,new);
                }
}
void Color(node* head)
{	
	for (int i = 1; i <= head->height;i++)
	{
		for (int j = 1; j <= head->width; j++)
		{
			printf("\033[38;2;%d;%d;%dm██",head->image[i][j].r, head->image[i][j].g, head->image[i][j].b);
		}
		printf("\n");
	}
	fflush(stdout);
}

void Gray(node* head)
{
	for (int i = 1; i <= head->height; i++)
	{
		for (int j = 1; j <= head->width; j++)
		{
			printf("\033[38;2;%d;%d;%dm██",head->image[i][j].gray, head->image[i][j].gray, head->image[i][j].gray);
		}
		printf("\n");
	}	
	fflush(stdout);
}

void resize(int win, int step, node* new)
{
#ifdef DEBUG
	printf("开始resize\n");
	fflush(stdout);
#endif
	new->height = new->height / step;
	new->width = new->width / step;
	for (int i = 1, x_index = 1; i <= new->height; i++, x_index += step)
	{
		for (int j = 1, y_index = 1; j <= new->width; j++, y_index += step)
		{
			new->image[i][j].g = (new->sumimage[x_index + win - 1][y_index + win - 1].g - new->sumimage[x_index - 1][y_index + win - 1].g - new->sumimage[x_index + win -1][y_index - 1].g + new->sumimage[x_index - 1][y_index - 1].g) / (win * win);
			new->image[i][j].r = (new->sumimage[x_index + win - 1][y_index + win - 1].r - new->sumimage[x_index - 1][y_index + win - 1].r - new->sumimage[x_index + win -1][y_index - 1].r + new->sumimage[x_index - 1][y_index - 1].r) / (win * win);
			new->image[i][j].b = (new->sumimage[x_index + win - 1][y_index + win - 1].b - new->sumimage[x_index - 1][y_index + win - 1].b - new->sumimage[x_index + win -1][y_index - 1].b + new->sumimage[x_index - 1][y_index - 1].b) / (win * win);
			new->image[i][j].gray = (new->sumimage[x_index + win - 1][y_index + win - 1].gray - new->sumimage[x_index - 1][y_index + win - 1].gray - new->sumimage[x_index + win -1][y_index - 1].gray + new->sumimage[x_index - 1][y_index - 1].gray) / (win * win);
		}
	}
#ifdef DEBUG
	printf("resize finish");
	fflush(stdout);
#endif
}


void* scan(void* arg)
{
	while(!end())
	{
		node* new = (node*)malloc(sizeof(node));
		getf(new);
		tail->next = new;
		new->next =NULL;
		tail = new;
		while(head->next->next == tail)
		{
			usleep(1);
		}
	}
	return (void*)0;
}

void print()
{
	changemode(1);
	printf("\033[?25l");
	void (*func)(node*);
	if(need_color)
		func = Color;
	else
		func = Gray;
	while(head!=NULL)
	{
		while(!end()&&head->next == tail)
		{
			usleep(10);
		}
		int nosleep = 0;
                if(kbhit())
                {
                        char ch = getchar();
                        rewind(stdin);
                        if (ch == ' ')
                        {
                                while(1)
				{
					usleep(1000);
					if(kbhit() && getchar()==' ')
						break;
				}
                        }
                        else if (ch == 'd' || ch == 'D')
                                nosleep = 1;
                }
		system("clear");
		func(head);
		node* temp = head;
		head = head->next;
	     	free(temp);	
		if(!nosleep)
			usleep(46000);
	}
	changemode(0);
}
