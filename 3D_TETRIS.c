#include <GL/glut.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

#define SIRINA 16
#define VISINA 16
#define DUZINA 16
#define CENTAR 7
#define TIMER_INTERVAL 10
#define TIMER_ID 0
#define BROJ_OBLIKA 7
#define COORD_CENTR_SIZE 9 //pamtimo po 3 koordinate za 3 kocke , za cetvrtu imamo vec , ona je centar mase oblika

//matrica 16*16*16 opisuje trodimenzionalni prostor podeljen na kocke 
//koje popunjavamo padajucim predmetima koji su takodje sastavljeni od kocki ...
//potrebne su nam tri informacije:
//1. da li je polje u matrici popunjeno kockom koja pripada nekom od predmeta 
//2. da li je ta kocka deo staticnog predmeta ili pada 
//3. boja kocke

typedef struct Polje{
bool popunjen;
bool pokretan;
int boja;
}polje;

polje prostorIgranja[SIRINA][VISINA][DUZINA];

static int window_width,window_height;
int animation_on=0;
float animation_param=0;
float  brzina_pada=0.01;
int token;              //oznaka pseudo slucajnog oblika

//pozicija improvizovanog centra mase oblika koji pada

int Px=CENTAR;
int Py=VISINA-1;
int Pz=CENTAR;

//broj rotacija oko Z i X osa

int brZ=0;
int brX=0;

bool blokNapred=false;
bool blokNazad=false;
bool blokLevo=false;
bool blokDesno=false;

//boje za svaki oblik

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
int izracunaj_token();
void iscrtaj_staticni_deo();
void proveri_blokove();
void proveri_blok(int i,int j,int k);

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
void iscrtaj_kocku(float x,float y,float z,int c);

void rotacijaZlevo();
void rotacijaZdesno();
void rotacijaXdole();
void rotacijaXgore();

void translacijaLevo();
void translacijaDesno();
void translacijaNapred();
void translacijaNazad();

float pokretni_deo[COORD_CENTR_SIZE];

void(*niz_pokazivaca_funkcija[])(float,float,float)=
	{oblik_TR,
	oblik_OR,
	oblik_IR,
	oblik_LR,
	oblik_ZR,
	oblik_YR,
	oblik_XR};

int main(int argc, char **argv)
{
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);

	glutInitWindowSize(1000,700);
	glutInitWindowPosition(0,0);
	glutCreateWindow(argv[0]);

	inicijalizuj_matricu();
	token=izracunaj_token();	

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

	proveri_blokove();
	iscrtaj_staticni_deo();

	glPushMatrix();
	
		glTranslatef(Px,Py-animation_param,Pz);
		glRotatef( brZ *90 , 0, 0, -1);
		glRotatef( brX *90 , 1, 0, 0);
		glTranslatef(-Px,-Py+animation_param,-Pz);
	
		(*niz_pokazivaca_funkcija[token])(Px,Py-animation_param,Pz);
	
	glPopMatrix();	

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
		//tab
		case 9:
			if(!animation_on){
				glutTimerFunc(TIMER_INTERVAL,on_timer,TIMER_ID);
				animation_on=1;}
			break;
		//space
		case 32:
			if(animation_on==1)	
				animation_on=0;
			break;
		case 'r':
			Px=CENTAR;
			Py=VISINA-1;
			Pz=CENTAR;
			brZ=0;
			brX=0;
			animation_on=0;
			animation_param=0;
			token=izracunaj_token();
			inicijalizuj_matricu();
			break;
		case 'a':
			if(animation_on){
				rotacijaZlevo();}
			break;
		case 'd':
			if(animation_on){
				rotacijaZdesno();}
			break;
		case 's':
			if(animation_on){
				rotacijaXdole();}
			break;
		case 'w':
			if(animation_on){
				rotacijaXgore();}
			break;
		case 'j':
			if(!blokLevo && animation_on){
				translacijaLevo();
				}
			break;
		case 'i':
			if(!blokNapred && animation_on){
				translacijaNapred();
				}
			break;
		case 'l':
			if(!blokDesno && animation_on){
				translacijaDesno();
				}
			break;
		case 'k':
			if(!blokNazad && animation_on){
				translacijaNazad();
				}
			break;
		case 'f':
			if(animation_on)
				brzina_pada=0.1;
			break;
		//esc	    	
		case 27:
			exit(EXIT_SUCCESS);
			break;
	}
}

static void on_timer(int value)
{
	if(value!=TIMER_ID)
		return ;
		
	bool indikator;
	if(Py-animation_param>0 && pokretni_deo[1]>0 && pokretni_deo[4]>0 && pokretni_deo[7]>0){

		if((Py-animation_param)-floor((Py-animation_param))==0 && animation_param!=0)
			{
				animation_on=0;
				int x1=(int)pokretni_deo[0];
				int y1=(int)pokretni_deo[1];
				int z1=(int)pokretni_deo[2];
	
				int x2=(int)pokretni_deo[3];
				int y2=(int)pokretni_deo[4];
				int z2=(int)pokretni_deo[5];	
	
				int x3=(int)pokretni_deo[6];
				int y3=(int)pokretni_deo[7];
				int z3=(int)pokretni_deo[8];
				int y4=(int)(Py-animation_param);
				
				if((prostorIgranja[x1][y1-1][z1].popunjen && (!prostorIgranja[x1][y1-1][z1].pokretan))
				|| (prostorIgranja[x2][y2-1][z2].popunjen && (!prostorIgranja[x2][y2-1][z2].pokretan))
				|| (prostorIgranja[x3][y3-1][z3].popunjen && (!prostorIgranja[x3][y3-1][z3].pokretan))
				|| (prostorIgranja[Px][y4-1][Pz].popunjen 
				&& (!prostorIgranja[Px][y4-1][Pz].pokretan)))
				{
					prostorIgranja[x1][y1][z1].popunjen=true;
					prostorIgranja[x1][y1][z1].pokretan=false;
					prostorIgranja[x1][y1][z1].boja=token;
					prostorIgranja[x2][y2][z2].popunjen=true;
					prostorIgranja[x2][y2][z2].pokretan=false;
					prostorIgranja[x1][y2][z1].boja=token;
					prostorIgranja[x3][y3][z3].popunjen=true;
					prostorIgranja[x3][y3][z3].pokretan=false;
					prostorIgranja[x1][y3][z1].boja=token;
					prostorIgranja[Px][y4][Pz].popunjen=true;
					prostorIgranja[Px][y4][Pz].pokretan=false;
					prostorIgranja[Px][y4][Pz].boja=token;
					indikator=true;								
				}
			}
		else {
			indikator=false;
			animation_param = animation_param + brzina_pada;
			pokretni_deo[1]=pokretni_deo[1]-brzina_pada;
			pokretni_deo[4]=pokretni_deo[4]-brzina_pada;
			pokretni_deo[7]=pokretni_deo[7]-brzina_pada;
			}
	}
	else{
		animation_on=0;
		int x1=(int)pokretni_deo[0];
		int y1=(int)pokretni_deo[1];
		int z1=(int)pokretni_deo[2];
	
		int x2=(int)pokretni_deo[3];
		int y2=(int)pokretni_deo[4];
		int z2=(int)pokretni_deo[5];	
	
		int x3=(int)pokretni_deo[6];
		int y3=(int)pokretni_deo[7];
		int z3=(int)pokretni_deo[8];
		int y4=(int)(Py-animation_param);		

		prostorIgranja[x1][y1][z1].popunjen=true;
		prostorIgranja[x1][y1][z1].pokretan=false;
		prostorIgranja[x1][y1][z1].boja=token;
		prostorIgranja[x2][y2][z2].popunjen=true;
		prostorIgranja[x2][y2][z2].pokretan=false;
		prostorIgranja[x1][y2][z1].boja=token;
		prostorIgranja[x3][y3][z3].popunjen=true;
		prostorIgranja[x3][y3][z3].pokretan=false;
		prostorIgranja[x1][y3][z1].boja=token;
		prostorIgranja[Px][y4][Pz].popunjen=true;
		prostorIgranja[Px][y4][Pz].pokretan=false;
		prostorIgranja[Px][y4][Pz].boja=token;
		indikator=true;		
	}
	
	glutPostRedisplay();
	
	if(indikator){
		animation_param=0;
		brzina_pada=0.01;
		Px=CENTAR;
		Pz=CENTAR;
		token=izracunaj_token();
		animation_on=1;
		}	
	
	if(animation_on){
		glutTimerFunc(TIMER_INTERVAL,on_timer,TIMER_ID);
	}
}

int izracunaj_token(){
	srand(time(NULL));
	int r=rand()%BROJ_OBLIKA;
	
	if(r==0)
		{
			pokretni_deo[0]=Px+1;
			pokretni_deo[1]=Py;
			pokretni_deo[2]=Pz;
			pokretni_deo[3]=Px-1;
			pokretni_deo[4]=Py;
			pokretni_deo[5]=Pz;
			pokretni_deo[6]=Px;
			pokretni_deo[7]=Py+1;
			pokretni_deo[8]=Pz;
		}
	else if(r==1)
		{
			pokretni_deo[0]=Px+1;
			pokretni_deo[1]=Py;
			pokretni_deo[2]=Pz;
			pokretni_deo[3]=Px;
			pokretni_deo[4]=Py+1;
			pokretni_deo[5]=Pz;
			pokretni_deo[6]=Px+1;
			pokretni_deo[7]=Py+1;
			pokretni_deo[8]=Pz;
		}
	else if(r==2)
		{
			pokretni_deo[0]=Px;
			pokretni_deo[1]=Py+1;
			pokretni_deo[2]=Pz;
			pokretni_deo[3]=Px;
			pokretni_deo[4]=Py-1;
			pokretni_deo[5]=Pz;
			pokretni_deo[6]=Px;
			pokretni_deo[7]=Py+2;
			pokretni_deo[8]=Pz;
		}
	else if(r==3)
		{
			pokretni_deo[0]=Px;
			pokretni_deo[1]=Py-1;
			pokretni_deo[2]=Pz;
			pokretni_deo[3]=Px;
			pokretni_deo[4]=Py+1;
			pokretni_deo[5]=Pz;
			pokretni_deo[6]=Px+1;
			pokretni_deo[7]=Py+1;
			pokretni_deo[8]=Pz;
		}
	else if(r==4)
		{
			pokretni_deo[0]=Px;
			pokretni_deo[1]=Py+1;
			pokretni_deo[2]=Pz;
			pokretni_deo[3]=Px+1;
			pokretni_deo[4]=Py+1;
			pokretni_deo[5]=Pz;
			pokretni_deo[6]=Px-1;
			pokretni_deo[7]=Py;
			pokretni_deo[8]=Pz;
		}
	else if(r==5)
		{
			pokretni_deo[0]=Px-1;
			pokretni_deo[1]=Py;
			pokretni_deo[2]=Pz;
			pokretni_deo[3]=Px;
			pokretni_deo[4]=Py+1;
			pokretni_deo[5]=Pz;
			pokretni_deo[6]=Px;
			pokretni_deo[7]=Py;
			pokretni_deo[8]=Pz-1;
		}
	else 
		{
			pokretni_deo[0]=Px-1;
			pokretni_deo[1]=Py;
			pokretni_deo[2]=Pz;
			pokretni_deo[3]=Px;
			pokretni_deo[4]=Py+1;
			pokretni_deo[5]=Pz;
			pokretni_deo[6]=Px;
			pokretni_deo[7]=Py+1;
			pokretni_deo[8]=Pz-1;
		}

	return r;
}

void iscrtaj_staticni_deo(void)
{
	int i,j,k;
	for(i=0;i<SIRINA;i++)
	{
		for(j=0;j<VISINA;j++)
		{
			for(k=0;k<DUZINA;k++)
			{
				if(prostorIgranja[i][j][k].popunjen==true 
			   	&& prostorIgranja[i][j][k].pokretan==false)
					iscrtaj_kocku( i, j, k, prostorIgranja[i][j][k].boja);	
			}
		}
	}
}

void proveri_blokove(void){
int i,j,k;
	for(i=0;i<SIRINA-3;i++)
	{
		for(j=0;j<VISINA-3;j++)
		{
			for(k=0;k<DUZINA-3;k++)
			{
				proveri_blok(i,j,k);						
			}
		}
	}
}

void proveri_blok(int i,int j,int k){
int m,n,l;
	for(m=i;m<i+4;m++)
	{
		for(n=j;n<j+4;n++)
		{
			for(l=k;l<k+4;l++)
			{
				if(prostorIgranja[m][n][l].popunjen==false)
					return;
			}
		}
	}
	
	for(m=i;m<i+4;m++)
	{
		for(n=j;n<VISINA;n++)
		{
			for(l=k;l<k+4;l++)
			{
				if(n < VISINA-4)
					prostorIgranja[m][n][l].popunjen=prostorIgranja[m][n+4][l].popunjen;
				else
					prostorIgranja[m][n][l].popunjen=false;
			}
		}
	}
	return;
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
			}
		}
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

//provera ispravnosti rotacije

void rotacijaZlevo(void){

	int p1;                      //pomocna promenljiva
	int x1=(int)pokretni_deo[0];
	float y1=pokretni_deo[1];
	int z1=(int)pokretni_deo[2];
	
	int x2=(int)pokretni_deo[3];
	float y2=pokretni_deo[4];
	int z2=(int)pokretni_deo[5];	
	
	int x3=(int)pokretni_deo[6];
	float y3=pokretni_deo[7];
	int z3=(int)pokretni_deo[8];
	float y4=Py-animation_param;
	
	if(x1-Px>0)
	{
		if((y1-y4) >0){
			p1=x1;
			x1=Px+y1-y4;
			y1=y4-p1+Px;
		}
		else if((y1-y4)==0){
			p1=y1;			
			y1=y4-x1+Px;
			x1=Px;		
		}
		else{
			p1=x1;
			x1=Px+y1-y4;
			y1=y4;
		}
	}
	else if(x1-Px==0)
	{
		if(y4 > y1){
			p1=x1;
			x1=Px-y4+y1;
			y1=y4;
		}
		else{
			p1=x1;
			x1=Px+y1-y4;
			y1=y4;
		}
	}
	else
	{
		if(y4 > y1){
			p1=x1;
			x1=Px+y1-y4;
			y1=y4+Px-p1;
		}
		else if(y4 == y1){
			p1=y1;
			y1=Px-x1+y4;
			x1=Px;
		}
		else{
			p1=y1;
			y1=y4+Px-x1;
			x1=Px+p1-y4;
		}
	}

	//za drugu

	if(x2-Px>0)
	{
		if((y2-y4) >0){
			p1=x2;
			x2=Px+y2-y4;
			y2=y4-p1+Px;
		}
		else if((y2-y4)==0){
			p1=y2;			
			y2=y4-x2+Px;
			x2=Px;		
		}
		else{
			p1=x2;
			x2=Px+y2-y4;
			y2=y4;
		}
	}
	else if(x2-Px==0)
	{
		if(y4 > y2){
			p1=x2;
			x2=Px-y4+y2;
			y2=y4;
		}
		else{
			p1=x2;
			x2=Px+y2-y4;
			y2=y4;
		}
	}
	else
	{
		if(y4 > y2){
			p1=x2;
			x2=Px+y2-y4;
			y2=y4+Px-p1;
		}
		else if(y4 == y2){
			p1=y2;
			y2=Px-x2+y4;
			x2=Px;
		}
		else{
			p1=y2;
			y2=y4+Px-x2;
			x2=Px+p1-y4;
		}
	}

	//za trecu
	
	if(x3-Px>0)
	{
		if((y3-y4) >0){
			p1=x3;
			x3=Px+y3-y4;
			y3=y4-p1+Px;
		}
		else if((y3-y4)==0){
			p1=y3;			
			y3=y4-x3+Px;
			x3=Px;		
		}
		else{
			p1=x3;
			x3=Px+y3-y4;
			y3=y4;
		}
	}
	else if(x3-Px==0)
	{
		if(y4 > y3){
			p1=x3;
			x3=Px-y4+y3;
			y3=y4;
		}
		else{
			p1=x3;
			x3=Px+y3-y4;
			y3=y4;
		}
	}
	else
	{
		if(y4 > y3){
			p1=x3;
			x3=Px+y3-y4;
			y3=y4+Px-p1;
		}
		else if(y4 == y3){
			p1=y3;
			y3=Px-x3+y4;
			x3=Px;
		}
		else{
			p1=y3;
			y3=y4+Px-x3;
			x3=Px+p1-y4;
		}
	}

	brZ++;
	if(x1<0 || x2<0 || x3<0 || y1<0 || y2<0 || y3<0 || x1>=SIRINA || x2>=SIRINA || x3>=SIRINA || 
	(prostorIgranja[x1][(int)floor(y1)][z1].popunjen && (!prostorIgranja[x1][(int)floor(y1)][z1].pokretan))
	|| (prostorIgranja[x1][(int)ceil(y1)][z1].popunjen && (!prostorIgranja[x1][(int)ceil(y1)][z1].pokretan))
	|| (prostorIgranja[x2][(int)floor(y2)][z2].popunjen && (!prostorIgranja[x2][(int)floor(y2)][z2].pokretan))
	|| (prostorIgranja[x2][(int)ceil(y2)][z2].popunjen && (!prostorIgranja[x2][(int)ceil(y2)][z2].pokretan))
	|| (prostorIgranja[x3][(int)floor(y3)][z3].popunjen && (!prostorIgranja[x3][(int)floor(y3)][z3].pokretan))
	|| (prostorIgranja[x3][(int)ceil(y3)][z3].popunjen && (!prostorIgranja[x3][(int)ceil(y3)][z3].pokretan)))	
		brZ--;		
}

void rotacijaZdesno(void){

	int p1;
	int x1=(int)pokretni_deo[0];
	float y1=pokretni_deo[1];
	int z1=(int)pokretni_deo[2];
	
	int x2=(int)pokretni_deo[3];
	float y2=pokretni_deo[4];
	int z2=(int)pokretni_deo[5];	
	
	int x3=(int)pokretni_deo[6];
	float y3=pokretni_deo[7];
	int z3=(int)pokretni_deo[8];
	float y4=Py-animation_param;
	
	if(x1-Px>0)
	{
		if((y1-y4) >0){
			p1=x1;
			x1=Px-y1+y4;
			y1=y4+p1-Px;
		}
		else if((y1-y4)==0){
			p1=y1;			
			y1=y4+x1-Px;
			x1=Px;		
		}
		else{
			p1=x1;
			x1=Px+y4-y1;
			y1=p1-Px+y4;
		}
	}
	else if(x1-Px==0)
	{
		if(y4 > y1){
			p1=x1;
			x1=Px+y4-y1;
			y1=y4;
		}
		else{
			p1=x1;
			x1=Px-y1+y4;
			y1=y4;
		}
	}
	else
	{
		if(y4 > y1){
			p1=y1;
			y1=x1-Px+y4;
			x1=y4-p1+Px;
		}
		else if(y4 == y1){
			p1=y1;
			y1=y4-Px+x1;
			x1=Px;
		}
		else{
			p1=x1;
			x1=Px-y1+y4;
			y1=y4-Px+p1;
		}
	}

	//za drugu

	if(x2-Px>0)
	{
		if((y2-y4) >0){
			p1=x2;
			x2=Px-y2+y4;
			y2=y4+p1-Px;
		}
		else if((y2-y4)==0){
			p1=y2;			
			y2=y4+x2-Px;
			x2=Px;		
		}
		else{
			p1=x2;
			x2=Px+y4-y2;
			y2=p1-Px+y4;
		}
	}
	else if((x2-Px)==0)
	{
		if(y4 > y2){
			p1=x2;
			x2=Px+y4-y2;
			y2=y4;
		}
		else{
			p1=x2;
			x2=Px-y2+y4;
			y2=y4;
		}
	}
	else
	{
		if(y4 > y2){
			p1=y2;
			y2=x2-Px+y4;
			x2=y4-p1+Px;
		}
		else if(y4 == y2){
			p1=y2;
			y2=y4-Px+x2;
			x2=Px;
		}
		else{
			p1=x2;
			x2=Px+y4-y2;
			y2=y4-Px+p1;
		}
	}

	//za trecu

	if(x3-Px>0)
	{
		if((y3-y4) >0){
			p1=x3;
			x3=Px-y3+y4;
			y3=y4+p1-Px;
		}
		else if((y3-y4)==0){
			p1=y3;			
			y3=y4+x3-Px;
			x3=Px;		
		}
		else{
			p1=x3;
			x3=Px+y4-y3;
			y3=p1-Px+y4;
		}
	}
	else if(x3-Px==0)
	{
		if(y4 > y3){
			p1=x3;
			x3=Px+y4-y3;
			y3=y4;
		}
		else{
			p1=x3;
			x3=Px-y3+y4;
			y3=y4;
		}
	}
	else
	{
		if(y4 > y3){
			p1=y3;
			y3=x3-Px+y4;
			x3=y4-p1+Px;
		}
		else if(y4 == y3){
			p1=y3;
			y3=y4-Px+x3;
			x3=Px;
		}
		else{
			p1=x3;
			x3=Px+y4-y3;
			y3=y4-Px+p1;
		}
	}

	brZ--;
	if( x1<0 || x2<0 || x3<0 || y1<0 || y2<0 || y3<0 || x1>=SIRINA || x2>=SIRINA || x3>=SIRINA ||
	(prostorIgranja[x1][(int)floor(y1)][z1].popunjen && (!prostorIgranja[x1][(int)floor(y1)][z1].pokretan))
	|| (prostorIgranja[x1][(int)ceil(y1)][z1].popunjen && (!prostorIgranja[x1][(int)ceil(y1)][z1].pokretan))
	|| (prostorIgranja[x2][(int)floor(y2)][z2].popunjen && (!prostorIgranja[x2][(int)floor(y2)][z2].pokretan))
	|| (prostorIgranja[x2][(int)ceil(y2)][z2].popunjen && (!prostorIgranja[x2][(int)ceil(y2)][z2].pokretan))
	|| (prostorIgranja[x3][(int)floor(y3)][z3].popunjen && (!prostorIgranja[x3][(int)floor(y3)][z3].pokretan))
	|| (prostorIgranja[x3][(int)ceil(y3)][z3].popunjen && (!prostorIgranja[x3][(int)ceil(y3)][z3].pokretan)))	
		brZ++;
}

void rotacijaXdole(void){
	
	int p1;
	int x1=(int)pokretni_deo[0];
	float y1=pokretni_deo[1];
	int z1=(int)pokretni_deo[2];
	
	int x2=(int)pokretni_deo[3];
	float y2=pokretni_deo[4];
	int z2=(int)pokretni_deo[5];	
	
	int x3=(int)pokretni_deo[6];
	float y3=pokretni_deo[7];
	int z3=(int)pokretni_deo[8];
	float y4=Py-animation_param;

	if(z1-Pz>0)
	{
		if((y1-y4) >0){
			p1=z1;
			z1=Pz-y1+y4;
			y1=y4+p1-Pz;
		}
		else if((y1-y4)==0){
			p1=y1;			
			y1=y4+z1-Pz;
			z1=Pz;		
		}
		else{
			p1=z1;
			z1=Pz+y4-y1;
			y1=p1-Pz+y4;
		}
	}
	else if(z1-Pz==0)
	{
		if(y4 > y1){
			p1=z1;
			z1=Pz+y4-y1;
			y1=y4;
		}
		else{
			p1=z1;
			z1=Pz-y1+y4;
			y1=y4;
		}
	}
	else
	{
		if(y4 > y1){
			p1=y1;
			y1=z1-Pz+y4;
			z1=y4-p1+Pz;
		}
		else if(y4 == y1){
			p1=y1;
			y1=y4-Pz+z1;
			x1=Px;
		}
		else{
			p1=z1;
			z1=Pz+y4-y1;
			y1=y4-Pz+p1;
		}
	}

	//za drugu

	if(z2-Pz>0)
	{
		if((y2-y4) >0){
			p1=z2;
			z2=Pz-y2+y4;
			y2=y4+p1-Pz;
		}
		else if((y2-y4)==0){
			p1=y2;			
			y2=y4+z2-Pz;
			z2=Pz;		
		}
		else{
			p1=z2;
			z2=Pz+y4-y2;
			y2=p1-Pz+y4;
		}
	}
	else if(z2-Pz==0)
	{
		if(y4 > y2){
			p1=z2;
			z2=Pz+y4-y2;
			y2=y4;
		}
		else{
			p1=z2;
			z2=Pz-y2+y4;
			y2=y4;
		}
	}
	else
	{
		if(y4 > y2){
			p1=y2;
			y2=z2-Pz+y4;
			z2=y4-p1+Pz;
		}
		else if(y4 == y2){
			p1=y2;
			y2=y4-Pz+z2;
			z2=Pz;
		}
		else{
			p1=z2;
			z2=Pz+y4-y2;
			y2=y4-Pz+p1;
		}
	}

	//za trecu

	if(z3-Pz>0)
	{
		if((y3-y4) >0){
			p1=z3;
			z3=Pz-y3+y4;
			y3=y4+p1-Pz;
		}
		else if((y3-y4)==0){
			p1=y3;			
			y3=y4+z3-Pz;
			z3=Pz;		
		}
		else{
			p1=z3;
			z3=Pz+y4-y3;
			y3=p1-Pz+y4;
		}
	}
	else if(z3-Pz==0)
	{
		if(y4 > y3){
			p1=z3;
			z3=Pz+y4-y3;
			y3=y4;
		}
		else{
			p1=z3;
			z3=Pz-y3+y4;
			y3=y4;
		}
	}
	else
	{
		if(y4 > y3){
			p1=y3;
			y3=z3-Pz+y4;
			z3=y4-p1+Pz;
		}
		else if(y4 == y3){
			p1=y3;
			y3=y4-Pz+z3;
			z3=Pz;
		}
		else{
			p1=z3;
			z3=Pz+y4-y3;
			y3=y4-Pz+p1;
		}
	}

	brX--;
	if( z1<0 || z2<0 || z3<0 || y1<0 || y2<0 || y3<0 || z1>=DUZINA || z2>=DUZINA || z3>=DUZINA ||
	(prostorIgranja[x1][(int)floor(y1)][z1].popunjen && (!prostorIgranja[x1][(int)floor(y1)][z1].pokretan))
	|| (prostorIgranja[x1][(int)ceil(y1)][z1].popunjen && (!prostorIgranja[x1][(int)ceil(y1)][z1].pokretan))
	|| (prostorIgranja[x2][(int)floor(y2)][z2].popunjen && (!prostorIgranja[x2][(int)floor(y2)][z2].pokretan))
	|| (prostorIgranja[x2][(int)ceil(y2)][z2].popunjen && (!prostorIgranja[x2][(int)ceil(y2)][z2].pokretan))
	|| (prostorIgranja[x3][(int)floor(y3)][z3].popunjen && (!prostorIgranja[x3][(int)floor(y3)][z3].pokretan))
	|| (prostorIgranja[x3][(int)ceil(y3)][z3].popunjen && (!prostorIgranja[x3][(int)ceil(y3)][z3].pokretan)))	
		brX++;
}

void rotacijaXgore(void){
	
	int p1;
	int x1=(int)pokretni_deo[0];
	float y1=pokretni_deo[1];
	int z1=(int)pokretni_deo[2];
	
	int x2=(int)pokretni_deo[3];
	float y2=pokretni_deo[4];
	int z2=(int)pokretni_deo[5];	
	
	int x3=(int)pokretni_deo[6];
	float y3=pokretni_deo[7];
	int z3=(int)pokretni_deo[8];
	float y4=Py-animation_param;	

	if(z1-Pz>0)
	{
		if((y1-y4) >0){
			p1=z1;
			z1=Pz+y1-y4;
			y1=y4-p1+Pz;
		}
		else if((y1-y4)==0){
			p1=y1;			
			y1=y4-z1+Pz;
			z1=Pz;		
		}
		else{
			p1=z1;
			z1=Pz+y1-y4;
			y1=y4;
		}
	}
	else if(z1-Pz==0)
	{
		if(y4 > y1){
			p1=z1;
			z1=Pz-y4+y1;
			y1=y4;
		}
		else{
			p1=z1;
			z1=Pz+y1-y4;
			y1=y4;
		}
	}
	else
	{
		if(y4 > y1){
			p1=z1;
			z1=Pz+y1-y4;
			y1=y4+Pz-p1;
		}
		else if(y4 == y1){
			p1=y1;
			y1=Pz-z1+y4;
			z1=Pz;
		}
		else{
			p1=y1;
			y1=y4+Pz-z1;
			z1=Pz+p1-y4;
		}
	}

	//za drugu

	if(z2-Pz>0)
	{
		if((y2-y4) >0){
			p1=z2;
			z2=Pz+y2-y4;
			y2=y4-p1+Pz;
		}
		else if((y2-y4)==0){
			p1=y2;			
			y2=y4-z2+Pz;
			z2=Pz;		
		}
		else{
			p1=z2;
			z2=Pz+y2-y4;
			y2=y4;
		}
	}
	else if(z2-Pz==0)
	{
		if(y4 > y2){
			p1=z2;
			z2=Pz-y4+y2;
			y2=y4;
		}
		else{
			p1=z2;
			z2=Pz+y2-y4;
			y2=y4;
		}
	}
	else
	{
		if(y4 > y2){
			p1=z2;
			z2=Pz+y2-y4;
			y2=y4+Pz-p1;
		}
		else if(y4 == y2){
			p1=y2;
			y2=Pz-z2+y4;
			z2=Pz;
		}
		else{
			p1=y2;
			y2=y4+Pz-z2;
			z2=Pz+p1-y4;
		}
	}

	//za trecu

	if(z3-Pz>0)
	{
		if((y3-y4) >0){
			p1=z3;
			z3=Pz+y3-y4;
			y3=y4-p1+Pz;
		}
		else if((y3-y4)==0){
			p1=y3;			
			y3=y4-z3+Pz;
			z3=Pz;		
		}
		else{
			p1=z3;
			z3=Pz+y3-y4;
			y3=y4;
		}
	}
	else if(z3-Pz==0)
	{
		if(y4 > y3){
			p1=z3;
			z3=Pz-y4+y3;
			y3=y4;
		}
		else{
			p1=z3;
			z3=Pz+y3-y4;
			y3=y4;
		}
	}
	else
	{
		if(y4 > y3){
			p1=z3;
			z3=Pz+y3-y4;
			y3=y4+Pz-p1;
		}
		else if(y4 == y3){
			p1=y3;
			y3=Pz-z3+y4;
			z3=Pz;
		}
		else{
			p1=y3;
			y3=y4+Pz-z3;
			z3=Pz+p1-y4;
		}
	}
	
	brX++;
	if(z1<0 || z2<0 || z3<0	|| y1<0 || y2<0 || y3<0 || z1>=DUZINA || z2>=DUZINA || z3>=DUZINA ||
	(prostorIgranja[x1][(int)floor(y1)][z1].popunjen && (!prostorIgranja[x1][(int)floor(y1)][z1].pokretan))
	|| (prostorIgranja[x1][(int)ceil(y1)][z1].popunjen && (!prostorIgranja[x1][(int)ceil(y1)][z1].pokretan))
	|| (prostorIgranja[x2][(int)floor(y2)][z2].popunjen && (!prostorIgranja[x2][(int)floor(y2)][z2].pokretan))
	|| (prostorIgranja[x2][(int)ceil(y2)][z2].popunjen && (!prostorIgranja[x2][(int)ceil(y2)][z2].pokretan))
	|| (prostorIgranja[x3][(int)floor(y3)][z3].popunjen && (!prostorIgranja[x3][(int)floor(y3)][z3].pokretan))
	|| (prostorIgranja[x3][(int)ceil(y3)][z3].popunjen && (!prostorIgranja[x3][(int)ceil(y3)][z3].pokretan)))	
		brX--;
}

void translacijaLevo(void){

	int x1=(int)pokretni_deo[0];
	float y1=pokretni_deo[1];
	int z1=(int)pokretni_deo[2];
	
	int x2=(int)pokretni_deo[3];
	float y2=pokretni_deo[4];
	int z2=(int)pokretni_deo[5];	
	
	int x3=(int)pokretni_deo[6];
	float y3=pokretni_deo[7];
	int z3=(int)pokretni_deo[8];
	
	if(Px>=SIRINA || x1>=SIRINA || x2>=SIRINA || x3>=SIRINA ||
	(prostorIgranja[x1+1][(int)floor(y1)][z1].popunjen && (!prostorIgranja[x1+1][(int)floor(y1)][z1].pokretan))
	|| (prostorIgranja[x1+1][(int)ceil(y1)][z1].popunjen && (!prostorIgranja[x1+1][(int)ceil(y1)][z1].pokretan))
	|| (prostorIgranja[x2+1][(int)floor(y2)][z2].popunjen && (!prostorIgranja[x2+1][(int)floor(y2)][z2].pokretan))
	|| (prostorIgranja[x2+1][(int)ceil(y2)][z2].popunjen && (!prostorIgranja[x2+1][(int)ceil(y2)][z2].pokretan))
	|| (prostorIgranja[x3+1][(int)floor(y3)][z3].popunjen && (!prostorIgranja[x3+1][(int)floor(y3)][z3].pokretan))
	|| (prostorIgranja[x3+1][(int)ceil(y3)][z3].popunjen && (!prostorIgranja[x3+1][(int)ceil(y3)][z3].pokretan))
	|| (prostorIgranja[Px][(int)floor(Py-animation_param)][Pz].popunjen 
	&& (!prostorIgranja[Px][(int)floor(Py-animation_param)][Pz].pokretan))
	|| (prostorIgranja[Px][(int)ceil(Py-animation_param)][Pz].popunjen 
	&& (!prostorIgranja[Px][(int)ceil(Py-animation_param)][Pz].pokretan)))
		blokLevo=true;
	else{
		Px++;
		pokretni_deo[0]=x1-1;
		pokretni_deo[3]=x2-1;
		pokretni_deo[6]=x3-1;
		blokDesno=false;
		}
}

void translacijaDesno(void){

	int x1=(int)pokretni_deo[0];
	float y1=pokretni_deo[1];
	int z1=(int)pokretni_deo[2];
	
	int x2=(int)pokretni_deo[3];
	float y2=pokretni_deo[4];
	int z2=(int)pokretni_deo[5];	
	
	int x3=(int)pokretni_deo[6];
	float y3=pokretni_deo[7];
	int z3=(int)pokretni_deo[8];	
		
	if(Px<0 || x1<0 || x2<0 || x3<0 ||
	(prostorIgranja[x1-1][(int)floor(y1)][z1].popunjen && (!prostorIgranja[x1-1][(int)floor(y1)][z1].pokretan))
	|| (prostorIgranja[x1-1][(int)ceil(y1)][z1].popunjen && (!prostorIgranja[x1-1][(int)ceil(y1)][z1].pokretan))
	|| (prostorIgranja[x2-1][(int)floor(y2)][z2].popunjen && (!prostorIgranja[x2-1][(int)floor(y2)][z2].pokretan))
	|| (prostorIgranja[x2-1][(int)ceil(y2)][z2].popunjen && (!prostorIgranja[x2-1][(int)ceil(y2)][z2].pokretan))
	|| (prostorIgranja[x3-1][(int)floor(y3)][z3].popunjen && (!prostorIgranja[x3-1][(int)floor(y3)][z3].pokretan))
	|| (prostorIgranja[x3-1][(int)ceil(y3)][z3].popunjen && (!prostorIgranja[x3-1][(int)ceil(y3)][z3].pokretan))
	|| (prostorIgranja[Px][(int)floor(Py-animation_param)][Pz].popunjen 
	&& (!prostorIgranja[Px][(int)floor(Py-animation_param)][Pz].pokretan))
	|| (prostorIgranja[Px][(int)ceil(Py-animation_param)][Pz].popunjen 
	&& (!prostorIgranja[Px][(int)ceil(Py-animation_param)][Pz].pokretan)))
		blokDesno=true;
	else{
		Px--;
		pokretni_deo[0]=x1+1;
		pokretni_deo[3]=x2+1;
		pokretni_deo[6]=x3+1;
		blokLevo=false;
		}	
}
	
void translacijaNapred(void){

	int x1=(int)pokretni_deo[0];
	float y1=pokretni_deo[1];
	int z1=(int)pokretni_deo[2];
	
	int x2=(int)pokretni_deo[3];
	float y2=pokretni_deo[4];
	int z2=(int)pokretni_deo[5];	
	
	int x3=(int)pokretni_deo[6];
	float y3=pokretni_deo[7];
	int z3=(int)pokretni_deo[8];	
	
	if(Pz>=DUZINA || z1>=DUZINA || z2>=DUZINA || z3>=DUZINA ||
	 (prostorIgranja[x1][(int)floor(y1)][z1+1].popunjen && (!prostorIgranja[x1][(int)floor(y1)][z1+1].pokretan))
	|| (prostorIgranja[x1][(int)ceil(y1)][z1+1].popunjen && (!prostorIgranja[x1][(int)ceil(y1)][z1+1].pokretan))
	|| (prostorIgranja[x2][(int)floor(y2)][z2+1].popunjen && (!prostorIgranja[x2][(int)floor(y2)][z2+1].pokretan))
	|| (prostorIgranja[x2][(int)ceil(y2)][z2+1].popunjen && (!prostorIgranja[x2][(int)ceil(y2)][z2+1].pokretan))
	|| (prostorIgranja[x3][(int)floor(y3)][z3+1].popunjen && (!prostorIgranja[x3][(int)floor(y3)][z3+1].pokretan))
	|| (prostorIgranja[x3][(int)ceil(y3)][z3+1].popunjen && (!prostorIgranja[x3][(int)ceil(y3)][z3+1].pokretan))
	|| (prostorIgranja[Px][(int)floor(Py-animation_param)][Pz].popunjen 
	&& (!prostorIgranja[Px][(int)floor(Py-animation_param)][Pz].pokretan))
	|| (prostorIgranja[Px][(int)ceil(Py-animation_param)][Pz].popunjen 
	&& (!prostorIgranja[Px][(int)ceil(Py-animation_param)][Pz].pokretan)))
		blokNapred=true;
	else{
		Pz++;
		pokretni_deo[2]=z1-1;
		pokretni_deo[5]=z2-1;
		pokretni_deo[8]=z3-1;
		blokNazad=false;
		}	
}

void translacijaNazad(void){

	int x1=(int)pokretni_deo[0];
	float y1=pokretni_deo[1];
	int z1=(int)pokretni_deo[2];
	
	int x2=(int)pokretni_deo[3];
	float y2=pokretni_deo[4];
	int z2=(int)pokretni_deo[5];	
	
	int x3=(int)pokretni_deo[6];
	float y3=pokretni_deo[7];
	int z3=(int)pokretni_deo[8];	
	
	if(Pz<0 || z1<0 || z2<0 || z3<0 || 
	(prostorIgranja[x1][(int)floor(y1)][z1-1].popunjen && (!prostorIgranja[x1][(int)floor(y1)][z1-1].pokretan))
	|| (prostorIgranja[x1][(int)ceil(y1)][z1-1].popunjen && (!prostorIgranja[x1][(int)ceil(y1)][z1-1].pokretan))
	|| (prostorIgranja[x2][(int)floor(y2)][z2-1].popunjen && (!prostorIgranja[x2][(int)floor(y2)][z2-1].pokretan))
	|| (prostorIgranja[x2][(int)ceil(y2)][z2-1].popunjen && (!prostorIgranja[x2][(int)ceil(y2)][z2-1].pokretan))
	|| (prostorIgranja[x3][(int)floor(y3)][z3-1].popunjen && (!prostorIgranja[x3][(int)floor(y3)][z3-1].pokretan))
	|| (prostorIgranja[x3][(int)ceil(y3)][z3-1].popunjen && (!prostorIgranja[x3][(int)ceil(y3)][z3-1].pokretan))
	|| (prostorIgranja[Px][(int)floor(Py-animation_param)][Pz].popunjen 
	&& (!prostorIgranja[Px][(int)floor(Py-animation_param)][Pz].pokretan))
	|| (prostorIgranja[Px][(int)ceil(Py-animation_param)][Pz].popunjen 
	&& (!prostorIgranja[Px][(int)ceil(Py-animation_param)][Pz].pokretan)))
		blokNazad=true;
	else{
		Pz--;
		pokretni_deo[2]=z1+1;
		pokretni_deo[5]=z2+1;
		pokretni_deo[8]=z3+1;
		blokNapred=false;
		}
}

    //funkcije uzimaju za argumente pozicije na kojima se iscrtava oblik
    //one u sebi pozivaju po cetiri funkcije "iscrtaj kocku" i tako nastaje oblik    

void iscrtaj_kocku(float x,float y,float z,int c)
{
	if(c==1){
		glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,Omaterial_diffuse);
		glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,Omaterial_ambient);
		glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,Omaterial_specular);
		glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shininess);}
	else if(c==2){
		glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,Imaterial_diffuse);
		glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,Imaterial_ambient);
		glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,Imaterial_specular);
		glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shininess);}
	else if(c==3){
		glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,Lmaterial_diffuse);
		glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,Lmaterial_ambient);
		glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,Lmaterial_specular);
		glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shininess);}
	else if(c==4){
		glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,Zmaterial_diffuse);
		glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,Zmaterial_ambient);
		glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,Zmaterial_specular);
		glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shininess);}
	else if(c==0){
		glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,Tmaterial_diffuse);
		glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,Tmaterial_ambient);
		glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,Tmaterial_specular);
		glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shininess);}
	else if(c==5){
		glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,Ymaterial_diffuse);
		glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,Ymaterial_ambient);
		glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,Ymaterial_specular);
		glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shininess);}
	else if(c==6){
		glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,Xmaterial_diffuse);
		glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,Xmaterial_ambient);
		glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,Xmaterial_specular);
		glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shininess);}
				
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
	iscrtaj_kocku(x+1,y,z,0);
	iscrtaj_kocku(x,y,z,0);
	iscrtaj_kocku(x-1,y,z,0);
	iscrtaj_kocku(x,y+1,z,0);
}

void oblik_OR(float x,float y,float z)
{
	iscrtaj_kocku(x,y,z,1);
	iscrtaj_kocku(x+1,y,z,1);
	iscrtaj_kocku(x,y+1,z,1);
	iscrtaj_kocku(x+1,y+1,z,1);
}

void oblik_IR(float x,float y,float z)
{
	iscrtaj_kocku(x,y,z,2);
	iscrtaj_kocku(x,y-1,z,2);
	iscrtaj_kocku(x,y+1,z,2);
	iscrtaj_kocku(x,y+2,z,2);
}

void oblik_LR(float x,float y,float z)
{
	iscrtaj_kocku(x,y-1,z,3);
	iscrtaj_kocku(x,y,z,3);
	iscrtaj_kocku(x,y+1,z,3);
	iscrtaj_kocku(x+1,y+1,z,3);
}

void oblik_ZR(float x,float y,float z)
{
	iscrtaj_kocku(x,y,z,4);
	iscrtaj_kocku(x,y+1,z,4);
	iscrtaj_kocku(x+1,y+1,z,4);
	iscrtaj_kocku(x-1,y,z,4);
}

void oblik_YR(float x,float y,float z)
{
	iscrtaj_kocku(x,y,z,5);
	iscrtaj_kocku(x-1,y,z,5);
	iscrtaj_kocku(x,y+1,z,5);
	iscrtaj_kocku(x,y,z-1,5);
}

void oblik_XR(float x,float y,float z)
{
	iscrtaj_kocku(x,y,z,6);
	iscrtaj_kocku(x-1,y,z,6);
	iscrtaj_kocku(x,y+1,z,6);
	iscrtaj_kocku(x,y+1,z-1,6);
}

/*
	//alternativni nacin iscrtavanja kocke i oblika 	
	//deo koda za oivicenje kocke
	
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
