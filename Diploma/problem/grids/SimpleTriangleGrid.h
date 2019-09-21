#ifndef DIPLOMA_SIMPLE_TRIANGLE_GRID_H
#define DIPLOMA_SIMPLE_TRIANGLE_GRID_H

#ifndef SIGNED_ARR_SIZE
    #define SIGNED_ARR_SIZE
#endif


#include "Matrix.h"
#include "Array.h"
#include "Vector2.h"


typedef struct st_grid_params_t
{
    arr_size_t surfaceSplitsNum;
    arr_size_t internalSplitsNum;
    arr_size_t infSplitsNum;
    double infMultiplier;
} STGridParams;


class SimpleTriangleGrid
{
public:
    SimpleTriangleGrid(const STGridParams& params);


    arr_size_t rowsNum() const;

    arr_size_t columnsNum() const;

    arr_size_t pointsNum() const;

    arr_size_t surfaceColumnsIndex() const;

    STGridParams parameters() const;


    const Matrix<Vector2<double>>& rawPoints() const;


    Vector2<double> operator()(arr_size_t row, arr_size_t columns) const;


    void generate(const Array<double>& surfacePointsR, const Array<double>& surfacePointsZ);

    void generate(const Array<Vector2<double>>& surfacePoints);

private:
    Matrix<Vector2<double>> mPoints;

    STGridParams mParams;
    arr_size_t mSurfaceColumnIndex;
};

#endif