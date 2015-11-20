//Localization General Function

#include "localization_function.h"
#include "m_general.h"
#include "m_usb.h"
#include <stdlib.h>

int* fourPointCalc(int points[4][2]);
int* threePointCalc(int points[3][2]);
int* twoPointCalc(int points[2][2]);
int* onePointCalc(int points[1][2]);
int* distIndex(int indexVal);
void orientationCalculationFour(int* pointsLong, int distVect[], int* top, int* bottom);


//Public function declarations 
int* localization_calc(int rawStarData[8], int robotCenterPrev[3])
{
	static int robotCenter[3];
	
	//Put the star data in a matrix where each row corresponds to 4 points
	int pvect[4][2] = {
		{rawStarData[0], rawStarData[4]},
		{rawStarData[1], rawStarData[5]},
		{rawStarData[2], rawStarData[6]},
		{rawStarData[3], rawStarData[7]}
	};
	
	//If the x or y data is not received, ensure that the whole point is removed (set to 1023)
	for (int i = 0; i < 4; i++ ){
		for (int j = 0; j < 2; j++ ){
			if (pvect[i][j] == 1023){
				pvect[i][0] = 1023;
				pvect[i][1] = 1023;
			}
		}
	}
	
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
		
		int* calcPointer; 
		static int rc[3] = {0,0,0};
		rc[0] = robotCenterPrev[0];
		rc[1] = robotCenterPrev[1];
		rc[2] = robotCenterPrev[2];
		switch (numPoints)
		{
			case 4 :
				calcPointer = fourPointCalc(points);
				break;
				
			case 3 : 
				calcPointer = threePointCalc(points);
				break;
				
			case 2 :
				calcPointer = twoPointCalc(points);
				break; 
				
			case 1 : 
				calcPointer = onePointCalc(points);
				break; 
				
			default:
				calcPointer = &rc[0];
				
		}
		
		robotCenter[0] = (int)calcPointer[0];
		robotCenter[1] = (int)calcPointer[1];
		robotCenter[2] = (int)calcPointer[2];
		
	
	} else { 
		robotCenter[0] = robotCenterPrev[0]; 
		robotCenter[1] = robotCenterPrev[1];
		robotCenter[2] = robotCenterPrev[2];
		m_red(OFF);	
	}
	
	
	return robotCenter;
	
	
	
}


int* fourPointCalc(int pvect[4][2])
{
	static int centerFour[3];
	
	int dist01 = sqrt((pvect[1][0]-pvect[0][0])*(pvect[1][0]-pvect[0][0]) + (pvect[1][1]-pvect[0][1])*(pvect[1][1]-pvect[0][1]));
	int dist02 = sqrt((pvect[2][0]-pvect[0][0])*(pvect[2][0]-pvect[0][0]) + (pvect[2][1]-pvect[0][1])*(pvect[2][1]-pvect[0][1]));
	int dist03 = sqrt((pvect[3][0]-pvect[0][0])*(pvect[3][0]-pvect[0][0]) + (pvect[3][1]-pvect[0][1])*(pvect[3][1]-pvect[0][1]));
	int dist12 = sqrt((pvect[2][0]-pvect[1][0])*(pvect[2][0]-pvect[1][0]) + (pvect[2][1]-pvect[1][1])*(pvect[2][1]-pvect[1][1]));
	int dist13 = sqrt((pvect[3][0]-pvect[1][0])*(pvect[3][0]-pvect[1][0]) + (pvect[3][1]-pvect[1][1])*(pvect[3][1]-pvect[1][1]));
	int dist23 = sqrt((pvect[3][0]-pvect[2][0])*(pvect[3][0]-pvect[2][0]) + (pvect[3][1]-pvect[2][1])*(pvect[3][1]-pvect[2][1]));
	
	int distVect[6] = {dist01, dist02, dist03, dist12, dist13, dist23};
	
	int indexMax = 0;
	int maxVal = 0;
	for (int i = 0; i < 6; i++){
		if (distVect[i] > maxVal) {
			indexMax = i;
			maxVal = distVect[i];
		}
	}
	
	int* pointsLong; 
	pointsLong = distIndex(indexMax);
	int center[2]; 
	center[0] = (pvect[pointsLong[0]][0] + pvect[pointsLong[1]][0])/2;
	center[1] = (pvect[pointsLong[0]][1] + pvect[pointsLong[1]][1])/2;
	
	int top;
	int bottom;
	orientationCalculationFour(pointsLong, distVect, &top, &bottom);
	
	int centerLine[2]; 
	centerLine[0] = pvect[top][0] - pvect[bottom][0];
	centerLine[1] = pvect[top][1] - pvect[bottom][1];
	
	float theta = atan2((float)-1*centerLine[1], (float)centerLine[0]); 
	theta += M_PI/2;
	
	int t[2];
	t[0] = -1*center[0] + 512;
	t[1] = -1*center[1] + 382;
	
	
	centerFour[0] = sin(theta)*t[1] - cos(theta)*t[0];
	centerFour[1] = cos(theta)*t[1] + sin(theta)*t[0];
	centerFour[2] = -theta*180.0/M_PI;
	
	return centerFour;
}


int* threePointCalc(int pvect[3][2])
{
	static int centerThree[2];	
	centerThree[0] = 1023;
	centerThree[1] = 1023;
	return centerThree;
}


int* twoPointCalc(int pvect[2][2])
{
	static int centerTwo[2];
	centerTwo[0] = 1023;
	centerTwo[1] = 1023;
	return centerTwo;
}


int* onePointCalc(int pvect[1][2])
{
	static int centerOne[2];
	centerOne[0] = 1023;
	centerOne[1] = 1023;
	return centerOne;
}


int* distIndex(int indexVal)
{
	static int pointsUsed[2] = {0, 0};
		
	switch (indexVal)
	{
		case 1 : 
			pointsUsed[0] = 0;
			pointsUsed[1] = 1;
			break;
			
		case 2 : 
			pointsUsed[0] = 0;
			pointsUsed[1] = 2;
			break;
			
		case 3 : 
			pointsUsed[0] = 0;
			pointsUsed[1] = 3;
			break;
			
		case 4 : 
			pointsUsed[0] = 1;
			pointsUsed[1] = 2;
			break;
			
		case 5 : 
			pointsUsed[0] = 1;
			pointsUsed[1] = 3;
			break;
			
		case 6 :
			pointsUsed[0] = 2;
			pointsUsed[1] = 3;
			break;
	}
	
	return pointsUsed;
}


void orientationCalculationFour(int* pointsLong, int distVect[], int* top, int* bottom)
{
	//Find index of shortest length
	int indexMin = 0;
	int minVal = distVect[0];
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
				*top = pointsLong[1];
				*bottom = pointsLong[0];
			}
		}
	}
}