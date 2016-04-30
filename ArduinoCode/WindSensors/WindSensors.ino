/*
 * rosserial publisher for wind sensors
 *
 * publishing the apparent windspeed in [m/s]
 * based on rotation counts from the wind anemometer
 *
 * publishing the apparent wind direction in [degrees]
 * based on the resistance measured with the wind vane
 */

#include <ros.h>
#include <stdlib.h>
#include <std_msgs/Float64.h>

ros::NodeHandle nh;

std_msgs::Float64 apparent_wind_speed;
std_msgs::Float64 apparent_wind_direction;
ros::Publisher publishApparentWindSpeed("/wind_speed_apparent", &apparent_wind_speed);
ros::Publisher publishApparentWindAngle("/wind_direction_apparent", &apparent_wind_direction);

float wind_speed = 0;   // Initialise wind speed
int timestep = 100;     // timestep in [ms] 

int PIN = 2;            // For the wind speed sensor, pin 2 or 3 only
int count = 0;

int   i;                // count number (for loop)


// read raw sensor data from the analog PINs
// Initialisation for the wind directions sensor
int   Reference[16] = { 422,   487, 516,  603, 707,  780, 818,   891, 915,   958, 965,   980, 994,   1001, 1009, 1016};
float Direction[16] = {112.5, 157.5, 135, 67.5,  90, 22.5,  45, 202.5, 180, 337.5,   0, 247.5, 225,  292.5,  270,  315};
int   position=-1,thread=100;
int   sensorValue1; 


void arduino_anemometer()
{
 count++;
}

void setup()
{
  nh.getHardware()->setBaud(9600);
  nh.initNode();
  nh.advertise(publishApparentWindSpeed);
  nh.advertise(publishApparentWindAngle);
  attachInterrupt(PIN , arduino_anemometer, RISING);

  sensorValue1 = analogRead(A0);  // Pin A0 for the wind direction
}


void loop()
{
  // Publish previously measured wind speed
  apparent_wind_speed.data = wind_speed;
  publishApparentWindSpeed.publish( &apparent_wind_speed );
  nh.spinOnce();


  // measure wind speed over the next time step
  count = 0; 
  delay(timestep);
  wind_speed = (666.66/timestep) * count;

  thread=100;
  // Update apparent wind angle
  for(i=0;i<=15;i++){
        if(abs(Reference[i]-sensorValue1)<thread){
            thread=abs(Reference[i]-sensorValue1);
            position=i;
        }
  }
  apparent_wind_direction.data = Direction[position];
  publishApparentWindAngle.publish(&apparent_wind_direction);
}