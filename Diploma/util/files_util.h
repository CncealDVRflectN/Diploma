#ifndef DIPLOMA_FILES_UTIL_H
#define DIPLOMA_FILES_UTIL_H

#define SIGNED_ARR_SIZE // required for simple reverse loops such as for(arr_size_t i = something; i >= 0; i--),
                        // or you should wrap these loops in #ifdef SIGNED_ARR_SIZE and add version for unsigned type, 
                        // but that will reduce code readability

#include <string>
#include <filesystem>
#include <fstream>
#include <sstream>
#include "Matrix.h"


const char COMMENT_CHARACTER = '#';
const unsigned int COLUMN_WIDTH = 20;
const unsigned int DATA_LINE_OFFSET = 1;

const std::string OUTPUT_DIR_NAME = "output";
const std::string INTERMEDIATE_DIR_NAME = "intermediate";
const std::string RESOURCES_DIR_NAME = "resources";
const std::string PLOT_CONFIGS_DIR_NAME = "plot-configs";


typedef struct fluid_result_params_t
{
    std::string xLabel;
    std::string yLabel;
    double chi;
    double w;
    bool isDimensionless;
} FluidResultParams;


typedef struct field_result_params_t
{
    std::string xLabel;
    std::string yLabel;
    std::string potentialLabel;
    double potentialMin;
    double potentialMax;
    double fluidTopPotential;
    double chi;
    int surfaceSplitsNum;
    int internalSplitsNum;
    int externalSplitsNum;
    bool isDimensionless;
} FieldResultParams;


typedef struct field_model_params_t
{
    std::string xLabel;
    std::string yLabel;
    std::string errorLabel;
    double errorMin;
    double errorMax;
    double chi;
    int surfaceSplitsNum;
    int internalSplitsNum;
    int externalSplitsNum;
    bool isDimensionless;
} FieldModelParams;


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


inline std::filesystem::path resources_path()
{
    return std::filesystem::current_path() / RESOURCES_DIR_NAME;
}


inline std::filesystem::path resource_path(const std::string& resourceName)
{
    return resources_path() / resourceName;
}


inline std::filesystem::path plot_configs_path()
{
    return resources_path() / PLOT_CONFIGS_DIR_NAME;
}


inline std::filesystem::path plot_config_path(const std::string& plotConfig)
{
    return plot_configs_path() / plotConfig;
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
        std::stringstream stream;
        stream << std::fixed << std::setprecision(3) << param;
        result += stream.str();
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


inline std::string generate_file_name(const std::string& prefix,
                                      const std::string& extension,
                                      const std::string& delimiter)
{
    return prefix + '.' + extension;
}


template <typename... Args>
inline std::string generate_file_name(const std::string& prefix, 
                                      const std::string& extension, 
                                      const std::string& delimiter, 
                                      Args... params)
{
    return prefix + generate_file_postfix(delimiter, params...) + '.' + extension;
}

#pragma endregion


#pragma region Write data

template <typename T>
inline std::ofstream& write_data(std::ofstream& output, char fill, unsigned int width, const T& param)
{
    output << std::setfill(fill)
           << std::left
           << std::setw(width)
           << std::fixed
           << std::scientific
           << std::boolalpha
           << param;

    return output;
}


template <typename T, typename... Args>
static std::ofstream& write_data(std::ofstream& output, char fill, unsigned int columnWidth, const T& param, Args... params)
{
    write_data(output, fill, columnWidth, param);
    write_data(output, fill, columnWidth, params...);

    return output;
}


template <typename... Args>
static std::ofstream& write_data(std::ofstream& output, unsigned int columnWidth, Args... params)
{
    write_data(output, ' ', columnWidth, params...);

    return output;
}


template <typename... Args>
inline std::ofstream& write_data_line(std::ofstream& output, unsigned int columnWidth, Args... params)
{
    write_data(output, columnWidth, params...);
    output << std::endl;

    return output;
}


template <typename... Args>
inline std::ofstream& write_offsetted_data_line(std::ofstream& output, unsigned int offset, unsigned int columnWidth, Args... params)
{
    for (unsigned int i = 0U; i < offset; i++)
    {
        output << ' ';
    }

    write_data_line(output, columnWidth, params...);

    return output;
}


template <typename... Args>
inline std::ofstream& write_comment_line(std::ofstream& output, unsigned int columnWidth, Args... params)
{
    output << COMMENT_CHARACTER;
    write_data_line(output, columnWidth, params...);

    return output;
}


static void write_fluid_data(const std::filesystem::path& path,
                             const FluidResultParams& fluidParams,
                             const Array<Vector2<double>>& points) noexcept(false)
{
    std::ofstream output(path);

    if (!output.good())
    {
        throw std::runtime_error("Cannot write data to file: " + path.string());
    }

    write_comment_line(output, COLUMN_WIDTH, "Chi", "W", "Dimensionless", "X label", "Y label");
    write_comment_line(output, COLUMN_WIDTH, fluidParams.chi, 
                                             fluidParams.w, 
                                             fluidParams.isDimensionless,
                                             fluidParams.xLabel, 
                                             fluidParams.yLabel);
    write_data(output, '#', 5 * COLUMN_WIDTH, "") << std::endl;
    write_comment_line(output, COLUMN_WIDTH, fluidParams.xLabel, fluidParams.yLabel);

    for (Vector2<double> point : points)
    {
        write_offsetted_data_line(output, DATA_LINE_OFFSET, COLUMN_WIDTH, point.x, point.y);
    }

    output.flush();
    output.close();
}


inline std::filesystem::path write_fluid_data(const FluidResultParams& fluidParams, 
                                              const Array<Vector2<double>>& points)
{
    std::string fileName = generate_file_name("fluid", "dat", "-", fluidParams.chi, fluidParams.w);
    std::filesystem::path outputPath = intermediate_file_path(fileName);

    write_fluid_data(outputPath, fluidParams, points);

    return outputPath;
}


static void write_field_data(const std::filesystem::path& path,
                             const FieldResultParams& fieldParams,
                             const Matrix<Vector2<double>>& points,
                             const Matrix<double>& field) noexcept(false)
{
    std::ofstream output(path);

    if (!output.good())
    {
        throw std::runtime_error("Cannot write data to file: " + path.string());
    }

    arr_size_t rowsNum = points.rowsNum();
    arr_size_t columnsNum = points.columnsNum();

    write_comment_line(output, COLUMN_WIDTH, "Chi", 
                                             "Surface splits", 
                                             "Internal splits", 
                                             "External splits", 
                                             "Potential min", 
                                             "Potential max", 
                                             "Fluid top potential", 
                                             "Dimensionless", 
                                             "X Label", 
                                             "Y Label", 
                                             "Potential label");
    write_comment_line(output, COLUMN_WIDTH, fieldParams.chi, 
                                             fieldParams.surfaceSplitsNum, 
                                             fieldParams.internalSplitsNum, 
                                             fieldParams.externalSplitsNum, 
                                             fieldParams.potentialMin, 
                                             fieldParams.potentialMax, 
                                             fieldParams.fluidTopPotential, 
                                             fieldParams.isDimensionless, 
                                             fieldParams.xLabel, 
                                             fieldParams.yLabel, 
                                             fieldParams.potentialLabel);
    write_data(output, '#', 11 * COLUMN_WIDTH, "") << std::endl;
    write_comment_line(output, COLUMN_WIDTH, fieldParams.xLabel, fieldParams.yLabel, fieldParams.potentialLabel);

    for (arr_size_t i = 0; i < rowsNum; i++)
    {
        for (arr_size_t j = 0; j < columnsNum; j++)
        {
            write_offsetted_data_line(output, DATA_LINE_OFFSET, COLUMN_WIDTH, points(i, j).x, points(i, j).y, field(i, j));
        }
    }

    output.flush();
    output.close();
}


inline std::filesystem::path write_field_data(const FieldResultParams& fieldParams,
                                              const Matrix<Vector2<double>>& points,
                                              const Matrix<double>& field)
{
    std::string fileName = generate_file_name("field", "dat", "-", fieldParams.chi);
    std::filesystem::path outputPath = intermediate_file_path(fileName);

    write_field_data(outputPath, fieldParams, points, field);

    return outputPath;
}


static void write_field_error_data(const std::filesystem::path& path,
                                   const FieldModelParams& modelParams,
                                   const Matrix<Vector2<double>>& points,
                                   const Matrix<double>& error)
{
    std::ofstream output(path);

    if (!output.good())
    {
        throw std::runtime_error("Cannot write data to file: " + path.string());
    }

    arr_size_t rowsNum = points.rowsNum();
    arr_size_t columnsNum = points.columnsNum();

    write_comment_line(output, COLUMN_WIDTH, "Chi",
                                             "Surface splits",
                                             "Internal splits",
                                             "External splits",
                                             "Error min",
                                             "Error max",
                                             "Dimensionless",
                                             "X Label",
                                             "Y Label",
                                             "Error label");
    write_comment_line(output, COLUMN_WIDTH, modelParams.chi,
                                             modelParams.surfaceSplitsNum,
                                             modelParams.internalSplitsNum,
                                             modelParams.externalSplitsNum,
                                             modelParams.errorMin,
                                             modelParams.errorMax,
                                             modelParams.isDimensionless,
                                             modelParams.xLabel,
                                             modelParams.yLabel,
                                             modelParams.errorLabel);
    write_data(output, '#', 10 * COLUMN_WIDTH, "") << std::endl;
    write_comment_line(output, COLUMN_WIDTH, modelParams.xLabel, modelParams.yLabel, modelParams.errorLabel);

    for (arr_size_t i = 0; i < rowsNum; i++)
    {
        for (arr_size_t j = 0; j < columnsNum; j++)
        {
            write_offsetted_data_line(output, DATA_LINE_OFFSET, COLUMN_WIDTH, points(i, j).x, points(i, j).y, error(i, j));
        }
    }

    output.flush();
    output.close();
}


inline std::filesystem::path write_field_error_data(const FieldModelParams& modelParams,
                                                    const Matrix<Vector2<double>>& points,
                                                    const Matrix<double>& error)
{
    std::string fileName = generate_file_name("field", "dat", "-", "error", modelParams.chi);
    std::filesystem::path outputPath = intermediate_file_path(fileName);

    write_field_error_data(outputPath, modelParams, points, error);

    return outputPath;
}


static void write_internal_grid_data(const std::filesystem::path& path,
                                     const FieldResultParams& fieldParams,
                                     const Matrix<Vector2<double>>& points) noexcept(false)
{
    std::ofstream output(path);

    if (!output.good())
    {
        throw std::runtime_error("Cannot write data to file: " + path.string());
    }

    arr_size_t rowsNum = points.rowsNum();
    arr_size_t columnsNum = points.columnsNum();
    arr_size_t surfaceColumnIndex = fieldParams.internalSplitsNum;

    write_comment_line(output, COLUMN_WIDTH, "Chi",
                                             "Surface splits",
                                             "Internal splits",
                                             "External splits", 
                                             "Dimensionless", 
                                             "X Label",
                                             "Y Label");
    write_comment_line(output, COLUMN_WIDTH, fieldParams.chi,
                                             fieldParams.surfaceSplitsNum,
                                             fieldParams.internalSplitsNum,
                                             fieldParams.externalSplitsNum, 
                                             fieldParams.isDimensionless, 
                                             fieldParams.xLabel,
                                             fieldParams.yLabel);
    write_data(output, '#', 7 * COLUMN_WIDTH, "") << std::endl;
    write_comment_line(output, COLUMN_WIDTH, fieldParams.xLabel, fieldParams.yLabel);

    for (arr_size_t i = 0; i < rowsNum; i++)
    {
        for (arr_size_t j = 0; j <= surfaceColumnIndex; j++)
        {
            write_offsetted_data_line(output, DATA_LINE_OFFSET, COLUMN_WIDTH, points(i, j).x, points(i, j).y);

            if (i != rowsNum - 1 && j != surfaceColumnIndex)
            {
                write_offsetted_data_line(output, DATA_LINE_OFFSET, COLUMN_WIDTH, points(i + 1, j).x, points(i + 1, j).y);
                write_offsetted_data_line(output, DATA_LINE_OFFSET, COLUMN_WIDTH, points(i, j + 1).x, points(i, j + 1).y);
                write_offsetted_data_line(output, DATA_LINE_OFFSET, COLUMN_WIDTH, points(i, j).x, points(i, j).y);
            }
            else if (i != rowsNum - 1)
            {
                for (arr_size_t k = surfaceColumnIndex; k >= 0; k--)
                {
                    write_offsetted_data_line(output, DATA_LINE_OFFSET, COLUMN_WIDTH, points(i + 1, k).x, points(i + 1, k).y);
                }
            }
        }
    }

    output.flush();
    output.close();
}


inline std::filesystem::path write_internal_grid_data(const FieldResultParams& fieldParams,
                                                      const Matrix<Vector2<double>>& points)
{
    std::string fileName = generate_file_name("grid", "dat", "-", "internal", fieldParams.chi);
    std::filesystem::path outputPath = intermediate_file_path(fileName);

    write_internal_grid_data(outputPath, fieldParams, points);

    return outputPath;
}


static void write_external_grid_data(const std::filesystem::path& path,
                                     const FieldResultParams& fieldParams,
                                     const Matrix<Vector2<double>>& points) noexcept(false)
{
    std::ofstream output(path);

    if (!output.good())
    {
        throw std::runtime_error("Cannot write data to file: " + path.string());
    }

    arr_size_t rowsNum = points.rowsNum();
    arr_size_t columnsNum = points.columnsNum();
    arr_size_t surfaceColumnIndex = fieldParams.internalSplitsNum;

    write_comment_line(output, COLUMN_WIDTH, "Chi",
                                             "Surface splits",
                                             "Internal splits",
                                             "External splits", 
                                             "Dimensionless", 
                                             "X Label",
                                             "Y Label");
    write_comment_line(output, COLUMN_WIDTH, fieldParams.chi,
                                             fieldParams.surfaceSplitsNum,
                                             fieldParams.internalSplitsNum,
                                             fieldParams.externalSplitsNum, 
                                             fieldParams.isDimensionless, 
                                             fieldParams.xLabel,
                                             fieldParams.yLabel);
    write_data(output, '#', 7 * COLUMN_WIDTH, "") << std::endl;
    write_comment_line(output, COLUMN_WIDTH, fieldParams.xLabel, fieldParams.yLabel);

    for (arr_size_t i = 0; i < rowsNum; i++)
    {
        for (arr_size_t j = surfaceColumnIndex; j < columnsNum; j++)
        {
            write_offsetted_data_line(output, DATA_LINE_OFFSET, COLUMN_WIDTH, points(i, j).x, points(i, j).y);

            if (i != rowsNum - 1 && j != columnsNum - 1)
            {
                write_offsetted_data_line(output, DATA_LINE_OFFSET, COLUMN_WIDTH, points(i + 1, j).x, points(i + 1, j).y);
                write_offsetted_data_line(output, DATA_LINE_OFFSET, COLUMN_WIDTH, points(i, j + 1).x, points(i, j + 1).y);
                write_offsetted_data_line(output, DATA_LINE_OFFSET, COLUMN_WIDTH, points(i, j).x, points(i, j).y);
            }
            else if (i != rowsNum - 1)
            {
                for (arr_size_t k = columnsNum - 1; k >= surfaceColumnIndex; k--)
                {
                    write_offsetted_data_line(output, DATA_LINE_OFFSET, COLUMN_WIDTH, points(i + 1, k).x, points(i + 1, k).y);
                }
            }
        }
    }

    output.flush();
    output.close();
}


inline std::filesystem::path write_external_grid_data(const FieldResultParams& fieldParams,
                                                      const Matrix<Vector2<double>>& points)
{
    std::string fileName = generate_file_name("grid", "dat", "-", "external", fieldParams.chi);
    std::filesystem::path outputPath = intermediate_file_path(fileName);

    write_external_grid_data(outputPath, fieldParams, points);

    return outputPath;
}


static void write_height_coefs_data(const std::filesystem::path& path, 
                                    double chi, 
                                    const std::vector<Vector2<double>>& coefs)
{
    std::ofstream output(path);

    if (!output.good())
    {
        throw std::runtime_error("Cannot write data to file: " + path.string());
    }

    write_comment_line(output, COLUMN_WIDTH, "Chi");
    write_comment_line(output, COLUMN_WIDTH, chi);
    write_data(output, '#', COLUMN_WIDTH, "") << std::endl;
    write_comment_line(output, COLUMN_WIDTH, "W", "k");

    for (Vector2<double> coef : coefs)
    {
        write_offsetted_data_line(output, DATA_LINE_OFFSET, COLUMN_WIDTH, coef.x, coef.y);
    }

    output.flush();
    output.close();
}


inline std::filesystem::path write_height_coefs_data(double chi, const std::vector<Vector2<double>>& coefs)
{
    std::string fileName = generate_file_name("height-coefs", "dat", "-", chi);
    std::filesystem::path outputPath = intermediate_file_path(fileName);

    write_height_coefs_data(outputPath, chi, coefs);

    return outputPath;
}

#pragma endregion


#pragma region Read data

static FluidResultParams read_fluid_params(const std::filesystem::path& path) noexcept(false)
{
    std::ifstream input(path);

    if (!input.good())
    {
        throw std::runtime_error("Cannot read data from file: " + path.string());
    }

    FluidResultParams result;

    input.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // skipping first header line
    input.ignore(std::numeric_limits<std::streamsize>::max(), COMMENT_CHARACTER); // skipping comment characters

    input >> std::boolalpha;

    input >> result.chi >> result.w >> result.isDimensionless >> result.xLabel >> result.yLabel;

    input.close();

    return result;
}


static FieldResultParams read_field_params(const std::filesystem::path& path) noexcept(false)
{
    std::ifstream input(path);

    if (!input.good())
    {
        throw std::runtime_error("Cannot read data from file: " + path.string());
    }

    FieldResultParams result;

    input.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // skipping first header line
    input.ignore(std::numeric_limits<std::streamsize>::max(), COMMENT_CHARACTER); // skipping comment characters

    input >> std::boolalpha;

    input >> result.chi >> result.surfaceSplitsNum >> result.internalSplitsNum >> result.externalSplitsNum 
          >> result.potentialMin >> result.potentialMax >> result.fluidTopPotential 
          >> result.isDimensionless >> result.xLabel >> result.yLabel >> result.potentialLabel;

    input.close();

    return result;
}


static FieldModelParams read_field_model_params(const std::filesystem::path& path) noexcept(false)
{
    std::ifstream input(path);

    if (!input.good())
    {
        throw std::runtime_error("Cannot read data from file: " + path.string());
    }

    FieldModelParams result;

    input.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // skipping first header line
    input.ignore(std::numeric_limits<std::streamsize>::max(), COMMENT_CHARACTER); // skipping comment characters

    input >> std::boolalpha;

    input >> result.chi >> result.surfaceSplitsNum >> result.internalSplitsNum >> result.externalSplitsNum
          >> result.errorMin >> result.errorMax
          >> result.isDimensionless >> result.xLabel >> result.yLabel >> result.errorLabel;

    input.close();

    return result;
}


static FieldResultParams read_field_grid_params(const std::filesystem::path& path) noexcept(false)
{
    std::ifstream input(path);

    if (!input.good())
    {
        throw std::runtime_error("Cannot read data from file: " + path.string());
    }

    FieldResultParams result;

    input.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // skipping first header line
    input.ignore(std::numeric_limits<std::streamsize>::max(), COMMENT_CHARACTER); // skipping comment characters

    input >> std::boolalpha;

    input >> result.chi >> result.surfaceSplitsNum >> result.internalSplitsNum
          >> result.externalSplitsNum >> result.isDimensionless >> result.xLabel >> result.yLabel;

    input.close();

    return result;
}


static double read_height_coefs_params(const std::filesystem::path& path) noexcept(false)
{
    std::ifstream input(path);

    if(!input.good())
    {
        throw std::runtime_error("Cannot read data from file: " + path.string());
    }

    double result;

    input.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // skipping first header line
    input.ignore(std::numeric_limits<std::streamsize>::max(), COMMENT_CHARACTER); // skipping comment characters

    input >> result;

    input.close();

    return result;
}

#pragma endregion

#endif