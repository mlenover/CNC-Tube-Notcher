# 1 "c:\\Users\\MLeno\\Documents\\GitHub\\CNC-Tube-Notcher\\Nucleo-Board-Source\\main.h"
# 1 "c:\\Users\\MLeno\\Documents\\GitHub\\CNC-Tube-Notcher\\Nucleo-Board-Source\\main.ino"




//FOR TESTINGS VALUES
float r1 = 0;
float r2 = 3;
float theta1 = 0;
float theta2 = 0;
float z1 = 0;
float z2 = 2;
float feedrate = 100;
//END OF TESTING VALUES

int i;

float deltaz = z2 - z1;
float deltar = r2 - r1;
float deltatheta = theta2 - theta1;

float delta[3] = {deltar, deltatheta, deltaz};

float r_speed;
float theta_speed;
float z_speed;

float speed[3];

float steps_per[3] {100, 10 /*Max acceleration of rotation axis in rad/s^2*/, 10 /*Max acceleration of z axis in mm/s^2*/};

float getNextCommand(){

}

float *getSpeed(float feedrate, float delta[3], float speed[3]) //Takes feedrate & array of r, theta, z, delta values
                                                                    //Returns array of speeds for r, theta, z
{
    deltar = delta[1];
    deltatheta = delta[2];
    deltaz = delta[3];

    float r_avg = (r2+r1)/2;
    if (deltar != 0) {
        r_speed = feedrate / sqrt(((deltaz/deltar)*(deltaz/deltar)) + ((r_avg)*(r_avg))*((deltatheta/deltar)*(deltatheta/deltar)) + 1);
        theta_speed = (deltatheta / deltar)*r_speed;
        z_speed = (deltaz / deltar)*r_speed;
    }
    else if (deltatheta != 0){
        r_speed = 0;
        theta_speed = feedrate / sqrt(((r_avg)*(r_avg)) + ((deltaz/deltatheta)*(deltaz/deltatheta)));
        z_speed = (deltaz / deltatheta) * theta_speed;
    }
    else {
        r_speed = 0;
        theta_speed = 0;
        z_speed = feedrate;
    }
    speed[1] = abs(r_speed);
    speed[2] = abs(theta_speed);
    speed[3] = abs(z_speed);
    return speed;
}

void setup()
{
    Serial2.begin(9600);
}

void loop()
{
    int i = 1;
}
