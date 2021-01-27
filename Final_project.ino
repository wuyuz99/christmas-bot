#include "PollTimer.h"// include time control library
#include <Servo.h> // include the Servo library
/* * * * * * * * * * * * * * * *
 * Here the Declaration Starts *
 * * * * * * * * * * * * * * * */
int i;
int start=0;
int loop_counter=0;

/********** Song Part **********/
int   tones[]={2093,2349,2637,2794,3136,3520,3951,3729,4186,4699,5274,5588};
//             c    d    e    f    g    a    b    aS   C    D    E    F
char tone_c[]={'c', 'd', 'e', 'f', 'g', 'a', 'b', 'S', 'C', 'D', 'E', 'F'};
int leng=sizeof(tones);

char song[]{'0','0','C','S','a','g','f','g','a','f','g','a','S','g','a','g','f','e','f',  'g','a','S','g','a','S','C','g','a','b','C','D','E','F','E','D','C','0',  'C','S','a','g','f','g','a','f','D','D','D','D','C','S','a','g','f'};
int durat[]{ 1 , 4 , 3 , 1 , 2 , 2 , 2 , 2 , 2 , 2 , 1 , 1 , 1 , 1 , 3 , 1 , 2 , 2 , 4 ,   3 , 1 , 2 , 2 , 3 , 1 , 2 , 2 , 1 , 1 , 2 , 1 , 1 , 2 , 2 , 2 , 4 , 6 ,   3 , 1 , 2 , 2 , 2 , 2 , 2 , 2 , 1 , 1 , 1 , 1 , 3 , 1 , 2 , 2 , 4 };
// array of song and durat shows the whole song
int song_l=sizeof(song);

int play=0;//the note being played
int dur=5*durat[0];//set the note length(n*5*50ms)
int buzzer=4;//buzzer pin
int j=12 ;//the first note

PollTimer musicloop(20UL);//every loop is 50 ms

/********** Servo Part **********/
int serpl=3;//left servo pin
int serpr=4;//right servo pin
Servo serl;//left servo
Servo serr;//right servo

int serl_stop=93;//Left servo stop number
int serr_stop=95;//Right servo stop number
int boundary=130;//The minimum difference in reading of two IR sensors

void foreward();
void backward();
void trunLeft();
void turnRight();
void still();

PollTimer moveloop(25UL);//set the frequency of moving and checking to 20 hz

/********** Sensor Part **********/
int sensl=A0;//left Opto-electric sensor
int sensr=A1;//right Opto-electric sensor

/********** LED Part **********/
int led[]={10,11,12};//LED pins
int led_config[3][3]={
{1,0,0},
{0,1,0},
{0,0,1}};//the led configrations
int ledstate=0;//the one configration currently at

/********** Bottom Part **********/
int bottom=8;//bottom pin
PollTimer bottomloop(1UL);

/* * * * * * * * * * * * * * * * *
 *Here the Initialization Starts *
 * * * * * * * * * * * * * * * * */
void setup() 
{
  Serial.begin(9600);
  serl.attach(3);//initialize pin for the left servo
  serr.attach(4);//initialize pin for the right servo
  //initialize servo pins
  
  pinMode(sensl,INPUT);
  pinMode(sensr,INPUT);
  //initialize all sensor pins as input

  pinMode(bottom,INPUT);
  
  int num=sizeof(led);
  for(i=0;i<num;i++)
  {
     pinMode(led[i],OUTPUT);
  }//initialize LED pins as output
  
  pinMode(buzzer,OUTPUT);
  still();

}

void loop() 
{
 int dir=0;
 start=1;
 while(start==0)//if not started
 {
  if(digitalRead(bottom)){
    start=1;
    bottomloop.start();
    moveloop.start();
    musicloop.start();
  }//when bottom pressed start all the timing loops
  noTone(buzzer);
  still();
 }
 if(start==1)
 {
    if(bottomloop.check())//check moving loop at 1hz
    {
      if(loop_counter==50)//time 50seconds
      {
        start=0; 
        loop_counter=0;
      }else{
        loop_counter++;
        Serial.print("Timer: ");
        Serial.println(loop_counter);
      }
    }
   if(moveloop.check())//check moveloop
   {
    int lr=analogRead(sensl);
    int rr=analogRead(sensr);
    Serial.print(lr);
    Serial.print("   ");
    Serial.println(rr);
    
    dir = lr-rr;//the darker the color is, the higher the reading is
    if(dir>boundary)//if difference greater than some certain value
    {
      trunLeft();
    }
    else if(dir<-boundary)
    { 
      turnRight();
    }else{
      foreward();
    } 
   }
   
 if(musicloop.check())
   {
     if(dur==0)
    {
      noTone(buzzer);
      if(play==song_l-1)//when reach the end of the song
      {
        play=0;
      }else{//if not the end of the song
        play++;//move to next note
        for(j=0;j<leng;j++)//find the next note to play
        {
          if (song[play]==tone_c[j])
          {
            break;
          }
        }
        changeled();
      }
      dur=5*durat[play];
    }else{
      if (j<leng)//if note not found in the list(a break)
      {
         tone(buzzer, tones[j]);
       }else{//else play the note
          noTone(buzzer);
       }
     }
      dur--;
    }
  }
}

/* * * * * * * * * * * * * * * * *
 *Here the Functions are defined *
 * * * * * * * * * * * * * * * * */
void foreward()
{
  serl.write(180);
  serr.write(180);
}
void backward()
{
  serl.write(0);
  serr.write(180);
}
void trunLeft()
{
  serl.write(0);
  serr.write(180);
}
void turnRight()
{
  serl.write(180);
  serr.write(0);
}
void still()
{
  serl.write(serl_stop);
  serr.write(serr_stop);
}
void changeled()
{
  for(int i=0;i<3;i++)
  {
    digitalWrite(led[i],led_config[ledstate][i]);
  }
  ledstate++;
  if(ledstate==3)
  {
    ledstate=0;
  }
}
