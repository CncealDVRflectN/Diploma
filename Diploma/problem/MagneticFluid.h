#ifndef DIPLOMA_MAGNETICFLUID_H
#define DIPLOMA_MAGNETICFLUID_H

#include "RightSweep.h"
#include "MagneticField.h"
#include "result_codes.h"


typedef struct fluid_params_t
{
    double relaxParamInitial;
	double relaxParamMin;
	double epsilon;
	double chi;
	double w;
	int splitsNum;
	int iterationsNumMax;
    bool isRightSweepPedantic;
} FluidParams;


class MagneticFluid
{
public:
	MagneticFluid(const FluidParams& params);


    void setRelaxationParam(double param);

    double currentRelaxationParam() const;

    void setW(double w);

    double currentW() const;

    void setChi(double chi);

    double currentChi() const;

    FluidParams parameters() const;

    arr_size_t pointsNum() const;

    void setLastValidResult(const Array<Vector2<double>>& values);

    const Array<Vector2<double>>& lastValidResult() const;

    void setDerivatives(const Array<Vector2<double>>& values);

    double volumeNondimMul() const;

    double heightCoef() const;

    void resetIterationsCounter();


	void calcInitialApproximation();

	ResultCode calcRelaxation();

private:
    arr_size_t mPointsNum;

    double mCurRelaxationParam;
    double mStep;

    unsigned int mIterationsCounter;

	FluidParams mParams;

	RightSweep mRightSweep;

	Array<Vector2<double>> mLastValidResult;
	Array<Vector2<double>> mDerivatives;
	Array<double> mNextApproxR;
	Array<double> mNextApproxZ;
	Array<double> mCurApproxR;
	Array<double> mCurApproxZ;


	void calcNextApproximationR(const Array<double>& valZ, const Array<double>& prevValZ);

	void calcNextApproximationZ(const Array<double>& valR, const Array<double>& prevValR);


	double calcIntegralTrapeze(const Array<double>& approxR, const Array<double>& approxZ) const;

	double calcIntegralTrapeze(const Array<Vector2<double>>& approx) const;

	double calcMagneticIntegralTrapeze(const Array<double>& approxR, 
                                       const Array<double>& approxZ, 
                                       double integralCbrt) const;


	double calcQ(const Array<double>& approxR, 
                 const Array<double>& approxZ, 
                 double integralCbrt) const;

	double calcMagneticF(const Array<double>& approxR, 
                         const Array<double>& approxZ, 
                         int index, 
                         double integralCbrt) const;


	bool isApproximationValid(const Array<double>& approx) const;
};


#endif