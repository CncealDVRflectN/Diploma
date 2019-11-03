#ifndef DIPLOMA_FILES_UTIL_H
#define DIPLOMA_FILES_UTIL_H

#define SIGNED_ARR_SIZE // required for reverse loops such as for(arr_size_t i = something; i >= 0; i--),
                        // or you should wrap these loops in #ifdef SIGNED_ARR_SIZE, but that will reduce code readability

#include <string>
#include <filesystem>
#include <fstream>
#include "Matrix.h"


static const char COMMENT_CHARACTER = '#';
static const unsigned int COLUMN_WIDTH = 15;

static const std::string OUTPUT_DIR_NAME = "output";
static const std::string INTERMEDIATE_DIR_NAME = "intermediate";


typedef struct fluid_params_output_t
{
    std::string xLabel;
    std::string yLabel;
    double chi;
    double w;
} FluidParamsOutput;


typedef struct field_params_output_t
{
    std::string xLabel;
    std::string yLabel;
    double chi;
    int surfaceSplitsNum;
    int internalSplitsNum;
    int externalSplitsNum;
} FieldParamsOutput;


#pragma region Paths generation

inline std::filesystem::path output_path()
{
    std::filesystem::path result = std::filesystem::current_path() / OUTPUT_DIR_NAME;

    if (!std::filesystem::exists(result))
    {
        std::filesystem::create_directories(result);
    }

    return result;
}


inline std::filesystem::path intermediate_path()
{
    std::filesystem::path result = output_path() / INTERMEDIATE_DIR_NAME;

    if (!std::filesystem::exists(result))
    {
        std::filesystem::create_directories(result);
    }

    return result;
}


inline std::filesystem::path intermediate_file_path(const std::string& fileName)
{
    return intermediate_path() / fileName;
}

#pragma endregion


#pragma region Clear files

inline void clear_intermediate_files()
{
    std::filesystem::remove_all(intermediate_path());
}


inline void clear_intermediate_files(std::error_code& error)
{
    std::filesystem::remove_all(intermediate_path(), error);
}

#pragma endregion


#pragma region Names generation

template <typename T>
static std::string generate_file_postfix(const std::string& delimiter, const T& param)
{
    std::string result = delimiter;

    if constexpr (std::is_same<T, std::string>::value ||
                  std::is_same<T, char*>::value ||
                  std::is_same<T, const char*>::value)
    {
        result += param;
    }
    else
    {
        result += std::to_string(param);
    }

    return result;
}


template <typename T, typename... Args>
static std::string generate_file_postfix(const std::string& delimiter, const T& param, Args... params)
{
    return generate_file_postfix(delimiter, param) + generate_file_postfix(delimiter, params...);
}


template <typename... Args>
inline std::string generate_file_name(const std::string& prefix, 
                                      const std::string& delimiter, 
                                      const std::string& extension, 
                                      Args... params)
{
    return prefix + generate_file_postfix(delimiter, params...) + "." + extension;
}


template <typename... Args>
inline std::string generate_file_name(const std::string& prefix, const std::string& extension, Args... params)
{
    return generate_file_name(prefix, "-", extension, params...);
}

#pragma endregion


#pragma region Write data

template <typename T>
inline void write_data(std::ofstream& output, char fill, unsigned int width, const T& param)
{
    output << std::setfill(fill)
           << std::left
           << std::setw(width)
           << std::fixed
           << std::setprecision(8)
           << param;
}


template <typename T, typename... Args>
static void write_data(std::ofstream& output, char fill, unsigned int columnWidth, const T& param, Args... params)
{
    write_data(output, fill, columnWidth, param);
    write_data(output, fill, columnWidth, params...);
}


template <typename... Args>
static void write_data(std::ofstream& output, unsigned int columnWidth, Args... params)
{
    write_data(output, ' ', columnWidth, params...);
}


template <typename... Args>
inline void write_data_line(std::ofstream& output, unsigned int columnWidth, Args... params)
{
    write_data(output, columnWidth, params...);
    output << std::endl;
}


template <typename... Args>
inline void write_comment_line(std::ofstream& output, unsigned int columnWidth, Args... params)
{
    output << COMMENT_CHARACTER;
    write_data_line(output, columnWidth, params...);
}


static void write_fluid_data(const std::filesystem::path& path,
                             const FluidParamsOutput& fluidParams,
                             const Array<Vector2<double>>& points)
{
    std::ofstream output(path);

    write_comment_line(output, COLUMN_WIDTH, "Chi", "W");
    write_comment_line(output, COLUMN_WIDTH, fluidParams.chi, fluidParams.w);
    write_comment_line(output, COLUMN_WIDTH, "########");
    write_comment_line(output, COLUMN_WIDTH, fluidParams.xLabel, fluidParams.yLabel);

    for (Vector2<double> point : points)
    {
        write_data_line(output, COLUMN_WIDTH, point.x, point.y);
    }

    output.flush();
    output.close();
}


inline void write_fluid_data(const FluidParamsOutput& fluidParams, const Array<Vector2<double>>& points)
{
    std::string fileName = generate_file_name("fluid", "dat", fluidParams.chi, fluidParams.w);
    std::filesystem::path outputPath = intermediate_file_path(fileName);

    write_fluid_data(outputPath, fluidParams, points);
}


static void write_field_data(const std::filesystem::path& path,
                             const FieldParamsOutput& fieldParams,
                             const Matrix<Vector2<double>>& points,
                             const Matrix<double>& field)
{
    std::ofstream output(path);

    arr_size_t rowsNum = points.rowsNum();
    arr_size_t columnsNum = points.columnsNum();

    write_comment_line(output, COLUMN_WIDTH, "Chi");
    write_comment_line(output, COLUMN_WIDTH, fieldParams.chi);
    write_comment_line(output, COLUMN_WIDTH, "########");
    write_comment_line(output, COLUMN_WIDTH, fieldParams.xLabel, fieldParams.yLabel, "field");

    for (arr_size_t i = 0; i < rowsNum; i++)
    {
        for (arr_size_t j = 0; j < columnsNum; j++)
        {
            write_data_line(output, COLUMN_WIDTH, points(i, j).x, points(i, j).y, field(i, j));
        }
    }

    output.flush();
    output.close();
}


inline void write_field_data(const FieldParamsOutput& fieldParams,
                             const Matrix<Vector2<double>>& points,
                             const Matrix<double>& field)
{
    std::string fileName = generate_file_name("field", "dat", fieldParams.chi);
    std::filesystem::path outputPath = intermediate_file_path(fileName);

    write_field_data(outputPath, fieldParams, points, field);
}


static void write_internal_grid_data(const std::filesystem::path& path,
                                     const FieldParamsOutput& fieldParams,
                                     const Matrix<Vector2<double>>& points)
{
    std::ofstream output(path);

    arr_size_t rowsNum = points.rowsNum();
    arr_size_t columnsNum = points.columnsNum();
    arr_size_t surfaceColumnIndex = fieldParams.internalSplitsNum;

    write_comment_line(output, COLUMN_WIDTH, "Chi");
    write_comment_line(output, COLUMN_WIDTH, fieldParams.chi);
    write_comment_line(output, COLUMN_WIDTH, "########");
    write_comment_line(output, COLUMN_WIDTH, fieldParams.xLabel, fieldParams.yLabel);

    for (arr_size_t i = 0; i < rowsNum; i++)
    {
        for (arr_size_t j = 0; j <= surfaceColumnIndex; j++)
        {
            write_data_line(output, COLUMN_WIDTH, points(i, j).x, points(i, j).y);

            if (i != rowsNum - 1 && j != surfaceColumnIndex)
            {
                write_data_line(output, COLUMN_WIDTH, points(i + 1, j).x, points(i + 1, j).y);
                write_data_line(output, COLUMN_WIDTH, points(i, j + 1).x, points(i, j + 1).y);
                write_data_line(output, COLUMN_WIDTH, points(i, j).x, points(i, j).y);
            }
            else if (i != rowsNum - 1)
            {
                for (arr_size_t k = surfaceColumnIndex; k >= 0; k--)
                {
                    write_data_line(output, COLUMN_WIDTH, points(i + 1, k).x, points(i + 1, k).y);
                }
            }
        }
    }

    output.flush();
    output.close();
}


inline void write_internal_grid_data(const FieldParamsOutput& fieldParams,
                                     const Matrix<Vector2<double>>& points)
{
    std::string fileName = generate_file_name("grid", "dat", "internal", fieldParams.chi);
    std::filesystem::path outputPath = intermediate_file_path(fileName);

    write_internal_grid_data(outputPath, fieldParams, points);
}


static void write_external_grid_data(const std::filesystem::path& path,
                                     const FieldParamsOutput& fieldParams,
                                     const Matrix<Vector2<double>>& points)
{
    std::ofstream output(path);

    arr_size_t rowsNum = points.rowsNum();
    arr_size_t columnsNum = points.columnsNum();
    arr_size_t surfaceColumnIndex = fieldParams.internalSplitsNum;

    write_comment_line(output, COLUMN_WIDTH, "Chi");
    write_comment_line(output, COLUMN_WIDTH, fieldParams.chi);
    write_comment_line(output, COLUMN_WIDTH, "########");
    write_comment_line(output, COLUMN_WIDTH, fieldParams.xLabel, fieldParams.yLabel);

    for (arr_size_t i = 0; i < rowsNum; i++)
    {
        for (arr_size_t j = surfaceColumnIndex; j < columnsNum; j++)
        {
            write_data_line(output, COLUMN_WIDTH, points(i, j).x, points(i, j).y);

            if (i != rowsNum - 1 && j != columnsNum - 1)
            {
                write_data_line(output, COLUMN_WIDTH, points(i + 1, j).x, points(i + 1, j).y);
                write_data_line(output, COLUMN_WIDTH, points(i, j + 1).x, points(i, j + 1).y);
                write_data_line(output, COLUMN_WIDTH, points(i, j).x, points(i, j).y);
            }
            else if (i != rowsNum - 1)
            {
                for (arr_size_t k = columnsNum - 1; k >= surfaceColumnIndex; k--)
                {
                    write_data_line(output, COLUMN_WIDTH, points(i + 1, k).x, points(i + 1, k).y);
                }
            }
        }
    }

    output.flush();
    output.close();
}


inline void write_external_grid_data(const FieldParamsOutput& fieldParams,
                                     const Matrix<Vector2<double>>& points)
{
    std::string fileName = generate_file_name("grid", "dat", "external", fieldParams.chi);
    std::filesystem::path outputPath = intermediate_file_path(fileName);

    write_external_grid_data(outputPath, fieldParams, points);
}

#pragma endregion

#endif