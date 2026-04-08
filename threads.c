// Team members: Maximos Frountzos, Vasileios Zafeiris, Giorgos Papastergiou, Kyril Koychev
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "devices.h"


int main(int argc, char *argv[]) 
{
	int n = init_devices ();
	
    //shared variables between the threads
    int S1 = 0;
    int S2 = 0;
    int D0 = 1, D1 = 0, D2 = 0, D3 = 0;
    int led1, led2;
    printf("main");
    

    #pragma omp parallel sections num_threads(4)
    {
            // head tilt 
            #pragma omp section 
            {
                int x_tilted_previous = 0, x_tilted_now = 0, y_tilted_previous = 0, y_tilted_now = 0;
                while(1)
                {
                    int x = Read_Giroscope_X();
                    int y = Read_Giroscope_Y();
                    int y_drowsy_condition = 0;
                   
                    printf("x:%d y:%d ", x,y);
                    fflush(stdout);
                    int steering_angle = (read_single_ADC_sensor(2) / 1023.0) * 360 - 180;
                    printf("steering: %d", steering_angle);
                    fflush(stdout);
                    // x-Axis now
                    x_tilted_now = (x > 30 || x < -30);

                    // y-Axis now
                    y_tilted_now = (y > 30 || y < -30);
                    if (y_tilted_now)
                    { 
						if(abs(steering_angle) < 7)
						{ 
							y_drowsy_condition = 1;
						}
						else if (y > 30 && steering_angle <= -7)
						{ 
							y_drowsy_condition = 1;
						}
						else if (y < -30 && steering_angle >= 7)
						{ 
							y_drowsy_condition = 1;
						}
					}
					
                    #pragma omp critical
                    S1 = ((x_tilted_now && x_tilted_previous) || (y_drowsy_condition && y_tilted_previous));

                    // x-Axis update for next reading
                    x_tilted_previous = x_tilted_now;

                    // y-Axis update for next reading
                    y_tilted_previous = y_drowsy_condition;
                    delay(400);
                  
                }
            }
        
            // distance 
            #pragma omp section  
            {
                while(1)
                {
                    float distance = getDistance(); 
                
                     
                    float speed = (read_single_ADC_sensor(3)/1023.0)*140;
                    float safety_distance = 2*(speed / 10.0);


                    #pragma omp critical
                    {
                        if(distance < safety_distance && distance >= safety_distance/2.0)
                        {
                            D0 = 0;
                            D1 = 1;
                            D2 = 0;
                            D3 = 0;
                        }
                        else if(distance < safety_distance/2.0 && distance >= safety_distance/3.0)
                        {
                            D0 = 0;
                            D1 = 0;
                            D2 = 1;
                            D3 = 0;
                        }
                        else if(distance < safety_distance/3.0)
                        {
                            D0 = 0;
                            D1 = 0;
                            D2 = 0;
                            D3 = 1;
                        }
                        else if (distance > safety_distance)
                        {
                            D0 = 1;
                            D1 = 0;
                            D2 = 0;
                            D3 = 0;
                        }
                    }   
                    
                
                    delay(300);
                   
                    
                }
            }

            // sharp turn 
            #pragma omp section 
            {
                int steering_angle_previous = 0;
                while(1)
                {
                    float speed = (read_single_ADC_sensor(3)/1023.0)*140;
                  
                    int steering_angle_now = (read_single_ADC_sensor(2) / 1023.0) * 360 - 180;

                    #pragma omp critical
                    S2 = ((abs(steering_angle_previous - steering_angle_now) > 20) && (speed > 40));

                    steering_angle_previous = steering_angle_now;                    
                    
                    delay(350);
              
                }
            } 

            // control thread
            #pragma omp section 
            {
                while(1)
                {
                    int s1, s2, d0, d1, d2, d3;
                    #pragma omp critical
                    {
                        s1 = S1;
                        s2 = S2;
                        d0 = D0;
                        d1 = D1;
                        d2 = D2;
                        d3 = D3;
                    }

                    if (d3)
                    {
                        led1 = set_led_1(0);
                        led2 = set_led_2(0);
                        moveServo(90); // level 3 breaking
                    }

                    else if ((s1 || s2) && d2)
                    {
                        led1 = set_led_1(1);
                        led2 = set_led_2(1);
                        moveServo(45); // level 2 breaking
                    }

                    else if ((s1 || s2) && d1)
                    {
                        led1 = set_led_1(1);
                        led2 = set_led_2(1);
                        moveServo(10); // level 1 breaking
                    }
                    
                    else if (d0 && (s1&&s2))
                    {
                        led1 = set_led_1(1);
                        led1 = set_led_2(0);
                    }

                    else if (d0 && (s1||s2))
                    {
                        led1 = set_led_2(1);
                        led2 = set_led_1(0);
                    }
                    
                    


                    // safe state
                    else
                    {
                        led1 = set_led_1(0);
                        led2 = set_led_2(0);
                        moveServo(0);
                    }  

                    delay(200);
                
                }
            } 
        }
}
