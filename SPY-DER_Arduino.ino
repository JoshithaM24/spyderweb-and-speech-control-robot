/* =========================================================
   Spider Robot Final Nano Code
   TEST VERSION (No Ultrasonic)
   Works with Pi Flask / Webpage / Voice UART

   Commands:
   f = forward
   b = back
   l = left
   r = right
   s = stop

   Starts AUTO roaming forward until command sent
   Baud = 9600
========================================================= */

#include <Servo.h>
#include <FlexiTimer2.h>

char data = 'f';   // auto start forward

Servo servo[4][3];

const int servo_pin[4][3] = {
  {2,3,4},
  {5,6,7},
  {8,9,10},
  {11,12,13}
};

/* ---------------- ROBOT DIMENSIONS ---------------- */

const float length_a = 55;
const float length_b = 77.5;
const float length_c = 27.5;
const float length_side = 71;

const float z_absolute = -28;
const float z_default  = -50;
const float z_up       = -30;
const float z_boot     = z_absolute;

const float x_default = 62;
const float y_start   = 0;
const float y_step    = 40;

const float KEEP = 255;
const float pi = 3.1415926;

/* ---------------- SPEEDS ---------------- */

float move_speed;

const float spot_turn_speed = 4;
const float leg_move_speed  = 8;
const float body_move_speed = 3;
const float stand_speed     = 1;

/* ---------------- POSITION ARRAYS ---------------- */

volatile float site_now[4][3];
volatile float site_expect[4][3];
float temp_speed[4][3];

/* ---------------- TURN VALUES ---------------- */

const float temp_a =
sqrt(pow(2*x_default+length_side,2)+pow(y_step,2));

const float temp_b =
2*(y_start+y_step)+length_side;

const float temp_c =
sqrt(pow(2*x_default+length_side,2)+
pow(2*y_start+y_step+length_side,2));

const float temp_alpha =
acos((pow(temp_a,2)+pow(temp_b,2)-pow(temp_c,2))/
(2*temp_a*temp_b));

const float turn_x1 = (temp_a-length_side)/2;
const float turn_y1 = y_start+y_step/2;

const float turn_x0 =
turn_x1-temp_b*cos(temp_alpha);

const float turn_y0 =
temp_b*sin(temp_alpha)-turn_y1-length_side;

/* ================================================= */

void setup()
{
  Serial.begin(9600);

  set_site(0,x_default,y_start+y_step,z_boot);
  set_site(1,x_default,y_start+y_step,z_boot);
  set_site(2,x_default,y_start,z_boot);
  set_site(3,x_default,y_start,z_boot);

  for(int i=0;i<4;i++)
    for(int j=0;j<3;j++)
      site_now[i][j] = site_expect[i][j];

  servo_attach();

  FlexiTimer2::set(20, servo_service);
  FlexiTimer2::start();

  stand();
}

/* ================================================= */

void loop()
{
  if(Serial.available() > 0)
  {
    data = Serial.read();   // receive from Pi
  }

  if(data == 'f')
    cstep_forward();

  else if(data == 'b')
    cstep_back();

  else if(data == 'l')
    cturn_left();

  else if(data == 'r')
    cturn_right();

  else if(data == 's')
    stand();
}

/* ================================================= */

void servo_attach()
{
  for(int i=0;i<4;i++)
  {
    for(int j=0;j<3;j++)
    {
      servo[i][j].attach(servo_pin[i][j]);
      delay(50);
    }
  }
}

/* ================================================= */

void stand()
{
  move_speed = stand_speed;

  for(int i=0;i<4;i++)
    set_site(i,KEEP,KEEP,z_default);

  wait_all_reach();
}

/* ================================================= */
/* FORWARD */

void cstep_forward()
{
  move_speed = leg_move_speed;

  if(site_now[2][1] == y_start)
  {
    set_site(2,x_default,y_start,z_up);
    wait_all_reach();

    set_site(2,x_default,y_start+2*y_step,z_up);
    wait_all_reach();

    set_site(2,x_default,y_start+2*y_step,z_default);
    wait_all_reach();

    move_speed = body_move_speed;

    set_site(0,x_default,y_start,z_default);
    set_site(1,x_default,y_start+2*y_step,z_default);
    set_site(2,x_default,y_start+y_step,z_default);
    set_site(3,x_default,y_start+y_step,z_default);
    wait_all_reach();

    move_speed = leg_move_speed;

    set_site(1,x_default,y_start+2*y_step,z_up);
    wait_all_reach();

    set_site(1,x_default,y_start,z_up);
    wait_all_reach();

    set_site(1,x_default,y_start,z_default);
    wait_all_reach();
  }
  else
  {
    set_site(0,x_default,y_start,z_up);
    wait_all_reach();

    set_site(0,x_default,y_start+2*y_step,z_up);
    wait_all_reach();

    set_site(0,x_default,y_start+2*y_step,z_default);
    wait_all_reach();

    move_speed = body_move_speed;

    set_site(0,x_default,y_start+y_step,z_default);
    set_site(1,x_default,y_start+y_step,z_default);
    set_site(2,x_default,y_start,z_default);
    set_site(3,x_default,y_start+2*y_step,z_default);
    wait_all_reach();

    move_speed = leg_move_speed;

    set_site(3,x_default,y_start+2*y_step,z_up);
    wait_all_reach();

    set_site(3,x_default,y_start,z_up);
    wait_all_reach();

    set_site(3,x_default,y_start,z_default);
    wait_all_reach();
  }
}

/* ================================================= */
/* BACK */

void cstep_back()
{
  move_speed = leg_move_speed;

  if(site_now[3][1] == y_start)
  {
    set_site(3,x_default,y_start,z_up);
    wait_all_reach();

    set_site(3,x_default,y_start+2*y_step,z_up);
    wait_all_reach();

    set_site(3,x_default,y_start+2*y_step,z_default);
    wait_all_reach();

    move_speed = body_move_speed;

    set_site(0,x_default,y_start+2*y_step,z_default);
    set_site(1,x_default,y_start,z_default);
    set_site(2,x_default,y_start+y_step,z_default);
    set_site(3,x_default,y_start+y_step,z_default);
    wait_all_reach();

    move_speed = leg_move_speed;

    set_site(0,x_default,y_start+2*y_step,z_up);
    wait_all_reach();

    set_site(0,x_default,y_start,z_up);
    wait_all_reach();

    set_site(0,x_default,y_start,z_default);
    wait_all_reach();
  }
}

/* ================================================= */
/* LEFT */

void cturn_left()
{
  move_speed = spot_turn_speed;

  set_site(0,turn_x1,turn_y1,z_default);
  set_site(1,turn_x0,turn_y0,z_default);
  set_site(2,turn_x1,turn_y1,z_default);
  set_site(3,turn_x0,turn_y0,z_default);

  wait_all_reach();
}

/* ================================================= */
/* RIGHT */

void cturn_right()
{
  move_speed = spot_turn_speed;

  set_site(0,turn_x0,turn_y0,z_default);
  set_site(1,turn_x1,turn_y1,z_default);
  set_site(2,turn_x0,turn_y0,z_default);
  set_site(3,turn_x1,turn_y1,z_default);

  wait_all_reach();
}

/* ================================================= */

void servo_service()
{
  sei();

  static float alpha,beta,gamma;

  for(int i=0;i<4;i++)
  {
    for(int j=0;j<3;j++)
    {
      if(abs(site_now[i][j]-site_expect[i][j]) >= abs(temp_speed[i][j]))
        site_now[i][j] += temp_speed[i][j];
      else
        site_now[i][j] = site_expect[i][j];
    }

    cartesian_to_polar(
      alpha,beta,gamma,
      site_now[i][0],
      site_now[i][1],
      site_now[i][2]
    );

    polar_to_servo(i,alpha,beta,gamma);
  }
}

/* ================================================= */

void set_site(int leg,float x,float y,float z)
{
  float length_x=0,length_y=0,length_z=0;

  if(x!=KEEP) length_x = x-site_now[leg][0];
  if(y!=KEEP) length_y = y-site_now[leg][1];
  if(z!=KEEP) length_z = z-site_now[leg][2];

  float length =
  sqrt(pow(length_x,2)+pow(length_y,2)+pow(length_z,2));

  temp_speed[leg][0] = length_x/length*move_speed;
  temp_speed[leg][1] = length_y/length*move_speed;
  temp_speed[leg][2] = length_z/length*move_speed;

  if(x!=KEEP) site_expect[leg][0]=x;
  if(y!=KEEP) site_expect[leg][1]=y;
  if(z!=KEEP) site_expect[leg][2]=z;
}

/* ================================================= */

void wait_reach(int leg)
{
  while(1)
  {
    if(site_now[leg][0]==site_expect[leg][0] &&
       site_now[leg][1]==site_expect[leg][1] &&
       site_now[leg][2]==site_expect[leg][2])
      break;
  }
}

void wait_all_reach()
{
  for(int i=0;i<4;i++)
    wait_reach(i);
}

/* ================================================= */

void cartesian_to_polar(
volatile float &alpha,
volatile float &beta,
volatile float &gamma,
volatile float x,
volatile float y,
volatile float z)
{
  float v,w;

  w=(x>=0?1:-1)*(sqrt(x*x+y*y));
  v=w-length_c;

  alpha =
  atan2(z,v)+
  acos((length_a*length_a-length_b*length_b+v*v+z*z)/
  (2*length_a*sqrt(v*v+z*z)));

  beta =
  acos((length_a*length_a+length_b*length_b-v*v-z*z)/
  (2*length_a*length_b));

  gamma=(w>=0)?atan2(y,x):atan2(-y,-x);

  alpha=alpha/pi*180;
  beta=beta/pi*180;
  gamma=gamma/pi*180;
}

/* ================================================= */

void polar_to_servo(int leg,float alpha,float beta,float gamma)
{
  if(leg==0)
  {
    alpha=90-alpha;
    gamma+=90;
  }
  else if(leg==1)
  {
    alpha+=90;
    beta=180-beta;
    gamma=90-gamma;
  }
  else if(leg==2)
  {
    alpha+=90;
    beta=180-beta;
    gamma=90-gamma;
  }
  else if(leg==3)
  {
    alpha=90-alpha;
    gamma+=90;
  }

  servo[leg][0].write(alpha);
  servo[leg][1].write(beta);
  servo[leg][2].write(gamma);
}