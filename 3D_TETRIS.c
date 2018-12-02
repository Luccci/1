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
void oblik_T(int x,int y,int z);
void oblik_O(int x,int y,int z);
void oblik_I(int x,int y,int z);
void oblik_L(int x,int y,int z);
void oblik_Z(int x,int y,int z);
void oblik_Y(int x,int y,int z);
void oblik_X(int x,int y,int z);
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
	postolje();	
	oblik_T(2,0,6);
	oblik_O(7,0,3);
	oblik_I(12,0,4);
	oblik_L(2,0,0);	
	oblik_Z(12,0,1);	
	oblik_Y(10,0,10);
	oblik_X(6,0,10);
	/*
	glRotatef(90,1,1,1);
	glTranslatef(-5,-5,-5);	
	oblik_X(5,5,5);
	*/	
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
	else if(c=='z')
		glColor3f(1,1,0);
	else if(c=='o')
		glColor3f(1,0.5,0);
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

void oblik_T(int x,int y,int z)
{
	iscrtaj_kocku(x+1,y,z,'m');
	iscrtaj_kocku(x,y,z,'m');
	iscrtaj_kocku(x-1,y,z,'m');
	iscrtaj_kocku(x,y+1,z,'m');
}

void oblik_O(int x,int y,int z)
{
	iscrtaj_kocku(x,y,z,'r');
	iscrtaj_kocku(x+1,y,z,'r');
	iscrtaj_kocku(x,y+1,z,'r');
	iscrtaj_kocku(x+1,y+1,z,'r');
}

void oblik_I(int x,int y,int z)
{
	iscrtaj_kocku(x,y,z,'p');
	iscrtaj_kocku(x,y+1,z,'p');
	iscrtaj_kocku(x,y+2,z,'p');
	iscrtaj_kocku(x,y+3,z,'p');
}

void oblik_L(int x,int y,int z)
{
	iscrtaj_kocku(x,y,z,'b');
	iscrtaj_kocku(x,y+1,z,'b');
	iscrtaj_kocku(x,y+2,z,'b');
	iscrtaj_kocku(x+1,y+2,z,'b');
}

void oblik_Z(int x,int y,int z)
{
	iscrtaj_kocku(x,y,z,'g');
	iscrtaj_kocku(x,y+1,z,'g');
	iscrtaj_kocku(x+1,y+1,z,'g');
	iscrtaj_kocku(x-1,y,z,'g');
}

void oblik_Y(int x,int y,int z)
{
	iscrtaj_kocku(x,y,z,'z');
	iscrtaj_kocku(x-1,y,z,'z');
	iscrtaj_kocku(x,y+1,z,'z');
	iscrtaj_kocku(x,y,z-1,'z');
}

void oblik_X(int x,int y,int z)
{
	iscrtaj_kocku(x,y,z,'o');
	iscrtaj_kocku(x-1,y,z,'o');
	iscrtaj_kocku(x,y+1,z,'o');
	iscrtaj_kocku(x,y+1,z-1,'o');
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
			glVertex3f(i-0.5,-0.5,j-0.5);
			glVertex3f(i-0.5,-0.5,j+0.5);
			glVertex3f(i+0.5,-0.5,j+0.5);
			glVertex3f(i+0.5,-0.5,j-0.5);
			glEnd();

			glBegin(GL_LINES);
			glVertex3f(i-0.5,-0.5,j-0.5);
			glVertex3f(i-0.5,-0.5,j+0.5);
			glVertex3f(i+0.5,-0.5,j+0.5);
			glVertex3f(i+0.5,-0.5,j-0.5);
			glEnd();
			}
		}
}

/*
void oblik_T(int x,int y,int z)
{
	glColor3f(0.3, 0.2, 0.2);
	glTranslatef(x+1,y+0,z+0);
	glutSolidCube(1);
	glutWireCube(1);
	glTranslatef(-1,0,0);
	glutSolidCube(1);
	glutWireCube(1);
	glTranslatef(-1,0,0);
	glutSolidCube(1);
	glutWireCube(1);
	glTranslatef(1,1,0);
	glutSolidCube(1);
	glutWireCube(1);
}

void oblik_O(int x,int y,int z)
{
	glColor3f(0.5,0,0);
	glTranslatef(x,y,z);
	glutSolidCube(1);
	glutWireCube(1);
	glTranslatef(1,0,0);
	glutSolidCube(1);
	glutWireCube(1);
	glTranslatef(0,1,0);
	glutSolidCube(1);
	glutWireCube(1);
	glTranslatef(-1,0,0);
	glutSolidCube(1);
	glutWireCube(1);
}

void oblik_I(int x,int y,int z)
{
	glColor3f(0.6, 0.5, 0.75);
	glTranslatef(x,y,z);
	glutSolidCube(1);
	glutWireCube(1);
	glTranslatef(0,1,0);
	glutSolidCube(1);
	glutWireCube(1);
	glTranslatef(0,1,0);
	glutSolidCube(1);
	glutWireCube(1);
	glTranslatef(0,1,0);
	glutSolidCube(1);
	glutWireCube(1);
}

void oblik_J(int x,int y,int z)
{
	glColor3f(0,0,0.5);
	glTranslatef(x,y,z);
	glutSolidCube(1);
	glutWireCube(1);
	glTranslatef(0,1,0);
	glutSolidCube(1);
	glutWireCube(1);
	glTranslatef(0,1,0);
	glutSolidCube(1);
	glutWireCube(1);
	glTranslatef(1,0,0);
	glutSolidCube(1);
	glutWireCube(1);
}

void oblik_Z(int x,int y,int z)
{
	glColor3f(0,0.2,0);
	glTranslatef(x,y,z);
	glutSolidCube(1);
	glutWireCube(1);
	glTranslatef(0,1,0);
	glutSolidCube(1);
	glutWireCube(1);
	glTranslatef(1,0,0);
	glutSolidCube(1);
	glutWireCube(1);
	glTranslatef(-2,-1,0);
	glutSolidCube(1);
	glutWireCube(1);
}

void oblik_Y(int x,int y,int z)
{
	glColor3f(1,1,0);
	glTranslatef(x,y,z);
	glutSolidCube(1);
	glutWireCube(1);
	glTranslatef(-1,0,0);
	glutSolidCube(1);
	glutWireCube(1);
	glTranslatef(1,0,-1);
	glutSolidCube(1);
	glutWireCube(1);
	glTranslatef(0,1,1);
	glutSolidCube(1);
	glutWireCube(1);
}

void oblik_X(int x,int y,int z)
{
	glColor3f(1,0.5,0);
	glTranslatef(x,y,z);
	glutSolidCube(1);
	glutWireCube(1);
	glTranslatef(-1,0,0);
	glutSolidCube(1);
	glutWireCube(1);
	glTranslatef(1,1,0);
	glutSolidCube(1);
	glutWireCube(1);
	glTranslatef(0,0,-1);
	glutSolidCube(1);
	glutWireCube(1);
}
*/
