
//Number of types of balls avaliable to use
const int numberOfTypesOfBalls = 4;
//Number of sensors to use as input
const int numberOfSensors = 2;

//how many data point you want to use
const int sampleSize = 10;

//raw info. since cannot have 3d array made it into a 2d array.
//first [] is the type of ball
//second [] is the sensor value associated for that index. index * numberOfSensors + sensorIndex
float info[numberOfTypesOfBalls][sampleSize * numberOfSensors];

//aggregated data for each type of ball and the sensor value they return in the order of average, min, max
float retain[numberOfTypesOfBalls][3 * numberOfSensors];

//history of how many data points have been added to each type of ball
int count[numberOfTypesOfBalls];

//If you are using the weights or not
bool weightsHaveBeenSet = false;

//The weights are used to give more imprtance to some sensore values.
//The new error is calculated as following total += error * weight
float weights[numberOfSensors];

//Sets the weights to new values
void setWeights(float *newWeights)
{
	weightsHaveBeenSet = true;

	for (int i = 0; i < numberOfSensors; i++)
	{
		weights[i] = newWeights[i];
	}
}

/**
* Adds the captured raw data for that type of ball to the info array to be later used
* @param typeOfBall the type of ball this data is for
* @param data the sensor values that were returned
*/
void addData(int typeOfBall, float *data){
	if(count[typeOfBall] <= sampleSize)
	{
		for(int i = 0; i < numberOfSensors; i++){
			info[typeOfBall][count[typeOfBall] * numberOfSensors + i] = data[i];
		}

		count[typeOfBall] += 1;
	}
}

/**
* Aggregates the data into the retain array. This calulates and saves the min, max, and average
* for each sensor for each type of ball.
*/
void reduceInfo(){
	for(int i = 0; i < numberOfTypesOfBalls; i++){
		for(int t = 0; t < count[i]; t++){
			for(int y = 0; y < numberOfSensors; y++)
			{
				float currentNumber = info[i][t * numberOfSensors + y];

				if (t == 0){
					retain[i][y * 3 ] = currentNumber;
					retain[i][y * 3 + 1] = currentNumber;
					retain[i][y * 3+ 2] = currentNumber;
				}
				else{
					retain[i][y * 3] += currentNumber;

					if(currentNumber < retain[i][y * 3+ 1] ){
						retain[i][t * 3+ y + 1] = currentNumber;
					}
					if(currentNumber > retain[i][y * 3+ 2] ){
						retain[i][y * 3 + 2] = currentNumber;
					}
				}

				if(t == count[i] - 1)
				{
					retain[i][y * 3] /= count[i];
				}
			}
		}
	}
}

/**
* Normalizes a value given its min and max (sets it between 0-1 inclusively)
* @param x the value to normalize
* @param min the minimum value in the range
* @param max the maximum value in the range
* @return the normalized value of x
*/
float normalize(float x, float min, float max) {
	if (max == min)
		return 0;

	return (x - min) / (max - min);
}

/**
* Returns the most similar ball type given a certain data point. It finds the error between the normalized
* sensor data and figures will set the type as the type it has the least amount of error with.
* @param data the data point to compare with
* @return the type of ball the data returns is the most similar with
*/
int getMostSimilar(float *data)
{
	int type = 0;
	float error = 0;

	for (int i = 0; i < numberOfTypesOfBalls; ++i) {
		float errorTemp = 0;

		for (int j = 0; j < numberOfSensors; ++j) {
			float average = retain[i][j * 3];
			float min = retain[i][j * 3 + 1];
			if(min > data[j])
			{
				min = data[j];
			}

			float max = retain[i][j * 3 + 2];
			if(max < data[j])
			{
				max = data[j];
			}

			average = normalize(average, min, max);
			float temp = normalize(data[j], min, max);


			if(weightsHaveBeenSet){
				errorTemp += (fabs(average - temp) * weights[j]);
			}
			else{
				errorTemp += fabs(average - temp)
			}
		}

		if (i == 0) {
			error = errorTemp;
			} else {
			if (error > errorTemp) {
				type = i;
				error = errorTemp;
			}
		}
	}

	return type;
}
