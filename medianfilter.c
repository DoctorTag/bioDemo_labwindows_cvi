#include "stdint.h"
#include "stdbool.h"


#define MEDIAN_WIN_SIZE  67

uint8_t medDataPointer;       // mid point of window
int     data[MEDIAN_WIN_SIZE];               // array pointer for data sorted by age in ring buffer
uint8_t sizeMap[MEDIAN_WIN_SIZE];         // array pointer for locations data in sorted by size
uint8_t locationMap[MEDIAN_WIN_SIZE];     // array pointer for data locations in history map
uint8_t oldestDataPoint;   // oldest data point location in ring buffer


void MedianFilterInit(int seed)
{
    medDataPointer  = (MEDIAN_WIN_SIZE >> 1);           // mid point of window
    oldestDataPoint = medDataPointer;      // oldest data point location in data array
   
    for(uint8_t i = 0; i < MEDIAN_WIN_SIZE; i++) // initialize the arrays
    {
        sizeMap[i]     = i;      // start map with straight run
        locationMap[i] = i;      // start map with straight run
        data[i]        = seed;   // populate with seed value
    }
}


int MedianFilterProcess(int  value)
{
    // sort sizeMap
    // small vaues on the left (-)
    // larger values on the right (+)

    uint8_t dataMoved = false;
    const uint8_t rightEdge = MEDIAN_WIN_SIZE - 1;  // adjusted for zero indexed array


    data[oldestDataPoint] = value;  // store new data in location of oldest data in ring buffer

    // SORT LEFT (-) <======(n) (+)
    if(locationMap[oldestDataPoint] > 0) // don't check left neighbours if at the extreme left
    {
        for(uint8_t i = locationMap[oldestDataPoint]; i > 0; i--)   //index through left adjacent data
        {
            uint8_t n = i - 1;   // neighbour location

            if(data[oldestDataPoint] < data[sizeMap[n]]) // find insertion point, move old data into position
            {
                sizeMap[i] = sizeMap[n];   // move existing data right so the new data can go left
                locationMap[sizeMap[n]]++;

                sizeMap[n] = oldestDataPoint; // assign new data to neighbor position
                locationMap[oldestDataPoint]--;

                dataMoved = true;
            }
            else
            {
                break; // stop checking once a smaller value is found on the left
            }
        }
    }

    // SORT RIGHT (-) (n)======> (+)
    if(!dataMoved && locationMap[oldestDataPoint] < rightEdge) // don't check right if at right border, or the data has already moved
    {
        for(int i = locationMap[oldestDataPoint]; i < rightEdge; i++)   //index through left adjacent data
        {
            int n = i + 1;   // neighbour location

            if(data[oldestDataPoint] > data[sizeMap[n]]) // find insertion point, move old data into position
            {
                sizeMap[i] = sizeMap[n];   // move existing data left so the new data can go right
                locationMap[sizeMap[n]]--;

                sizeMap[n] = oldestDataPoint; // assign new data to neighbor position
                locationMap[oldestDataPoint]++;
            }
            else
            {
                break; // stop checking once a smaller value is found on the right
            }
        }
    }
    oldestDataPoint++;       // increment and wrap
    if(oldestDataPoint == MEDIAN_WIN_SIZE) oldestDataPoint = 0;

    return data[sizeMap[medDataPointer]];
}


int MedianFilterResult() // return the value of the median data sample
{
    return  data[sizeMap[medDataPointer]];
}


int MedianFilterGetMin()
{
    return data[sizeMap[ 0 ]];
}


int MedianFilterGetMax()
{
    return data[sizeMap[ MEDIAN_WIN_SIZE - 1 ]];
}



