# Detect-the-fall-of-the-motorcycle
Based on the fall detection algorithm of the motorcycle when the motorcycle falls, the fall of the motorcycle can be detected by using accelerometer sensor. 
The system determines the angular velocity and checks if angular velocity exceeds threshold. 
If angular velocity exceeds threshold, we check lean angle and acceleration value of the motorcycle. 
If these values exceed the threshold, the system detects the fall of the motorcycle. 
Furthermore, the coordinates of the fall location can be gathered by using the GPS module. 
Through the GSM module, we send a message including the Google Map link of the fall location to the assigned contact
