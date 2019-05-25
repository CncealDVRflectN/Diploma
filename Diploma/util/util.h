#ifndef DIPLOMA_UTIL_H
#define DIPLOMA_UTIL_H

#define _USE_MATH_DEFINES

#include <math.h>
#include <limits>


#ifdef WIN32
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif


// Vector2

typedef struct vector2_t
{
	double r;
	double z;
} Vector2;


typedef struct indices2_t
{
	int i;
	int j;
} Indices2;


static inline double norm(const double* next, const double* prev, int size)
{
	double max = -std::numeric_limits<double>::infinity();
	double tmp = 0.0;

	for (int i = 0; i < size; i++)
	{
		tmp = fabs(next[i] - prev[i]);

		if (tmp > max)
		{
			max = tmp;
		}
	}

	return max;
}

static inline double norm(double** next, double** prev, int linesNum, int columnsNum)
{
	double max = -std::numeric_limits<double>::infinity();
	double tmp = 0.0;

	for (int i = 0; i < linesNum; i++)
	{
		for (int j = 0; j < columnsNum; j++)
		{
			tmp = fabs(next[i][j] - prev[i][j]);

			if (tmp > max)
			{
				max = tmp;
			}
		}
	}

	return max;
}

static inline double calcLerp(double begin, double end, double param)
{
	return begin + param * (end - begin);
}

static inline void relaxation(double* nextApprox, const double* curApprox, int size, double relaxationCoef)
{
	for (int i = 0; i < size; i++)
	{
		nextApprox[i] = calcLerp(curApprox[i], nextApprox[i], relaxationCoef);
	}
}

static inline void relaxation(double** nextApprox, double** curApprox, int linesNum, int columnsNum, double relaxationCoef)
{
	for (int i = 0; i < linesNum; i++)
	{
		for (int j = 0; j < columnsNum; j++)
		{
			nextApprox[i][j] = calcLerp(curApprox[i][j], nextApprox[i][j], relaxationCoef);
		}
	}
}

static inline Vector2 calcLerp(const Vector2 &begin, const Vector2 &end, double param)
{
	return { calcLerp(begin.r, end.r, param), calcLerp(begin.z, end.z, param) };
}

static inline double calcDoubleTriangleArea(const Vector2& vert1, const Vector2& vert2, const Vector2& vert3)
{
	return (vert1.r - vert2.r) * (vert3.z - vert2.z) - (vert3.r - vert2.r) * (vert1.z - vert2.z);
}

static inline Vector2 calcParametricPoint(const Vector2* linePoints, int pointsNum, double param)
{
	int prevIndex = (int)((pointsNum - 1) * param);
	int nextIndex = prevIndex + 1;

	if (prevIndex >= pointsNum - 1)
	{
		prevIndex = pointsNum - 1;
		nextIndex = prevIndex;
	}

	if (prevIndex < 0)
	{
		prevIndex = 0;
	}

	if (nextIndex < 0)
	{
		nextIndex = 0;
	}

	if (prevIndex == nextIndex)
	{
		return linePoints[prevIndex];
	}

	double prevParam = (double)prevIndex / (pointsNum - 1);
	double nextParam = (double)nextIndex / (pointsNum - 1);
	double localParam = (param - prevParam) / (nextParam - prevParam);

	return calcLerp(linePoints[prevIndex], linePoints[nextIndex], localParam);
}


// Args parsing

typedef struct short_opt_t
{
	char arg;
	int id;
} ShortOpt;

typedef struct long_opt_t
{
	char *arg;
	int id;
} LongOpt;


char* compareShortOpt(char *opt, const ShortOpt *shortOpt);

char* compareLongOpt(char *opt, const LongOpt *longOpt);

char* getShortOpt(char *opt, const ShortOpt *shortOpts, int &id);

char* getLongOpt(char *opt, const LongOpt *longOpts, int &id);

char* getOpt(char *arg, const ShortOpt *shortOpts, const LongOpt *longOpts, int &id);

#endif //DIPLOMA_UTIL_H