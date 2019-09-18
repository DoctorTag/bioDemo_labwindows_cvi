#ifndef MEDIAN_FILTER_H__
#define MEDIAN_FILTER_H__


void MedianFilterInit(int seed);



int MedianFilterProcess(int value);


int MedianFilterResult() ;// return the value of the median data sample



int MedianFilterGetMin();



int MedianFilterGetMax();



int MedianFilterGetMean();

#endif
