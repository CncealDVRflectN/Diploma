#ifndef DIPLOMA_RIGHTSWEEP_H
#define DIPLOMA_RIGHTSWEEP_H

// if link problem occured with RightSweep, try to include it before Array
// or define _SIGNED_ARR_SIZE_ before first Array inclusion

#ifndef _SIGNED_ARR_SIZE_
    #define _SIGNED_ARR_SIZE_   // for reversal step
#endif


#include "Array.h"


enum RightSweepAccess
{
    RS_LOWER_DIAGONAL,
    RS_MAIN_DIAGONAL,
    RS_UPPER_DIAGONAL,
    RS_CONST_TERMS
};


class RightSweep
{
public:
	RightSweep(arr_size_t size, bool isPedantic = false);


    arr_size_t size();


    double& operator()(RightSweepAccess accessType, arr_size_t index);


    Array<double> solve();

	void solve(Array<double>& solutionDest);

private:
    Array<double> mLowerDiagonal;
    Array<double> mMainDiagonal;
    Array<double> mUpperDiagonal;
    Array<double> mConstTerms;

	Array<double> mAlpha;
	Array<double> mBeta;

	arr_size_t mSize;

    bool mIsPedantic;


	void calcAlpha();

	void calcBeta();

	void reversal(Array<double>& solutionDest);

	bool isValid();
};


#endif