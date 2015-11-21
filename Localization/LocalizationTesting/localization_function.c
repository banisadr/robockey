//Localization General Function

#include "localization_function.h"
#include "m_general.h"
#include "m_usb.h"
#include <stdlib.h>

float* fourPointCalc(int points[4][2]);
float* threePointCalc(int points[3][2]);
float* twoPointCalc(int points[2][2]);
float* onePointCalc(int points[1][2]);
int* distIndex(int indexVal);
void orientationCalculationFour(int* pointsLong, float distVect[], int* top, int* bottom);


//Public function declarations 
float* localization_calc(int rawStarData[8], float robotCenterPrev[3])
{

	m_usb_tx_string(" rawStarData: ");
	m_usb_tx_int(rawStarData[0]);
	m_usb_tx_string(" ");
	m_usb_tx_int(rawStarData[1]);
	m_usb_tx_string(" ");
	m_usb_tx_int(rawStarData[2]);
	m_usb_tx_string(" ");
	m_usb_tx_int(rawStarData[3]);
	m_usb_tx_string(" ");
	m_usb_tx_int(rawStarData[4]);
	m_usb_tx_string(" ");
	m_usb_tx_int(rawStarData[5]);
	m_usb_tx_string(" ");
	m_usb_tx_int(rawStarData[6]);
	m_usb_tx_string(" ");
	m_usb_tx_int(rawStarData[7]);
	
	static float robotCenter[3];
	
	//Put the star data in a matrix where each row corresponds to 4 points
	int pvect[4][2] = {
		{rawStarData[0], rawStarData[4]},
		{rawStarData[1], rawStarData[5]},
		{rawStarData[2], rawStarData[6]},
		{rawStarData[3], rawStarData[7]}
	};
	
	
	//Extract all points that are readable into an array
	int pointIndices[4] = {0, 0, 0, 0}; //Initializes the indices
	int numPoints = 0; //Initialize number of points
	
	for (int i = 0; i < 4; i++){
		if (pvect[i][0] != 1023){
			pointIndices[numPoints] = i;
			numPoints++;
		}
	}
	
	
	if (numPoints) { //If any points were found 
		int points[numPoints][2]; //an array of the usable points 
		for (int i = 0; i < numPoints; i++) {
			int index = pointIndices[i];
			points[i][0] = pvect[index][0];
			points[i][1] = pvect[index][1];
		}
		
		
		m_red(ON);
		
		float* calcPointer; 
		static float rc[3] = {0,0,0};
		rc[0] = robotCenterPrev[0];
		rc[1] = robotCenterPrev[1];
		rc[2] = robotCenterPrev[2];
		switch (numPoints)
		{
			case 4 :
				calcPointer = fourPointCalc(points);
				break;
				
			case 3 : 
				//calcPointer = threePointCalc(points);
				calcPointer = &rc[0];
				break;
				
			case 2 :
				//calcPointer = twoPointCalc(points);
				calcPointer = &rc[0];
				break; 
				
			case 1 : 
				//calcPointer = onePointCalc(points);
				calcPointer = &rc[0];
				break; 
				
			default:
				calcPointer = &rc[0];
				
		}
		
		robotCenter[0] = (float)calcPointer[0];
		robotCenter[1] = (float)calcPointer[1];
		robotCenter[2] = (float)calcPointer[2];
		
	
	} else { 
		robotCenter[0] = robotCenterPrev[0]; 
		robotCenter[1] = robotCenterPrev[1];
		robotCenter[2] = robotCenterPrev[2];
		m_red(OFF);	
	}
	
	
	return robotCenter;
	
	
	
}


float* fourPointCalc(int pvect[4][2])
{
	static float centerFour[3];
	
	float dist01 = sqrt((float)(pvect[1][0]-pvect[0][0])*(pvect[1][0]-pvect[0][0]) + (pvect[1][1]-pvect[0][1])*(pvect[1][1]-pvect[0][1]));
	float dist02 = sqrt((float)(pvect[2][0]-pvect[0][0])*(pvect[2][0]-pvect[0][0]) + (pvect[2][1]-pvect[0][1])*(pvect[2][1]-pvect[0][1]));
	float dist03 = sqrt((float)(pvect[3][0]-pvect[0][0])*(pvect[3][0]-pvect[0][0]) + (pvect[3][1]-pvect[0][1])*(pvect[3][1]-pvect[0][1]));
	float dist12 = sqrt((float)(pvect[2][0]-pvect[1][0])*(pvect[2][0]-pvect[1][0]) + (pvect[2][1]-pvect[1][1])*(pvect[2][1]-pvect[1][1]));
	float dist13 = sqrt((float)(pvect[3][0]-pvect[1][0])*(pvect[3][0]-pvect[1][0]) + (pvect[3][1]-pvect[1][1])*(pvect[3][1]-pvect[1][1]));
	float dist23 = sqrt((float)(pvect[3][0]-pvect[2][0])*(pvect[3][0]-pvect[2][0]) + (pvect[3][1]-pvect[2][1])*(pvect[3][1]-pvect[2][1]));
	
	float distVect[6] = {dist01, dist02, dist03, dist12, dist13, dist23};
	
	int indexMax = 0;
	float maxVal = 0;
	for (int i = 0; i < 6; i++){
		if (distVect[i] > maxVal) {
			indexMax = i;
			maxVal = distVect[i];
		}
	}
	
	int* pointsLong; 
	pointsLong = distIndex(indexMax);
	float center[2]; 
	center[0] = (pvect[pointsLong[0]][0] + pvect[pointsLong[1]][0])/2;
	center[1] = (pvect[pointsLong[0]][1] + pvect[pointsLong[1]][1])/2;
	
	int top;
	int bottom;
	orientationCalculationFour(pointsLong, distVect, &top, &bottom);
	
	float centerLine[2]; 
	centerLine[0] = pvect[top][0] - pvect[bottom][0];
	centerLine[1] = pvect[top][1] - pvect[bottom][1];
	
	float theta = atan2(-1.0*centerLine[1], (float)centerLine[0]); 
	theta += M_PI/2.0;
	
	float t[2];
	t[0] = -1*center[0] + 512;
	t[1] = -1*center[1] + 382;
	
	
	centerFour[0] = sin(theta)*t[1] - cos(theta)*t[0];
	centerFour[1] = cos(theta)*t[1] + sin(theta)*t[0];
	centerFour[2] = -theta*180.0/M_PI;
	
	return centerFour;
}


float* threePointCalc(int pvect[3][2])
{
	static float centerThree[2];	
	centerThree[0] = 1023;
	centerThree[1] = 1023;
	return centerThree;
}


float* twoPointCalc(int pvect[2][2])
{
	static float centerTwo[2];
	centerTwo[0] = 1023;
	centerTwo[1] = 1023;
	return centerTwo;
}


float* onePointCalc(int pvect[1][2])
{
	static float centerOne[2];
	centerOne[0] = 1023;
	centerOne[1] = 1023;
	return centerOne;
}


int* distIndex(int indexVal)
{
	static int pointsUsed[2] = {0, 0};
		
	switch (indexVal)
	{
		case 0 : 
			pointsUsed[0] = 0;
			pointsUsed[1] = 1;
			break;
			
		case 1 : 
			pointsUsed[0] = 0;
			pointsUsed[1] = 2;
			break;
			
		case 2 : 
			pointsUsed[0] = 0;
			pointsUsed[1] = 3;
			break;
			
		case 3 : 
			pointsUsed[0] = 1;
			pointsUsed[1] = 2;
			break;
			
		case 4 : 
			pointsUsed[0] = 1;
			pointsUsed[1] = 3;
			break;
			
		case 5 :
			pointsUsed[0] = 2;
			pointsUsed[1] = 3;
			break;
	}
	
	return pointsUsed;
}


void orientationCalculationFour(int* pointsLong, float distVect[], int* top, int* bottom)
{
	//Find index of shortest length
	int indexMin = 0;
	float minVal = distVect[0];
	for (int i = 0; i < 6; i++){
		if (distVect[i] < minVal) {
			indexMin = i;
			minVal = distVect[i];
		}
	}
	
	//Find points used by shortest length
	
	int* pointsShort; 
	pointsShort = distIndex(indexMin);
	
	if (pointsShort[0] == pointsLong[0]){
		*top = pointsLong[0];
		*bottom = pointsLong[1];
	} else {
		if (pointsShort[0] == pointsLong[1]){
			*top = pointsLong[1];
			*bottom = pointsLong[0];
		} else {
			if (pointsShort[1] == pointsLong[0]) {
				*top = pointsLong[0];
				*bottom = pointsLong[1];
			} else {
				if (pointsShort[1] == pointsLong[1])
				{
					*top = pointsLong[1];
					*bottom = pointsLong[0];
				} else {
						m_green(OFF);
						*top = pointsLong[1];
						*bottom = pointsLong[0];
				}
			}
		}
	}
// 	m_usb_tx_int(*top);
// 	m_usb_tx_string(" ");
// 	m_usb_tx_int(*bottom);
// 	m_usb_tx_string(" ");
}