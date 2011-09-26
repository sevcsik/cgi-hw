//=============================================================================================
// Szamitogepes grafika hazi feladat keret. Ervenyes 2011-tol.
// A //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// sorokon beluli reszben celszeru garazdalkodni, mert a tobbit ugyis toroljuk.
// A beadott program csak ebben a fajlban lehet, a fajl 1 byte-os ASCII karaktereket tartalmazhat.
// Tilos:
// - mast "beincludolni", illetve mas konyvtarat hasznalni
// - faljmuveleteket vegezni (printf is fajlmuvelet!)
// - new operatort hivni
// - felesleges programsorokat a beadott programban hagyni
// - tovabbi kommenteket a beadott programba irni a forrasmegjelolest kommentjeit kiveve
// ---------------------------------------------------------------------------------------------
// A feladatot ANSI C++ nyelvu forditoprogrammal ellenorizzuk, a Visual Studio-hoz kepesti elteresekrol
// es a leggyakoribb hibakrol (pl. ideiglenes objektumot nem lehet referencia tipusnak ertekul adni)
// a hazibeado portal ad egy osszefoglalot.
// ---------------------------------------------------------------------------------------------
// A feladatmegoldasokban csak olyan gl/glu/glut fuggvenyek hasznalhatok, amelyek
// 1. Az oran a feladatkiadasig elhangzottak ES (logikai AND muvelet)
// 2. Az alabbi listaban szerepelnek:
// Rendering pass: glBegin, glVertex[2|3]f, glColor3f, glNormal3f, glTexCoord2f, glEnd, glDrawPixels
// Transzformaciok: glViewport, glMatrixMode, glLoadIdentity, glMultMatrixf, gluOrtho2D,
// glTranslatef, glRotatef, glScalef, gluLookAt, gluPerspective, glPushMatrix, glPopMatrix,
// Illuminacio: glMaterialfv, glMaterialfv, glMaterialf, glLightfv
// Texturazas: glGenTextures, glBindTexture, glTexParameteri, glTexImage2D, glTexEnvi,
// Pipeline vezerles: glShadeModel, glEnable/Disable a kovetkezokre:
// GL_LIGHTING, GL_NORMALIZE, GL_DEPTH_TEST, GL_CULL_FACE, GL_TEXTURE_2D, GL_BLEND, GL_LIGHT[0..7]
//
// NYILATKOZAT
// ---------------------------------------------------------------------------------------------
// Nev    : Sevcsik Andras
// Neptun : RR9P7J
// ---------------------------------------------------------------------------------------------
// ezennel kijelentem, hogy a feladatot magam keszitettem, es ha barmilyen segitseget igenybe vettem vagy
// mas szellemi termeket felhasznaltam, akkor a forrast es az atvett reszt kommentekben egyertelmuen jeloltem.
// A forrasmegjeloles kotelme vonatkozik az eloadas foliakat es a targy oktatoi, illetve a
// grafhazi doktor tanacsait kiveve barmilyen csatornan (szoban, irasban, Interneten, stb.) erkezo minden egyeb
// informaciora (keplet, program, algoritmus, stb.). Kijelentem, hogy a forrasmegjelolessel atvett reszeket is ertem,
// azok helyessegere matematikai bizonyitast tudok adni. Tisztaban vagyok azzal, hogy az atvett reszek nem szamitanak
// a sajat kontribucioba, igy a feladat elfogadasarol a tobbi resz mennyisege es minosege alapjan szuletik dontes.
// Tudomasul veszem, hogy a forrasmegjeloles kotelmenek megsertese eseten a hazifeladatra adhato pontokat
// negativ elojellel szamoljak el es ezzel parhuzamosan eljaras is indul velem szemben.
//=============================================================================================

#include <math.h>
#include <stdlib.h>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
// MsWindows-on ez is kell
#include <windows.h>
#endif // Win32 platform

#include <GL/gl.h>
#include <GL/glu.h>
// A GLUT-ot le kell tolteni: http://www.opengl.org/resources/libraries/glut/
#include <GL/glut.h>

#define new new_nelkul_is_meg_lehet_csinalni

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Innentol modosithatod...

#define COLOR(X) X.r, X.g, X.b
#define COORD(X) X.x, X.y

#define GREEN_BODY_COLOR Color(0.0f, 0.5f, 0.0f)
#define GREEN_HEAD_COLOR Color(1.0f, 1.0f, 0.0f)
#define RED_BODY_COLOR Color(0.5f, 0.0f, 0.0f)
#define RED_HEAD_COLOR Color(1.0f, 1.0f, 0.0f)
#define WORM_SPEED 0.1f
#define WORM_LENGTH 0.4f
#define WORM_HEIGHT 0.05f
#define WORM_SEGMENTS 100
#define WORM_BUMPS 5
#define HEAD_HEIGHT 0.025f
#define HEAD_WIDTH 0.05f

enum WormType { GREEN, RED };

struct Color
{
  float r, g, b;
  Color(): r(0), g(0), b(0) {}
  Color(float r, float g, float b): r(r), g(g), b(b) {}
};

struct Vect
{
  float x;
  float y;
  Vect(float x, float y): x(x), y(y) {}
  Vect():x(0), y(0) {}
  Vect operator+(Vect b) { return Vect(x + b.x, y + b.y); }
  Vect operator-(Vect b) { return Vect(x - b.x, y - b.y); }
  float getAbs() { return sqrt(x * x + y * y); }
  float operator*(Vect b) { return x * b.x + y * b.y; }
  Vect operator*(float f) { return Vect(x * f, y * f); }
};

struct Coord
{
  float x;
  float y;
  Coord(float x, float y):x(x), y(y) {}
  Coord(): x(0), y(0) {}
  Coord operator+(Vect b) { return Coord(x + b.x, y + b.y); }
  Coord operator+=(Vect b) { x += b.x; y += b.y; return *this; }
  Vect operator-(Coord b) { return Vect(x - b.x, y - b.y); }
};

struct Worm
{
  WormType type;
  Color bodyColor;
  Color headColor;
  Coord tail;
  Coord head;
  Vect v;
  long updated;
  float dir;
  Worm(): type(RED) {}
  Worm(WormType type, Coord tail, float dir):
    type(type), tail(tail), dir(dir), updated(0)
  {
    head = tail + Vect(dir * WORM_LENGTH, 0);
    v = Vect(dir * WORM_SPEED, 0);
    if (type == GREEN)
    {
      bodyColor = GREEN_BODY_COLOR;
      headColor = GREEN_HEAD_COLOR;
    }
    else
    {
      bodyColor = RED_BODY_COLOR;
      headColor = RED_HEAD_COLOR;
    }
  }
  void draw()
  {
    float length = (head - tail).getAbs();
    glShadeModel(GL_FLAT);

    glBegin(GL_LINE_STRIP);
      glColor3f(COLOR(bodyColor));

      for (float x = tail.x;
           dir > 0 ? x < head.x : x > head.x;
           x += dir * (length / WORM_SEGMENTS))
      {
        glVertex2f(x,
            (WORM_HEIGHT / 2.0f) *
            sin((x / length) *
            (WORM_BUMPS * 2) * 3.1415f) + tail.y);
      }
    glEnd();

    glBegin(GL_QUADS);
      glColor3f(COLOR(headColor));
      glVertex2f(head.x - (HEAD_WIDTH / 2), head.y);
      glVertex2f(head.x, head.y + (HEAD_HEIGHT / 2));
      glVertex2f(head.x + (HEAD_WIDTH / 2), head.y);
      glVertex2f(head.x, head.y - (HEAD_HEIGHT / 2));
    glEnd();
  }
  void move(long time)
  {
    float seconds = (float) (time - updated) / 1000.0f;
    if (dir > 0 ?
        (head + v * (seconds)).x < 1.0f :
        (head + v * (seconds)).x > -1.0f)
    {
      tail += v * (seconds);
      head += v * (seconds);
    }
    else
    {
      dir = -dir;
      v = v * -1.0f;
      Coord temp = head;
      head = tail;
      tail = temp;
    }
    updated = time;
  }
};

Worm redworm;
Worm greenworm;

// Inicializacio, a program futasanak kezdeten, az OpenGL kontextus letrehozasa utan hivodik meg (ld. main() fv.)
void onInitialization( ) {
  redworm = Worm(RED, Coord(0.2, 0.2), 1);
  greenworm = Worm(GREEN, Coord(0.2, 0), -1);
}

// Rajzolas, ha az alkalmazas ablak ervenytelenne valik, akkor ez a fuggveny hivodik meg
void onDisplay( ) {
  glClearColor(0, 0, 0, 1.0);		// torlesi szin beallitasa
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // kepernyo torles

  redworm.draw();
  greenworm.draw();

  glutSwapBuffers();     				// Buffercsere: rajzolas vege

}

// Billentyuzet esemenyeket lekezelo fuggveny
void onKeyboard(unsigned char key, int x, int y) {
    if (key == 'd') glutPostRedisplay( ); 		// d beture rajzold ujra a kepet

}

// Eger esemenyeket lekezelo fuggveny
void onMouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT && state == GLUT_DOWN);  // A GLUT_LEFT_BUTTON / GLUT_RIGHT_BUTTON illetve GLUT_DOWN / GLUT_UP
}

// `Idle' esemenykezelo, jelzi, hogy az ido telik, az Idle esemenyek frekvenciajara csak a 0 a garantalt minimalis ertek
void onIdle( ) {
  long time = glutGet(GLUT_ELAPSED_TIME);		// program inditasa ota eltelt ido
  greenworm.move(time);
  redworm.move(time);
  glutPostRedisplay();
}

// ...Idaig modosithatod
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// A C++ program belepesi pontja, a main fuggvenyt mar nem szabad bantani
int main(int argc, char **argv) {
    glutInit(&argc, argv); 				// GLUT inicializalasa
    glutInitWindowSize(600, 600);			// Alkalmazas ablak kezdeti merete 600x600 pixel
    glutInitWindowPosition(100, 100);			// Az elozo alkalmazas ablakhoz kepest hol tunik fel
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);	// 8 bites R,G,B,A + dupla buffer + melyseg buffer

    glutCreateWindow("Grafika hazi feladat");		// Alkalmazas ablak megszuletik es megjelenik a kepernyon

    glMatrixMode(GL_MODELVIEW);				// A MODELVIEW transzformaciot egysegmatrixra inicializaljuk
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);			// A PROJECTION transzformaciot egysegmatrixra inicializaljuk
    glLoadIdentity();

    onInitialization();					// Az altalad irt inicializalast lefuttatjuk

    glutDisplayFunc(onDisplay);				// Esemenykezelok regisztralasa
    glutMouseFunc(onMouse);
    glutIdleFunc(onIdle);
    glutKeyboardFunc(onKeyboard);

    glutMainLoop();					// Esemenykezelo hurok

    return 0;
}

