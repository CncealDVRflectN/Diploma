#include "RightSweep.h"


#pragma region Constructors

RightSweep::RightSweep(arr_size_t size, bool isPedantic) : mLowerDiagonal(size - 1), mMainDiagonal(size), 
                                                           mUpperDiagonal(size - 1), mConstTerms(size),
                                                           mAlpha(size - 1), mBeta(size), 
                                                           mSize(size), mIsPedantic(isPedantic) {}

#pragma endregion


#pragma region Right sweep parameters

arr_size_t RightSweep::size()
{
    return mSize;
}

#pragma endregion


#pragma region Access operators

double& RightSweep::operator()(RightSweepAccess accessType, arr_size_t index)
{
    switch (accessType)
    {
        case RS_LOWER_DIAGONAL:
            assert_message(index >= 0 && index < mLowerDiagonal.size(), "Right sweep lower diagonal index out of bounds");
            return mLowerDiagonal(index);
        case RS_MAIN_DIAGONAL:
            assert_message(index >= 0 && index < mMainDiagonal.size(), "Right sweep main diagonal index out of bounds");
            return mMainDiagonal(index);
        case RS_UPPER_DIAGONAL:
            assert_message(index >= 0 && index < mUpperDiagonal.size(), "Right sweep upper diagonal index out of bounds");
            return mUpperDiagonal(index);
        case RS_CONST_TERMS:
            assert_message(index >= 0 && index < mConstTerms.size(), "Right sweep vector index out of bounds");
            return mConstTerms(index);
        default:
            assert_message(false, "Right sweep unknown access type");
            return mMainDiagonal(-1);
    }
}

#pragma endregion


#pragma region Public solve methods

Array<double> RightSweep::solve()
{
    Array<double> solution(mSize);

    this->solve(solution);

    return solution;
}


void RightSweep::solve(Array<double>& solutionDest)
{
    assert_message(mSize == solutionDest.size(), 
                   "Right sweep solution cannot be calculated due to different size of solution destination");

    if (!isValid())
    {
        if (mIsPedantic)
        {
            assert_message(false, "Right sweep matrix is invalid!");
        }
        else
        {
            printf("! Warning: Right sweep matrix is invalid ! \n");
        }
    }

    calcAlpha();
    calcBeta();
    reversal(solutionDest);
}

#pragma endregion


#pragma region Private calculation methods

void RightSweep::calcAlpha()
{
    arr_size_t limit = mSize - 1;

    mAlpha(0) = -mUpperDiagonal(0) / mMainDiagonal(0);

    #pragma loop(no_parallel)
    #pragma loop(no_vector)
    for (arr_size_t i = 1; i < limit; i++)
    {
        mAlpha(i) = -mUpperDiagonal(i) / (mMainDiagonal(i) + mLowerDiagonal(i - 1) * mAlpha(i - 1));
    }
}


void RightSweep::calcBeta()
{
    mBeta(0) = mConstTerms(0) / mMainDiagonal(0);

    #pragma loop(no_parallel)
    #pragma loop(no_vector)
    for (arr_size_t i = 1; i < mSize; i++)
    {
        mBeta(i) = (mConstTerms(i) - mLowerDiagonal(i - 1) * mBeta(i - 1)) /
                   (mMainDiagonal(i) + mLowerDiagonal(i - 1) * mAlpha(i - 1));
    }
}


void RightSweep::reversal(Array<double>& solutionDest)
{
    solutionDest(mSize - 1) = mBeta(mSize - 1);

    #pragma loop(no_parallel)
    #pragma loop(no_vector)
    for (arr_size_t i = mSize - 2; i >= 0; i--)
    {
        solutionDest(i) = mAlpha(i) * solutionDest(i + 1) + mBeta(i);
    }
}

#pragma endregion


#pragma region Validation

bool RightSweep::isValid()
{
    if (std::abs(mMainDiagonal(0)) < std::fabs(mUpperDiagonal(0)))
    {
        return false;
    }

    if (std::abs(mMainDiagonal(mSize - 1)) < std::abs(mLowerDiagonal(mSize - 2)))
    {
        return false;
    }

    arr_size_t lowerDiagSize = mSize - 1;
    for (int i = 1; i < lowerDiagSize; i++)
    {
        if (std::abs(mMainDiagonal(i)) < (std::abs(mLowerDiagonal(i - 1)) + std::abs(mUpperDiagonal(i))))
        {
            return false;
        }
    }

    return true;
}

#pragma endregion
