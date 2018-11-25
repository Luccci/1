#include <GL/glut.h>
#include <stdbool.h>
#define SIRINA 20
#define VISINA 20
#define DUZINA 20

//matrica 20*20*20 opisuje trodimenzionalni prostor podeljen na kocke 
//koje popunjavamo padajucim predmetima koji su takodje sastavljeni od kocki ...
//potrebne su nam dve informacije:
//da li je polje u matrici popunjeno kockom koja pripada nekom od predmeta 
// i da li je ta kocka deo staticnog predmeta ili  koji je u kretanju(pada) 

typedef struct Polje{
bool popunjen;
bool pokretan;
}polje;

polje prostorIgranja[SIRINA][VISINA][DUZINA];

static int window_width,window_height;

static void on_display();
static void on_reshape(int width,int height);
void iscrtaj_kocku(int x,int y,int z,char c);
void oblik_T(int x,int y,int z,char c);
void oblik_O(int x,int y,int z,char c);
void oblik_I(int x,int y,int z,char c);
void oblik_J(int x,int y,int z,char c);
void oblik_Z(int x,int y,int z,char c);
void inicijalizuj_matricu();
void postolje();

int main(int argc, char **argv)
{
glutInit(&argc,argv);
glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);

glutInitWindowSize(1000,700);
glutInitWindowPosition(0,0);
glutCreateWindow(argv[0]);

inicijalizuj_matricu();

glutDisplayFunc(on_display);
glutReshapeFunc(on_reshape);

glClearColor(0,0,0,0);
glEnable(GL_DEPTH_TEST);

glutMainLoop();

return 0;
}

static void on_display(void)
	{
	glClear(GL_COLOR_BUFFER_BIT |GL_DEPTH_BUFFER_BIT);

	glViewport(0,0, window_width, window_height);

	    glMatrixMode(GL_PROJECTION);
	    glLoadIdentity();
	    gluPerspective(
		    60,
		    window_width/(float)window_height,
		    1,50 );

	    glMatrixMode(GL_MODELVIEW);
	    glLoadIdentity();
	    gluLookAt(
		    -1*6, 2*6,-3*6,
		    0, 0, 0,
		    0, 1, 0
		);
	//test iscrtavanja oblika (funkcije uzimaju za argumente pozicije na kojima se iscrtava oblik)
	//one u sebi pozivaju po cetiri funkcije "iscrtaj kocku" i tako nastaje oblik
	oblik_T(2,0,6,'m');
	oblik_O(7,0,3,'r');
	oblik_I(12,0,4,'p');
	oblik_J(2,0,0,'b');
	oblik_Z(12,0,1,'g');
	postolje();
	
	glutSwapBuffers();
	}

static void on_reshape(int width,int height)
	{
	window_width=width;
	window_height=height;
	}

void iscrtaj_kocku(int x,int y,int z,char c)
{

	if(c=='r')
		glColor3f(0.5,0,0);
	else if(c=='p')
		glColor3f(0.6, 0.5, 0.75);
	else if(c=='b')
		glColor3f(0,0,0.5);
	else if(c=='g')
		glColor3f(0,0.2,0);
	else if(c=='m')
		glColor3f(0.3, 0.2, 0.2);
	else
		glColor3f(0,0,0);
	
	glBegin(GL_POLYGON);
	glVertex3f(x-0.5,y+0.5,z-0.5);
	glVertex3f(x-0.5,y-0.5,z-0.5);	
	glVertex3f(x+0.5,y-0.5,z-0.5);
	glVertex3f(x+0.5,y+0.5,z-0.5);
	glEnd();
	
	glBegin(GL_POLYGON);
	glVertex3f(x-0.5,y+0.5,z+0.5);
	glVertex3f(x-0.5,y-0.5,z+0.5);	
	glVertex3f(x+0.5,y-0.5,z+0.5);
	glVertex3f(x+0.5,y+0.5,z+0.5);
	glEnd();
	
	glBegin(GL_POLYGON);
	glVertex3f(x-0.5,y+0.5,z-0.5);
	glVertex3f(x-0.5,y+0.5,z+0.5);	
	glVertex3f(x+0.5,y+0.5,z+0.5);
	glVertex3f(x+0.5,y+0.5,z-0.5);
	glEnd();
	
	glBegin(GL_POLYGON);
	glVertex3f(x-0.5,y-0.5,z-0.5);
	glVertex3f(x-0.5,y-0.5,z+0.5);	
	glVertex3f(x+0.5,y-0.5,z+0.5);
	glVertex3f(x+0.5,y-0.5,z-0.5);
	glEnd();
	
	glBegin(GL_POLYGON);
	glVertex3f(x+0.5,y+0.5,z+0.5);
	glVertex3f(x+0.5,y+0.5,z-0.5);	
	glVertex3f(x+0.5,y-0.5,z-0.5);
	glVertex3f(x+0.5,y-0.5,z+0.5);
	glEnd();
	
	glBegin(GL_POLYGON);
	glVertex3f(x-0.5,y+0.5,z+0.5);
	glVertex3f(x-0.5,y+0.5,z-0.5);	
	glVertex3f(x-0.5,y-0.5,z-0.5);
	glVertex3f(x-0.5,y-0.5,z+0.5);
	glEnd();
        
	//a sad ivice

	glColor3f(0,0,0);
	
	glBegin(GL_LINES);
	glVertex3f(x-0.5,y+0.5,z-0.5);
	glVertex3f(x-0.5,y-0.5,z-0.5);	
	glVertex3f(x+0.5,y-0.5,z-0.5);
	glVertex3f(x+0.5,y+0.5,z-0.5);
	glEnd();
	
	glBegin(GL_LINES);
	glVertex3f(x-0.5,y+0.5,z+0.5);
	glVertex3f(x-0.5,y-0.5,z+0.5);	
	glVertex3f(x+0.5,y-0.5,z+0.5);
	glVertex3f(x+0.5,y+0.5,z+0.5);
	glEnd();
	
	glBegin(GL_LINES);
	glVertex3f(x-0.5,y+0.5,z-0.5);
	glVertex3f(x-0.5,y+0.5,z+0.5);	
	glVertex3f(x+0.5,y+0.5,z+0.5);
	glVertex3f(x+0.5,y+0.5,z-0.5);
	glEnd();
	
	glBegin(GL_LINES);
	glVertex3f(x-0.5,y-0.5,z-0.5);
	glVertex3f(x-0.5,y-0.5,z+0.5);	
	glVertex3f(x+0.5,y-0.5,z+0.5);
	glVertex3f(x+0.5,y-0.5,z-0.5);
	glEnd();
	
	glBegin(GL_LINES);
	glVertex3f(x+0.5,y+0.5,z+0.5);
	glVertex3f(x+0.5,y+0.5,z-0.5);	
	glVertex3f(x+0.5,y-0.5,z-0.5);
	glVertex3f(x+0.5,y-0.5,z+0.5);
	glEnd();
	
	glBegin(GL_LINES);
	glVertex3f(x-0.5,y+0.5,z+0.5);
	glVertex3f(x-0.5,y+0.5,z-0.5);	
	glVertex3f(x-0.5,y-0.5,z-0.5);
	glVertex3f(x-0.5,y-0.5,z+0.5);
	glEnd();
}

void oblik_T(int x,int y,int z,char c)
{
	iscrtaj_kocku(x+1,y,z,c);
	iscrtaj_kocku(x,y,z,c);
	iscrtaj_kocku(x-1,y,z,c);
	iscrtaj_kocku(x,y+1,z,c);
}

void oblik_O(int x,int y,int z,char c)
{
	iscrtaj_kocku(x,y,z,c);
	iscrtaj_kocku(x+1,y,z,c);
	iscrtaj_kocku(x,y+1,z,c);
	iscrtaj_kocku(x+1,y+1,z,c);
}

void oblik_I(int x,int y,int z,char c)
{
	iscrtaj_kocku(x,y,z,c);
	iscrtaj_kocku(x,y+1,z,c);
	iscrtaj_kocku(x,y+2,z,c);
	iscrtaj_kocku(x,y+3,z,c);
}

void oblik_J(int x,int y,int z,char c)
{
	iscrtaj_kocku(x,y,z,c);
	iscrtaj_kocku(x,y+1,z,c);
	iscrtaj_kocku(x,y+2,z,c);
	iscrtaj_kocku(x+1,y+2,z,c);
}

void oblik_Z(int x,int y,int z,char c)
{
	iscrtaj_kocku(x,y,z,c);
	iscrtaj_kocku(x,y+1,z,c);
	iscrtaj_kocku(x+1,y+1,z,c);
	iscrtaj_kocku(x-1,y,z,c);
}

void inicijalizuj_matricu(void)
{
	int i,j,k;
	for(i=0;i<SIRINA;i++)
	{
		for(j=0;j<VISINA;j++)
		{
			for(k=0;k<DUZINA;k++)
			{
				prostorIgranja[i][j][k].popunjen=false;
				prostorIgranja[i][j][k].pokretan=false;
			}
		}
	}
}

//detalj... postolje...
void postolje()
{
	int i,j;
	glColor3f(0.3,0.3,0.3);
	for(i=0;i<20;i++)
		{
		for(j=0;j<20;j++)
			{
			glBegin(GL_POLYGON);
			glVertex3f(i-0.5,-1,j-0.5);
			glVertex3f(i-0.5,-1,j+0.5);
			glVertex3f(i+0.5,-1,j+0.5);
			glVertex3f(i+0.5,-1,j-0.5);
			glEnd();

			glBegin(GL_LINES);
			glVertex3f(i-0.5,-1,j-0.5);
			glVertex3f(i-0.5,-1,j+0.5);
			glVertex3f(i+0.5,-1,j+0.5);
			glVertex3f(i+0.5,-1,j-0.5);
			glEnd();
			}
		}
}
