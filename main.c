#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <glut.h>

#include "arm_simul.h"

/* Frame size */
int FrameW, FrameH;

double SyncTime;
int IsPause, Key_Arm = 0, Key_Shoulder = 0, Key_Shoulder2 = 0, Key_Hand = 0, Key_Hand2 = 0;

void Box( double Len, double Size )
{
  glPushMatrix();
  glScaled(Len, Size, Size);
  glTranslated(0.5, 0, 0);
  glutSolidCube(1);
  glPopMatrix();
}

void TimerResponse( void )
{
  int t;
  static int StartTime = -1, OldTime, PauseTime;

  t = clock();
  if (StartTime == -1)
    StartTime = OldTime = t;

  if (IsPause)
    PauseTime += t - OldTime;
  else
    SyncTime = (t - PauseTime + StartTime) / (double)CLOCKS_PER_SEC;
  OldTime = t;
}

void Draw( void )
{
  static double arm_ang = 0, shoulder_ang = 0, hand_ang = 0;
  TimerResponse();

  /* Clear the frame */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glLoadIdentity();

  glColor3d(0.3, 0.3, 0.3);
  //glRectd(-1, -1, 1, 1);


  glPushMatrix();
  gluLookAt(2, 3, 8, 0, 0, 0, 0, 1, 0);

  // Axes
  glBegin(GL_LINES);
    glColor3d(10.8, 0, 0);
    glVertex2d(-1000, 0);
    glVertex2d(1000, 0);
    glColor3d(0, 10, 0);
    glVertex2d(0, -1000);
    glVertex2d(0, 1000);
    glColor3d(0, 0, 10);
    glVertex3d(0, 0, -1000);
    glVertex3d(0, 0, 1000);
  glEnd();


  glRotated(30 * SyncTime, 0, 1, 0);

  glScaled(0.5, 0.5, 0.5);

  // shoulder start
  if (Key_Shoulder == 1 && shoulder_ang < 50.0)
    shoulder_ang += 3;
  else if (Key_Shoulder == 0 && shoulder_ang > 0)
    shoulder_ang -= 3;
  if (Key_Shoulder2 == 1 && shoulder_ang > -50.0)
    shoulder_ang -= 3;
  else if (Key_Shoulder2 == 0 && shoulder_ang < 0)
    shoulder_ang += 3;
  glRotated(shoulder_ang, 0, 0, 1);

  glColor3d(0.3, 0.8, 0.3);
  Box(5, 1);

  if (Key_Arm == 1 && arm_ang < 50.0)
    arm_ang += 3;
  else if (Key_Arm == 0 && arm_ang > 0)
    arm_ang -= 3;
  // arm start
  glTranslated(5, 0, 0);
  glutSolidSphere(0.8, 30, 30);
  glRotated(arm_ang, 0, 0, 1);
  glColor3d(0.3, 0.8, 0.9);
  Box(3.8, 0.98);

  // hand start
  if (Key_Hand == 1 && hand_ang < 30.0)
    hand_ang += 3;
  else if (Key_Hand == 0 && hand_ang > 0)
    hand_ang -= 3;
  if (Key_Hand2 == 1 && hand_ang > -30.0)
    hand_ang -= 3;
  else if (Key_Hand2 == 0 && hand_ang < 0)
    hand_ang += 3;

  glTranslated(3.8, 0, 0);
  glRotated(hand_ang, 0, 0, 1);
  glColor3d(0.9, 0.8, 0.9);
 
  glPushMatrix();
  glScaled(1, 0.1, 1);
  Box(1.8, 1.3);
  glPopMatrix();


  glTranslated(1.8, 0, 0);
  glutSolidTorus(0.3, 0.8, 30, 30);


  //glutSolidTeapot(500);
  glPopMatrix();

  /* Draw model */
  DrawModel();

  /* Finalize frame */

  glFinish();
  glutSwapBuffers();
}


void MousePressed(int button, int state, int x, int y)
{
  if (button == GLUT_LEFT_BUTTON)
    flag = 1;
  if (button == GLUT_RIGHT_BUTTON)
    flag = 0;
  ax = x, ay = y;
}

void Timer( int t )
{
  glutPostRedisplay();
  glutTimerFunc(100, Timer, 0);
}

void Keyboard( unsigned char Key, int X, int Y )
{
  static int IsFullScreen = 0, SaveW, SaveH;

  if (Key == 27)
    exit(0);
  else if (Key == 'p')
    IsPause = !IsPause;
  else if (Key == 'w')
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  else if (Key == 'q')
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  else if (Key == 'f')
  {
    if (IsFullScreen)
      glutReshapeWindow(SaveW, SaveH);
    else
    {
      SaveW = FrameW;
      SaveH = FrameH;
      glutFullScreen();
    }
    IsFullScreen = !IsFullScreen;
  }
  else if (Key == 'a')
    Key_Arm = !Key_Arm;
  else if (Key == 's')
    Key_Shoulder = !Key_Shoulder;
  else if (Key == 'd')
    Key_Shoulder2 = !Key_Shoulder2;
  else if (Key == 'h')
    Key_Hand = !Key_Hand;
  else if (Key == 'g')
    Key_Hand2 = !Key_Hand2;
}

void Reshape( int W, int H )
{
  double
    size = 0.010,
    ratio_x = size / 2, ratio_y = size / 2;

  glViewport(0, 0, FrameW = W, FrameH = H);

  if (W > H)
    ratio_x *= (double)W / H;
  else
    ratio_y *= (double)H / W;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  //glOrtho(-ratio_x, ratio_x, -ratio_y, ratio_y, -1000, 1000);
  glFrustum(-ratio_x, ratio_x, -ratio_y, ratio_y, size, 1000);
  //glOrtho(0, W_Width, 0, W_Height, -1000.0, 1000.0);
  glMatrixMode(GL_MODELVIEW);
}

void Init( void )
{
  glClearColor(0.1, 0.1, 0.1, 0.2);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_NORMALIZE);
}


int main(int argc, char **argv)
{
  /* Initiliazation */
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
  glutInitWindowPosition(0, 0);
  glutInitWindowSize(W_Width, W_Height);
  glutCreateWindow("Arm simulation project");
 
  // Application init
  Init();
 
  glutKeyboardFunc(Keyboard);
  glutMouseFunc(MousePressed);
  glutReshapeFunc(Reshape);
  glutDisplayFunc(Draw);
  glutTimerFunc(1000, Timer, 0);

  glutMainLoop();
  return 0;
}