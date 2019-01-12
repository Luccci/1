#include <GL/glut.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <stdio.h>

#define SIRINA 12
#define VISINA 15
#define DUZINA 12
#define VISINA_POCETKA_PADA 13
#define CENTAR 5
#define TIMER_INTERVAL 10
#define TIMER_ID 0
#define BROJ_OBLIKA 7
#define COORD_CENTR_SIZE 9 //pamtimo po 3 koordinate za 3 kocke , za cetvrtu imamo vec , ona je centar mase oblika

//matrica 12*15*12 opisuje trodimenzionalni prostor podeljen na kocke 
//koje popunjavamo padajucim predmetima koji su takodje sastavljeni od kocki ...
//potrebne su nam dve informacije:
//1. da li je polje u matrici popunjeno kockom koja pripada nekom od palih predmeta  
//2. boja kocke

typedef struct Polje{
bool popunjen;
int boja;
}polje;

//koordinate matrice predstavljaju centar u kome se iscrtava kocka
polje prostorIgranja[SIRINA][VISINA][DUZINA];

static int window_width,window_height;
int animation_on=0;
bool brzo=false;
bool kraj=false;
float animation_param=0;
float  brzina_pada_normalno=0.01;
float brzina_pada_ubrzano=0.1;
float brzina_pada=0.01;
int brTimer=1;
int brTimerFast=10;
int brTimerCheck=100;
int token;              //oznaka pseudo slucajnog oblika

//pozicija improvizovanog centra mase oblika koji pada
int Px=CENTAR;
int Py=VISINA_POCETKA_PADA;
int Pz=CENTAR;

//broj rotacija oko Z i X osa
int brZ=0;
int brX=0;
//boje i pozicija svetla

GLfloat light_position[]={1,1,1,0};
GLfloat light_diffuse[]={0.7, 0.7, 0.7, 1};
GLfloat light_ambient[]={0.5, 0.5, 0.5, 1};
GLfloat light_specular[]={0.9, 0.9, 0.9, 1};

GLfloat shininess=30;

//boje za svaki oblik
	
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
void provera_kraja();

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

//niz u kome cuvamo koordinate pokretnog oblika za ostale 3 kocke po 3 koordinate 
float pokretni_deo[COORD_CENTR_SIZE];

//niz pokazivaca funkcija na osnovu ostatka pri deljenju pseudo slcajnog broja bira se oblik
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

	//deo koda za iscrtavanje 
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
				animation_on=1;
				glutTimerFunc(TIMER_INTERVAL,on_timer,TIMER_ID);}
			break;
		//space
		case 32:
			if(animation_on==1)	
				animation_on=0;
			break;
		case 'r':
			Px=CENTAR;
			Pz=CENTAR;
			brZ=0;
			brX=0;
			animation_on=0;
			animation_param=0;
			kraj=false;
			token=izracunaj_token();
			inicijalizuj_matricu();
			glutPostRedisplay();
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
			if(animation_on){
				translacijaLevo();
				}
			break;
		case 'i':
			if(animation_on){
				translacijaNapred();
				}
			break;
		case 'l':
			if(animation_on){
				translacijaDesno();
				}
			break;
		case 'k':
			if(animation_on){
				translacijaNazad();
				}
			break;
		case 'f':
			if(animation_on)
				brzo=true;
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

	float centarY=Py-animation_param;
	
	int x1=(int)pokretni_deo[0];
	int y1=(int)pokretni_deo[1];
	int z1=(int)pokretni_deo[2];
	
	int x2=(int)pokretni_deo[3];
	int y2=(int)pokretni_deo[4];
	int z2=(int)pokretni_deo[5];	

	int x3=(int)pokretni_deo[6];
	int y3=(int)pokretni_deo[7];
	int z3=(int)pokretni_deo[8];
	int y4=(int)(centarY);
	printf("\n%d %d %d",y1,y2,y3);		

	/*else if(brzo){
			//azuriramo nove koordinate
			brTimer=brTimer+brTimerFast;
			brzina_pada=brzina_pada_ubrzano;
			animation_param = animation_param + brzina_pada;
			pokretni_deo[1]=pokretni_deo[1]-brzina_pada;
			pokretni_deo[4]=pokretni_deo[4]-brzina_pada;
			pokretni_deo[7]=pokretni_deo[7]-brzina_pada;
			}*/

	bool indikator=false;
	if( y1>0 && y2>0 && y3>0 && y4>0)
	{	
		if( (brTimer % brTimerCheck) == 0 )
			{	
				if(prostorIgranja[x1][y1-1][z1].popunjen
				|| prostorIgranja[x2][y2-1][z2].popunjen
				|| prostorIgranja[x3][y3-1][z3].popunjen
				|| prostorIgranja[Px][y4-1][Pz].popunjen)
				{
					prostorIgranja[x1][y1][z1].popunjen=true;
					prostorIgranja[x1][y1][z1].boja=token;
					prostorIgranja[x2][y2][z2].popunjen=true;
					prostorIgranja[x1][y2][z1].boja=token;
					prostorIgranja[x3][y3][z3].popunjen=true;
					prostorIgranja[x1][y3][z1].boja=token;
					prostorIgranja[Px][y4][Pz].popunjen=true;
					prostorIgranja[Px][y4][Pz].boja=token;
					indikator=true;
					brTimer=1;								
				}
				else
					brTimer++;
			}
		else
			{
			animation_param = animation_param + brzina_pada;
			pokretni_deo[1]=pokretni_deo[1]-brzina_pada;
			pokretni_deo[4]=pokretni_deo[4]-brzina_pada;
			pokretni_deo[7]=pokretni_deo[7]-brzina_pada;
			brTimer++;
			}
	}
	
	else if((y1==0 || y2==0 || y3==0 || y4==0))
	{
		//ukoliko je oblik pao na postolje postaje statican deo			
		prostorIgranja[x1][y1][z1].popunjen=true;
		prostorIgranja[x1][y1][z1].boja=token;
		prostorIgranja[x2][y2][z2].popunjen=true;
		prostorIgranja[x1][y2][z1].boja=token;
		prostorIgranja[x3][y3][z3].popunjen=true;
		prostorIgranja[x1][y3][z1].boja=token;
		prostorIgranja[Px][y4][Pz].popunjen=true;
		prostorIgranja[Px][y4][Pz].boja=token;
		indikator=true;
		brTimer=1;		
	}
	
	glutPostRedisplay();
	
	provera_kraja();
	if(kraj)
		animation_on=0;	
	
	if(indikator){
		animation_param=0;
		brzina_pada=brzina_pada_normalno;
		Px=CENTAR;
		Pz=CENTAR;
		brX=0;
		brZ=0;
		brzo=false;
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
	
	//biramo sledeci oblik i postavljamo koordinate za ostale 3 kocke u zavisnosti koji oblik je odabran
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

//prilikom svakog poziva funkcije on_display prvo iscrtavamo staticni deo o kome
//imamo informacije na osnovu matrice prostorIgranja
void iscrtaj_staticni_deo(void)
{
	int i,j,k;
	for(i=0;i<SIRINA;i++)
	{
		for(j=0;j<VISINA;j++)
		{
			for(k=0;k<DUZINA;k++)
			{
				if(prostorIgranja[i][j][k].popunjen==true)
					iscrtaj_kocku( i, j, k, prostorIgranja[i][j][k].boja);	
			}
		}
	}
}

void proveri_blokove(void){
int i,j,k;
//ne proveravamo blokove koje predstavljaju poslednje 2 koordinate u svakoj osi jer ne mogu popuniti 3*3*3
	for(i=0;i<SIRINA-2;i++)
	{
		for(j=0;j<VISINA-2;j++)
		{
			for(k=0;k<DUZINA-2;k++)
			{
				proveri_blok(i,j,k);						
			}
		}
	}
}

void proveri_blok(int i,int j,int k){
int m,n,l;
	//svaki blok je jednoznacno odredjen jednom koordinatom matrice(onom sa najmanjim koordinatama)
	for(m=i;m<i+3;m++)
	{
		for(n=j;n<j+3;n++)
		{
			for(l=k;l<k+3;l++)
			{
				if(prostorIgranja[m][n][l].popunjen==false)
					return;
			}
		}
	}
	//ukoliko je popunjen transliramo sve sto je iznad za 3 kocke na dole
	for(m=i;m<i+3;m++)
	{
		for(n=j;n<VISINA;n++)
		{
			for(l=k;l<k+3;l++)
			{
				//ukoliko su u pitanju poslednje 3 kordinate po visini onda postaju prazne
				if(n < VISINA-3)
					prostorIgranja[m][n][l].popunjen=prostorIgranja[m][n+3][l].popunjen;
				else
					prostorIgranja[m][n][l].popunjen=false;
			}
		}
	}
	return;
}

void provera_kraja(void)
{
	int i,j;
	for(i=0;i<SIRINA;i++)
	{
		for(j=0;j<DUZINA;j++)
		{
			if(prostorIgranja[i][VISINA-1][j].popunjen)
				kraj=true;
		}
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
			}
		}
}

//postavljanje cele matrice na nulu 
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
			}
		}
	}
}

//provera ispravnosti rotacija , ima ih 4 , slican princip za sve 4

void rotacijaZlevo(void){

	float p1;                      //pomocna promenljiva
	float x1=pokretni_deo[0];
	float y1=pokretni_deo[1];
	float z1=pokretni_deo[2];
	
	float x2=pokretni_deo[3];
	float y2=pokretni_deo[4];
	float z2=pokretni_deo[5];	
	
	float x3=pokretni_deo[6];
	float y3=pokretni_deo[7];
	float z3=pokretni_deo[8];
	float y4=Py-animation_param;
	
	// ideja je ne racunati cos i sin vec
	// razloziti na 8 slucajeva u zavisnosti od polozaja ostale 3 kocke
	// na kocku koja predstavlja centar mase celog oblika
	//one se rotiraju oko nje za ugao od 90 stepeni

	//za prvu kocku
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

	//za drugu kocku

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

	//za trecu kocku
	
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

	if(x1<=0 || x2<=0 || x3<=0 || y1<=0 || y2<=0 || y3<=0 || x1>=SIRINA-1 || x2>=SIRINA-1 || x3>=SIRINA-1 
	|| (prostorIgranja[(int)x1][(int)floor(y1)][(int)z1].popunjen)
	|| (prostorIgranja[(int)x1][(int)ceil(y1)][(int)z1].popunjen)
	|| (prostorIgranja[(int)x2][(int)floor(y2)][(int)z2].popunjen)
	|| (prostorIgranja[(int)x2][(int)ceil(y2)][(int)z2].popunjen)
	|| (prostorIgranja[(int)x3][(int)floor(y3)][(int)z3].popunjen)
	|| (prostorIgranja[(int)x3][(int)ceil(y3)][(int)z3].popunjen))	
		return;
	else
		{
			pokretni_deo[0]=x1;
			pokretni_deo[1]=y1;
			pokretni_deo[2]=z1;
			pokretni_deo[3]=x2;
			pokretni_deo[4]=y2;
			pokretni_deo[5]=z2;
			pokretni_deo[6]=x3;
			pokretni_deo[7]=y3;
			pokretni_deo[8]=z3;
			brZ++;
		}	
	//ukoliko je rotacija validna azuriraju se nove kordinate i povecava brojac rotacija	
}

void rotacijaZdesno(void){

	float p1;
	float x1=pokretni_deo[0];
	float y1=pokretni_deo[1];
	float z1=pokretni_deo[2];
	
	float x2=pokretni_deo[3];
	float y2=pokretni_deo[4];
	float z2=pokretni_deo[5];	
	
	float x3=pokretni_deo[6];
	float y3=pokretni_deo[7];
	float z3=pokretni_deo[8];
	float y4=Py-animation_param;
	
	//za prvu kocku
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

	//za drugu kocku

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

	//za trecu kocku

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

	if( x1<=0 || x2<=0 || x3<=0 || y1<=0 || y2<=0 || y3<=0 || x1>=SIRINA-1 || x2>=SIRINA-1 || x3>=SIRINA-1
 	||(prostorIgranja[(int)x1][(int)floor(y1)][(int)z1].popunjen)
	|| (prostorIgranja[(int)x1][(int)ceil(y1)][(int)z1].popunjen)
	|| (prostorIgranja[(int)x2][(int)floor(y2)][(int)z2].popunjen)
	|| (prostorIgranja[(int)x2][(int)ceil(y2)][(int)z2].popunjen)
	|| (prostorIgranja[(int)x3][(int)floor(y3)][(int)z3].popunjen)
	|| (prostorIgranja[(int)x3][(int)ceil(y3)][(int)z3].popunjen))	
		return;
	else
		{
			pokretni_deo[0]=x1;
			pokretni_deo[1]=y1;
			pokretni_deo[2]=z1;
			pokretni_deo[3]=x2;
			pokretni_deo[4]=y2;
			pokretni_deo[5]=z2;
			pokretni_deo[6]=x3;
			pokretni_deo[7]=y3;
			pokretni_deo[8]=z3;
			brZ--;
		}
}

void rotacijaXdole(void){
	
	float p1;
	float x1=pokretni_deo[0];
	float y1=pokretni_deo[1];
	float z1=pokretni_deo[2];
	
	float x2=pokretni_deo[3];
	float y2=pokretni_deo[4];
	float z2=pokretni_deo[5];	
	
	float x3=pokretni_deo[6];
	float y3=pokretni_deo[7];
	float z3=pokretni_deo[8];
	float y4=Py-animation_param;

	//za prvu kocku
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

	//za drugu kocku

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

	//za trecu kocku

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

	if( z1<=0 || z2<=0 || z3<=0 || y1<=0 || y2<=0 || y3<=0 || z1>=DUZINA-1 || z2>=DUZINA-1 || z3>=DUZINA-1 
	|| (prostorIgranja[(int)x1][(int)floor(y1)][(int)z1].popunjen)
	|| (prostorIgranja[(int)x1][(int)ceil(y1)][(int)z1].popunjen)
	|| (prostorIgranja[(int)x2][(int)floor(y2)][(int)z2].popunjen)
	|| (prostorIgranja[(int)x2][(int)ceil(y2)][(int)z2].popunjen)
	|| (prostorIgranja[(int)x3][(int)floor(y3)][(int)z3].popunjen)
	|| (prostorIgranja[(int)x3][(int)ceil(y3)][(int)z3].popunjen))	
		return;
	else
		{
			pokretni_deo[0]=x1;
			pokretni_deo[1]=y1;
			pokretni_deo[2]=z1;
			pokretni_deo[3]=x2;
			pokretni_deo[4]=y2;
			pokretni_deo[5]=z2;
			pokretni_deo[6]=x3;
			pokretni_deo[7]=y3;
			pokretni_deo[8]=z3;
			brX--;
		}
}

void rotacijaXgore(void){
	
	float p1;
	float x1=pokretni_deo[0];
	float y1=pokretni_deo[1];
	float z1=pokretni_deo[2];
	
	float x2=pokretni_deo[3];
	float y2=pokretni_deo[4];
	float z2=pokretni_deo[5];	
	
	float x3=pokretni_deo[6];
	float y3=pokretni_deo[7];
	float z3=pokretni_deo[8];
	float y4=Py-animation_param;	

	//za prvu kocku
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

	//za drugu kocku

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

	//za trecu kocku

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
	
	if(z1<=0 || z2<=0 || z3<=0 || y1<=0 || y2<=0 || y3<=0 || z1>=DUZINA-1 || z2>=DUZINA-1 || z3>=DUZINA-1 
	|| (prostorIgranja[(int)x1][(int)floor(y1)][(int)z1].popunjen)
	|| (prostorIgranja[(int)x1][(int)ceil(y1)][(int)z1].popunjen)
	|| (prostorIgranja[(int)x2][(int)floor(y2)][(int)z2].popunjen)
	|| (prostorIgranja[(int)x2][(int)ceil(y2)][(int)z2].popunjen)
	|| (prostorIgranja[(int)x3][(int)floor(y3)][(int)z3].popunjen)
	|| (prostorIgranja[(int)x3][(int)ceil(y3)][(int)z3].popunjen))	
		return;
	else
		{
			pokretni_deo[0]=x1;
			pokretni_deo[1]=y1;
			pokretni_deo[2]=z1;
			pokretni_deo[3]=x2;
			pokretni_deo[4]=y2;
			pokretni_deo[5]=z2;
			pokretni_deo[6]=x3;
			pokretni_deo[7]=y3;
			pokretni_deo[8]=z3;
			brX++;
		}
}

//provera translacija , ima ih 4 , slican princip za sve 4
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

	//ukoliko se pokretna kocka nalazi izmedju sve staticne po visini moramo da proverimo za obe
	
	if(Px>=SIRINA-1 || x1>=SIRINA-1 || x2>=SIRINA-1 || x3>=SIRINA-1 ||
	(prostorIgranja[x1+1][(int)floor(y1)][z1].popunjen)
	|| (prostorIgranja[x1+1][(int)ceil(y1)][z1].popunjen)
	|| (prostorIgranja[x2+1][(int)floor(y2)][z2].popunjen)
	|| (prostorIgranja[x2+1][(int)ceil(y2)][z2].popunjen)
	|| (prostorIgranja[x3+1][(int)floor(y3)][z3].popunjen)
	|| (prostorIgranja[x3+1][(int)ceil(y3)][z3].popunjen)
	|| (prostorIgranja[Px+1][(int)floor(Py-animation_param)][Pz].popunjen)
	|| (prostorIgranja[Px+1][(int)ceil(Py-animation_param)][Pz].popunjen))
		return;
	else{
		Px++;
		pokretni_deo[0]=x1+1;
		pokretni_deo[3]=x2+1;
		pokretni_deo[6]=x3+1;
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
		
	if(Px<=0 || x1<=0 || x2<=0 || x3<=0 ||
	(prostorIgranja[x1-1][(int)floor(y1)][z1].popunjen)
	|| (prostorIgranja[x1-1][(int)ceil(y1)][z1].popunjen)
	|| (prostorIgranja[x2-1][(int)floor(y2)][z2].popunjen)
	|| (prostorIgranja[x2-1][(int)ceil(y2)][z2].popunjen)
	|| (prostorIgranja[x3-1][(int)floor(y3)][z3].popunjen)
	|| (prostorIgranja[x3-1][(int)ceil(y3)][z3].popunjen)
	|| (prostorIgranja[Px-1][(int)floor(Py-animation_param)][Pz].popunjen)
	|| (prostorIgranja[Px-1][(int)ceil(Py-animation_param)][Pz].popunjen))
		return;
	else{
		Px--;
		pokretni_deo[0]=x1-1;
		pokretni_deo[3]=x2-1;
		pokretni_deo[6]=x3-1;
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
	
	if(Pz>=DUZINA-1 || z1>=DUZINA-1 || z2>=DUZINA-1 || z3>=DUZINA-1 ||
	 (prostorIgranja[x1][(int)floor(y1)][z1+1].popunjen)
	|| (prostorIgranja[x1][(int)ceil(y1)][z1+1].popunjen)
	|| (prostorIgranja[x2][(int)floor(y2)][z2+1].popunjen)
	|| (prostorIgranja[x2][(int)ceil(y2)][z2+1].popunjen)
	|| (prostorIgranja[x3][(int)floor(y3)][z3+1].popunjen)
	|| (prostorIgranja[x3][(int)ceil(y3)][z3+1].popunjen)
	|| (prostorIgranja[Px][(int)floor(Py-animation_param)][Pz+1].popunjen)
	|| (prostorIgranja[Px][(int)ceil(Py-animation_param)][Pz+1].popunjen))
		return;
	else{
		Pz++;
		pokretni_deo[2]=z1+1;
		pokretni_deo[5]=z2+1;
		pokretni_deo[8]=z3+1;
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
	
	if(Pz<=0 || z1<=0 || z2<=0 || z3<=0 || 
	(prostorIgranja[x1][(int)floor(y1)][z1-1].popunjen)
	|| (prostorIgranja[x1][(int)ceil(y1)][z1-1].popunjen)
	|| (prostorIgranja[x2][(int)floor(y2)][z2-1].popunjen)
	|| (prostorIgranja[x2][(int)ceil(y2)][z2-1].popunjen)
	|| (prostorIgranja[x3][(int)floor(y3)][z3-1].popunjen)
	|| (prostorIgranja[x3][(int)ceil(y3)][z3-1].popunjen)
	|| (prostorIgranja[Px][(int)floor(Py-animation_param)][Pz-1].popunjen)
	|| (prostorIgranja[Px][(int)ceil(Py-animation_param)][Pz-1].popunjen))
		return;
	else{
		Pz--;
		pokretni_deo[2]=z1-1;
		pokretni_deo[5]=z2-1;
		pokretni_deo[8]=z3-1;
		}
}

    //funkcije za iscrtavanje oblika
    //funkcije uzimaju za argumente pozicije na kojima se iscrtava oblik
    //one u sebi pozivaju po cetiri funkcije "iscrtaj kocku" i tako nastaje oblik
    //najpre na osnovu cetvrtog argumenta saznajemo boju 

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

