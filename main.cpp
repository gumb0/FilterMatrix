#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace
{
    const char Delimiter1 = ' ';
    const char Delimiter2 = ',';

    char detectDelimiter(std::ifstream& input)
    {
        std::string line;
        std::getline(input, line);
        if (!input)
            throw std::runtime_error("Invalid input file.");

        const char delimiter = line.find(Delimiter1) != std::string::npos ? Delimiter1 : Delimiter2;

        input.seekg(0);
        if (!input)
            throw std::runtime_error("Failed to rewind input file.");

        return delimiter;
    }

    std::vector<int> parseLine(const std::string& line, char delimiter)
    {
        std::istringstream lineStream(line);

        std::vector<int> values;
        std::string value;
        while (std::getline(lineStream, value, delimiter))
        {
            values.push_back(std::stoi(value));
        }

        return values;
    }

    std::vector<int> readLine(std::ifstream& input, char delimiter)
    {
        std::string line;
        std::getline(input, line);

        return parseLine(line, delimiter);
    }

    int interpolateNeighbors(const std::vector<int>& prevLine, const std::vector<int>& line, const std::vector<int>& nextLine, size_t i)
    {
        int neighbourSum = 0;
        int neighborCount = 0;
        if (i > 0)
        {
            neighbourSum += line[i - 1];
            ++neighborCount;
        }
        if (i < line.size() - 1)
        {
            neighbourSum += line[i + 1];
            ++neighborCount;
        }
        if (!prevLine.empty())
        {
            neighbourSum += prevLine[i];
            ++neighborCount;
        }
        if (!nextLine.empty())
        {
            neighbourSum += nextLine[i];
            ++neighborCount;
        }

        return neighbourSum / neighborCount;
    }
        
    std::vector<int> filterLine(const std::vector<int>& prevLine, const std::vector<int>& line, const std::vector<int>& nextLine)
    {
        std::vector<int> filteredLine(line.size());
        
        for (size_t i = 0; i < line.size(); ++i)
        {
            filteredLine[i] = line[i] == 0 ? interpolateNeighbors(prevLine, line, nextLine, i) : line[i];
        }

        return filteredLine;
    }

    void writeLine(const std::vector<int>& line, std::ofstream& output, char delimiter)
    {
        for (auto it = line.cbegin(); it != line.cend(); ++it)
        {
            if (it != line.cbegin())
                output << delimiter;
            output << *it;
        }
        output << "\n";

        if (!output)
            throw std::runtime_error("Failed to write line to output file.");
    }

    void processLines(std::ifstream& input, std::ofstream& output, char delimiter)
    {
        std::vector<int> prevLine;
        std::vector<int> line = readLine(input, delimiter);

        while (input)
        {
            std::vector<int> nextLine = readLine(input, delimiter);

            const std::vector<int> filteredLine = filterLine(prevLine, line, nextLine);
            writeLine(filteredLine, output, delimiter);

            prevLine = std::move(line);
            line = std::move(nextLine);
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        std::cerr << "USAGE: filter input_file output_file\n";
        return -1;
    }

    try
    {
        std::ifstream input(argv[1]);
        if (!input)
            throw std::runtime_error("Failed to open input file.");

        std::ofstream output(argv[2]);
        if (!output)
            throw std::runtime_error("Failed to open output file.");

        const char delimiter = detectDelimiter(input);

        processLines(input, output, delimiter);

        std::cout << "Output written to " << argv[2] << "\n";
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error occured: " << e.what() << '\n';
        return -1;
    }

    return 0;
}