#include <GL/glut.h>
#include <stdbool.h>
#define SIRINA 16
#define VISINA 16
#define DUZINA 16
#define TIMER_INTERVAL 10
#define TIMER_ID 0

//matrica 16*16*16 opisuje trodimenzionalni prostor podeljen na kocke 
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

int animation_on=0;
float animation_param=0;

int Px=7;
int Py=15;
int Pz=7;
int brZ=0;
int brX=0;

GLfloat light_position[]={1,1,1,0};
GLfloat light_diffuse[]={0.7, 0.7, 0.7, 1};
GLfloat light_ambient[]={0.5, 0.5, 0.5, 1};
GLfloat light_specular[]={0.9, 0.9, 0.9, 1};

GLfloat shininess=30;
	
GLfloat Tmaterial_diffuse[]={0.3, 0.2, 0.2, 1};
GLfloat Tmaterial_ambient[]={0.3, 0.2, 0.2, 1};
GLfloat Tmaterial_specular[]={0.9, 0.9, 0.9, 1};
	
GLfloat Omaterial_diffuse[]={0.5,0,0,1};
GLfloat Omaterial_ambient[]={0.5, 0, 0, 1};
GLfloat Omaterial_specular[]={0.9, 0.9, 0.9, 1};
	
GLfloat Imaterial_diffuse[]={0.6, 0.5, 0.75, 1};
GLfloat Imaterial_ambient[]={0.6, 0.5, 0.75, 1};
GLfloat Imaterial_specular[]={0.9, 0.9, 0.9, 1};

GLfloat Lmaterial_diffuse[]={0, 0, 0.5, 1};
GLfloat Lmaterial_ambient[]={0, 0, 0.5, 1};
GLfloat Lmaterial_specular[]={0.9, 0.9, 0.9, 1};

GLfloat Zmaterial_diffuse[]={0, 0.2, 0, 1};
GLfloat Zmaterial_ambient[]={0, 0.2, 0, 1};
GLfloat Zmaterial_specular[]={0.9, 0.9, 0.9, 1};

GLfloat Ymaterial_diffuse[]={1, 1, 0, 1};
GLfloat Ymaterial_ambient[]={1, 1, 0, 1};
GLfloat Ymaterial_specular[]={0.9, 0.9, 0.9, 1};

GLfloat Xmaterial_diffuse[]={1, 0.5, 0, 1};
GLfloat Xmaterial_ambient[]={1, 0.5, 0, 1};
GLfloat Xmaterial_specular[]={0.9, 0.9, 0.9, 1};

GLfloat Bmaterial_diffuse[]={0,0,0,1};
GLfloat Bmaterial_ambient[]={0,0,0,1};
GLfloat Bmaterial_specular[]={0,0,0,1};

static void on_keyboard(unsigned char key,int x,int y);
static void on_display();
static void on_reshape(int width,int height);
static void on_timer(int value);

void inicijalizuj_matricu();
void postolje();

/*
void oblik_T(int x,int y,int z);
void oblik_O(int x,int y,int z);
void oblik_I(int x,int y,int z);
void oblik_L(int x,int y,int z);
void oblik_Z(int x,int y,int z);
void oblik_Y(int x,int y,int z);
void oblik_X(int x,int y,int z);
*/

void oblik_TR(float x,float y,float z);
void oblik_OR(float x,float y,float z);
void oblik_IR(float x,float y,float z);
void oblik_LR(float x,float y,float z);
void oblik_ZR(float x,float y,float z);
void oblik_YR(float x,float y,float z);
void oblik_XR(float x,float y,float z);
void iscrtaj_kocku(float x,float y,float z,char c);

int main(int argc, char **argv)
{
glutInit(&argc,argv);
glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);

glutInitWindowSize(1000,700);
glutInitWindowPosition(0,0);
glutCreateWindow(argv[0]);

inicijalizuj_matricu();

glutKeyboardFunc(on_keyboard);
glutReshapeFunc(on_reshape);
glutDisplayFunc(on_display);

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
		1,
		100);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(
		-1*10, 2*10,-3*10,
		0, 0, 0,
		0, 1, 0);

	glShadeModel(GL_SMOOTH);
	glEnable(GL_NORMALIZE);
	    
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	
	glLightfv(GL_LIGHT0,GL_POSITION,light_position);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,light_diffuse);
	glLightfv(GL_LIGHT0,GL_AMBIENT,light_ambient);
	glLightfv(GL_LIGHT0,GL_SPECULAR,light_specular);

	postolje();
	
	glPushMatrix();
	glTranslatef(Px,Py-animation_param,Pz);
	glRotatef( brZ *90 , 0, 0, -1);
	glRotatef( brX *90 , 1, 0, 0);
	glTranslatef(-Px,-Py+animation_param,-Pz);
	oblik_TR(Px,Py-animation_param,Pz);
	glPopMatrix();

	oblik_TR(3,0,1);
	oblik_OR(7,0,3);
	oblik_IR(12,1,4);
	oblik_LR(0,1,4);	
	oblik_ZR(12,0,1);	
	oblik_YR(13,0,13);
	oblik_XR(6,0,10);

	glutSwapBuffers();
	}

static void on_reshape(int width,int height)
	{
	window_width=width;
	window_height=height;
	}
	
static void on_keyboard(unsigned char key,int x,int y)
{
	switch(key)
	{
		case 9:
			if(!animation_on){
				glutTimerFunc(TIMER_INTERVAL,on_timer,TIMER_ID);
				animation_on=1;}
			break;
		case 32:
			if(animation_on==1)	
				animation_on=0;
			break;
		case 'r':
			Px=7;
			Py=15;
			Pz=7;
			brZ=0;
			brX=0;
			animation_on=0;
			animation_param=0;
			break;
		case 'a':
			if(animation_on)
				brZ++;
			break;
		case 'd':
			if(animation_on)
				brZ--;
			break;
		case 's':
			if(animation_on)
				brX--;
			break;
		case 'w':
			if(animation_on)
				brX++;
			break;
		case 'j':
			if(animation_on)
				Px++;
			break;
		case 'i':
			if(animation_on)
				Pz++;
			break;
		case 'l':
			if(animation_on)
				Px--;
			break;
		case 'k':
			if(animation_on)
				Pz--;
			break;
	    	case 27:
			exit(EXIT_SUCCESS);
			break;
	}
}

static void on_timer(int value)
{
	if(value!=TIMER_ID)
		return ;	
	glutPostRedisplay();
	if(animation_param<Py)
		animation_param=animation_param+0.01;
	else
		animation_param=0;	
	if(animation_on){
		glutTimerFunc(TIMER_INTERVAL,on_timer,TIMER_ID);
	}
}

//detalj... postolje...
void postolje()
{
	int i,j;
	GLfloat postoljeDiffuse[]={0.3,0.3,0.3,1};
	GLfloat postoljeAmbient[]={0.1,0.1,0.1,1};
	GLfloat postoljeSpecular[]={0.9,0.9,0.9,1};
	
	glMaterialfv(GL_FRONT,GL_DIFFUSE,postoljeDiffuse);
	glMaterialfv(GL_FRONT,GL_AMBIENT,postoljeAmbient);
	glMaterialfv(GL_FRONT,GL_SPECULAR,postoljeSpecular);
	glMaterialf(GL_FRONT,GL_SHININESS,shininess);
	
	for(i=0;i<SIRINA;i++)
		{
		for(j=0;j<DUZINA;j++)
			{
			glBegin(GL_POLYGON);
			glNormal3f(0,1,0);
			glVertex3f(i-0.5,-0.5,j-0.5);
			glVertex3f(i-0.5,-0.5,j+0.5);
			glVertex3f(i+0.5,-0.5,j+0.5);
			glVertex3f(i+0.5,-0.5,j-0.5);
			glEnd();

			glColor3f(0,0,0);
			glBegin(GL_LINES);
			glVertex3f(i-0.5,-0.5,j-0.5);
			glVertex3f(i-0.5,-0.5,j+0.5);
			glVertex3f(i+0.5,-0.5,j+0.5);
			glVertex3f(i+0.5,-0.5,j-0.5);
			glEnd();
			}
		}
}

/* 	//deo koda za oivicenje kocke
	
	glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,Bmaterial_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,Bmaterial_ambient);
	glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,Bmaterial_specular);
	
	glPushMatrix();
	glTranslatef(x,y,z);
	glutWireCube(1);	
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(x+1,y,z);	
	glutWireCube(1);	
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(x-1,y,z);	
	glutWireCube(1);	
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(x,y+1,z);	
	glutWireCube(1);	
	glPopMatrix();
*/

/*
void oblik_T(int x,int y,int z)
{	
	glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,Tmaterial_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,Tmaterial_ambient);
	glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,Tmaterial_specular);
	glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shininess);
	
	glPushMatrix();
	glTranslatef(x,y,z);
	glutSolidCube(1);	
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(x+1,y,z);	
	glutSolidCube(1);	
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(x-1,y,z);	
	glutSolidCube(1);	
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(x,y+1,z);	
	glutSolidCube(1);	
	glPopMatrix();
}

void oblik_O(int x,int y,int z)
{
	glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,Omaterial_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,Omaterial_ambient);
	glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,Omaterial_specular);
	glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shininess);
	
	glPushMatrix();
	glTranslatef(x,y,z);
	glutSolidCube(1);
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(x+1,y,z);	
	glutSolidCube(1);	
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(x+1,y+1,z);
	glutSolidCube(1);	
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(x,y+1,z);
	glutSolidCube(1);
	glPopMatrix();
}

void oblik_I(int x,int y,int z)
{
	glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,Imaterial_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,Imaterial_ambient);
	glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,Imaterial_specular);
	glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shininess);
	
	glPushMatrix();
	glTranslatef(x,y,z);
	glutSolidCube(1);
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(x,y+1,z);	
	glutSolidCube(1);	
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(x,y-1,z);	
	glutSolidCube(1);	
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(x,y+2,z);	
	glutSolidCube(1);	
	glPopMatrix();
}

void oblik_L(int x,int y,int z)
{
	glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,Lmaterial_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,Lmaterial_ambient);
	glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,Lmaterial_specular);
	glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shininess);

	glPushMatrix();
	glTranslatef(x,y,z);
	glutSolidCube(1);
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(x,y-1,z);	
	glutSolidCube(1);	
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(x,y+1,z);	
	glutSolidCube(1);
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(x+1,y+1,z);
	glutSolidCube(1);
	glPopMatrix();
}

void oblik_Z(int x,int y,int z)
{
	glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,Zmaterial_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,Zmaterial_ambient);
	glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,Zmaterial_specular);
	glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shininess);

	glPushMatrix();
	glTranslatef(x,y,z);
	glutSolidCube(1);
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(x,y+1,z);	
	glutSolidCube(1);	
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(x+1,y+1,z);
	glutSolidCube(1);	
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(x-1,y,z);
	glutSolidCube(1);	
	glPopMatrix();	
}

void oblik_Y(int x,int y,int z)
{
	glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,Ymaterial_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,Ymaterial_ambient);
	glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,Ymaterial_specular);
	glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shininess);

	glPushMatrix();
	glTranslatef(x,y,z);
	glutSolidCube(1);
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(x-1,y,z);	
	glutSolidCube(1);	
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(x,y,z-1);
	glutSolidCube(1);	
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(x,y+1,z);
	glutSolidCube(1);	
	glPopMatrix();
}

void oblik_X(int x,int y,int z)
{
	glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,Xmaterial_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,Xmaterial_ambient);
	glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,Xmaterial_specular);
	glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shininess);

	glPushMatrix();
	glTranslatef(x,y,z);
	glutSolidCube(1);
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(x-1,y,z);	
	glutSolidCube(1);	
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(x,y+1,z);
	glutSolidCube(1);	
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(x,y+1,z-1);
	glutSolidCube(1);	
	glPopMatrix();
}
*/

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


    //funkcije uzimaju za argumente pozicije na kojima se iscrtava oblik
    //one u sebi pozivaju po cetiri funkcije "iscrtaj kocku" i tako nastaje oblik    
void iscrtaj_kocku(float x,float y,float z,char c)
{
	if(c=='o'){
		glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,Omaterial_diffuse);
		glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,Omaterial_ambient);
		glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,Omaterial_specular);
		glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shininess);}
	else if(c=='i'){
		glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,Imaterial_diffuse);
		glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,Imaterial_ambient);
		glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,Imaterial_specular);
		glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shininess);}
	else if(c=='l'){
		glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,Lmaterial_diffuse);
		glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,Lmaterial_ambient);
		glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,Lmaterial_specular);
		glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shininess);}
	else if(c=='z'){
		glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,Zmaterial_diffuse);
		glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,Zmaterial_ambient);
		glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,Zmaterial_specular);
		glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shininess);}
	else if(c=='t'){
		glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,Tmaterial_diffuse);
		glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,Tmaterial_ambient);
		glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,Tmaterial_specular);
		glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shininess);}
	else if(c=='y'){
		glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,Ymaterial_diffuse);
		glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,Ymaterial_ambient);
		glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,Ymaterial_specular);
		glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shininess);}
	else if(c=='x'){
		glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,Xmaterial_diffuse);
		glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,Xmaterial_ambient);
		glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,Xmaterial_specular);
		glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shininess);}
	else{
		glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,Bmaterial_diffuse);
		glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,Bmaterial_ambient);
		glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,Bmaterial_specular);
		glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shininess);
		}
			
	glBegin(GL_POLYGON);
	glNormal3f(0,0,-1);
	glVertex3f(x-0.5,y+0.5,z-0.5);
	glVertex3f(x-0.5,y-0.5,z-0.5);	
	glVertex3f(x+0.5,y-0.5,z-0.5);
	glVertex3f(x+0.5,y+0.5,z-0.5);
	glEnd();
	
	glBegin(GL_POLYGON);
	glNormal3f(0,0,1);
	glVertex3f(x-0.5,y+0.5,z+0.5);
	glVertex3f(x-0.5,y-0.5,z+0.5);	
	glVertex3f(x+0.5,y-0.5,z+0.5);
	glVertex3f(x+0.5,y+0.5,z+0.5);
	glEnd();
	
	glBegin(GL_POLYGON);
	glNormal3f(0,1,0);
	glVertex3f(x-0.5,y+0.5,z-0.5);
	glVertex3f(x-0.5,y+0.5,z+0.5);	
	glVertex3f(x+0.5,y+0.5,z+0.5);
	glVertex3f(x+0.5,y+0.5,z-0.5);
	glEnd();
	
	glBegin(GL_POLYGON);
	glNormal3f(0,-1,0);
	glVertex3f(x-0.5,y-0.5,z-0.5);
	glVertex3f(x-0.5,y-0.5,z+0.5);	
	glVertex3f(x+0.5,y-0.5,z+0.5);
	glVertex3f(x+0.5,y-0.5,z-0.5);
	glEnd();
	
	glBegin(GL_POLYGON);
	glNormal3f(1,0,0);
	glVertex3f(x+0.5,y+0.5,z+0.5);
	glVertex3f(x+0.5,y+0.5,z-0.5);	
	glVertex3f(x+0.5,y-0.5,z-0.5);
	glVertex3f(x+0.5,y-0.5,z+0.5);
	glEnd();
	
	glBegin(GL_POLYGON);
	glNormal3f(-1,0,0);
	glVertex3f(x-0.5,y+0.5,z+0.5);
	glVertex3f(x-0.5,y+0.5,z-0.5);	
	glVertex3f(x-0.5,y-0.5,z-0.5);
	glVertex3f(x-0.5,y-0.5,z+0.5);
	glEnd();
        
	//a sad ivice

	glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,Bmaterial_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,Bmaterial_ambient);
	glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,Bmaterial_specular);
	
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

void oblik_TR(float x,float y,float z)
{
	iscrtaj_kocku(x+1,y,z,'t');
	iscrtaj_kocku(x,y,z,'t');
	iscrtaj_kocku(x-1,y,z,'t');
	iscrtaj_kocku(x,y+1,z,'t');
}
void oblik_OR(float x,float y,float z)
{
	iscrtaj_kocku(x,y,z,'o');
	iscrtaj_kocku(x+1,y,z,'o');
	iscrtaj_kocku(x,y+1,z,'o');
	iscrtaj_kocku(x+1,y+1,z,'o');
}
void oblik_IR(float x,float y,float z)
{
	iscrtaj_kocku(x,y,z,'i');
	iscrtaj_kocku(x,y+1,z,'i');
	iscrtaj_kocku(x,y+2,z,'i');
	iscrtaj_kocku(x,y+3,z,'i');
}
void oblik_LR(float x,float y,float z)
{
	iscrtaj_kocku(x,y,z,'l');
	iscrtaj_kocku(x,y+1,z,'l');
	iscrtaj_kocku(x,y+2,z,'l');
	iscrtaj_kocku(x+1,y+2,z,'l');
}
void oblik_ZR(float x,float y,float z)
{
	iscrtaj_kocku(x,y,z,'z');
	iscrtaj_kocku(x,y+1,z,'z');
	iscrtaj_kocku(x+1,y+1,z,'z');
	iscrtaj_kocku(x-1,y,z,'z');
}
void oblik_YR(float x,float y,float z)
{
	iscrtaj_kocku(x,y,z,'y');
	iscrtaj_kocku(x-1,y,z,'y');
	iscrtaj_kocku(x,y+1,z,'y');
	iscrtaj_kocku(x,y,z-1,'y');
}
void oblik_XR(float x,float y,float z)
{
	iscrtaj_kocku(x,y,z,'x');
	iscrtaj_kocku(x-1,y,z,'x');
	iscrtaj_kocku(x,y+1,z,'x');
	iscrtaj_kocku(x,y+1,z-1,'x');
}

