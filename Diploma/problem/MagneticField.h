#ifndef DIPLOMA_MAGNETICFIELD_H
#define DIPLOMA_MAGNETICFIELD_H

#ifndef SIGNED_ARR_SIZE
    #define SIGNED_ARR_SIZE
#endif

#include "SimpleTriangleGrid.h"
#include "result_codes.h"


typedef struct magnetic_params_t
{
	STGridParams gridParams;
    double relaxParamInitial;
    double relaxParamMin;
	double chi;
	double accuracy;
    int iterationsNumMax;
} MagneticParams;


class MagneticField
{
public:
	MagneticField(const MagneticParams& params);


    void setChi(double chi);

    double currentChi() const;

    void setRelaxationParam(double param);

    double currentRelaxationParam() const;

    MagneticParams parameters() const;

    void setGrid(const SimpleTriangleGrid& grid);

    const SimpleTriangleGrid& grid() const;

    void setLastValidResult(const Matrix<double>& values);

    const Matrix<double>& lastValidResult() const;

    const Array<Vector2<double>>& innerDerivatives() const;

    const Array<Vector2<double>>& outerDerivatives() const;

    void resetIterationsCounter();


	ResultCode calcRelaxation();


	Vector2<double> calcInnerDerivative(double param) const;

	Vector2<double> calcOuterDerivative(double param) const;


    void updateGrid(const Array<Vector2<double>>& surfacePoints);


	void calcInitialApproximation();

private:
	MagneticParams mParams;

	SimpleTriangleGrid mGrid;

	Matrix<double> mLastValidValues;
	Matrix<double> mCurApprox;
	Matrix<double> mNextApprox;

	Array<Vector2<double>> mInnerDerivatives;
	Array<Vector2<double>> mOuterDerivatives;

	double mCurRelaxationParam;

	unsigned int mIterationsCounter;


	double calcCoefficientIntegral(const Vector2<double>& vert1, 
                                   const Vector2<double>& vert2, 
                                   const Vector2<double>& vert3, 
                                   double doubleTriangleArea, 
                                   double chi) const;

    double calcCoefficient(const Vector2<arr_size_t>& globIndex, arr_size_t coefIndex);

	double calcCoefficient(arr_size_t i, arr_size_t j, arr_size_t coefIndex);

    double calcNextValue(const Vector2<arr_size_t>& globIndex);

	double calcNextValue(arr_size_t i, arr_size_t j);

	void calcNextApproximation();


    void calcDerivatives();


	bool isApproximationValid(const Matrix<double>& approx) const;

	bool isIndicesValid(const Vector2<arr_size_t>& indices) const;
};

#endif