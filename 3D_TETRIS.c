#include <GL/glut.h>
#include <stdbool.h>
#define SIRINA 10
#define VISINA 20
#define DUZINA 10

//matrica 10*20*10 opisuje trodimenzionalni prostor podeljen na kocke 
//koje popunjavamo padajucim predmetima koji su takodje sastavljeni od kocki ...
//potrebne su nam dve informacije:
//da li je polje u matrici popunjeno kockom koja pripada nekom od predmeta 
// i da li je ta kocka deo staticnog predmeta ili  koji je u kretanju(pada) 
bool prostorIgranja[SIRINA][VISINA][DUZINA];

static int window_width,window_height;

static void on_display();
static void on_reshape(int width,int height);
void iscrtaj_kocku(int x,int y,int z,char c);
void inicijalizuj_matricu();

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
		    1,20 );

	    glMatrixMode(GL_MODELVIEW);
	    glLoadIdentity();
	    gluLookAt(
		    -1*5, 2*5,-3*5,
		    0, 0, 0,
		    0, 1, 0
		);
		
	iscrtaj_kocku(-1.5,0.5,0.5,'m');
	iscrtaj_kocku(1.5,0.5,0.5,'m');
	iscrtaj_kocku(-0.5,0.5,0.5,'m');
	iscrtaj_kocku(0.5,1.5,0.5,'m');

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

void inicijalizuj_matricu(void)
{
	int i,j,k;
	for(i=0;i<SIRINA;i++)
	{
		for(j=0;j<VISINA;j++)
		{
			for(k=0;k<DUZINA;k++)
			{
				prostorIgranja[i][j][k]=false;
			}
		}
	}
}
