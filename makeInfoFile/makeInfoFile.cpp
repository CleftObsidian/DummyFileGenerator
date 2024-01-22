#include "tinyxml2.h"
#include <random>
#include <string>
#include <algorithm>
#include <iostream>
#include <filesystem>

using namespace tinyxml2;
namespace fs = std::filesystem;

void MakeFileListXML(const size_t numOfFiles,
					 const size_t minFileSizeByKB,
					 const size_t maxFileSizeByKB,
					 const float lamdaOfExpDistributionForFileSize,
					 const float minProcessingTimeByms,
					 const float maxProcessingTimeByms,
					 const float lamdaOfExpDistributionForProcessingTime,
					 const size_t maxNumOfDependency = 0);

int main(void)
{
	constexpr size_t numOfFiles = 1000;
	constexpr size_t minFileSizeByKB = 1;
	constexpr size_t maxFileSizeByKB = 5120;
	constexpr float lamdaOfExpDistributionForFileSize = 5.0f;
	constexpr float minProcessingTimeByms = 0.5f;
	constexpr float maxProcessingTimeByms = 5.0f;
	constexpr float lamdaOfExpDistributionForProcessingTime = 10.0f;
	constexpr size_t maxNumOfDependency = 0;

	MakeFileListXML(numOfFiles,
					minFileSizeByKB,
					maxFileSizeByKB,
					lamdaOfExpDistributionForFileSize,
					minProcessingTimeByms,
					maxProcessingTimeByms,
					lamdaOfExpDistributionForProcessingTime,
					maxNumOfDependency);

	return 0;
}

void MakeFileListXML(const size_t numOfFiles,
					 const size_t minFileSizeByKB,
					 const size_t maxFileSizeByKB,
					 const float lamdaOfExpDistributionForFileSize,
					 const float minProcessingTimeByms,
					 const float maxProcessingTimeByms,
					 const float lamdaOfExpDistributionForProcessingTime,
					 const size_t maxNumOfDependency)
{
	if (maxFileSizeByKB < minFileSizeByKB)
	{
		std::cout << "error: 'maxFileSizeByKB < minFileSizeByKB'\n";
		return;
	}
	if (maxProcessingTimeByms < minProcessingTimeByms)
	{
		std::cout << "error: 'maxProcessingTimeByms < minProcessingTimeByms'\n";
		return;
	}

	char flag = '\0';
	std::cout << "Make XML file with information data to generation\n";
	std::cout << "Remove existing data before generating information data. [Y/n]: ";
	std::cin >> flag;
	switch (flag)
	{
	case 'Y':
	case 'y':
		break;
	case 'N':
	case 'n':
		return;
	default:
		return;
	}

	XMLDocument doc;

	XMLDeclaration* declare = doc.NewDeclaration();
	doc.InsertEndChild(declare);

	XMLElement* fileList = doc.NewElement("FileList");
	doc.InsertEndChild(fileList);

	std::default_random_engine generator;

	//const float mean = static_cast<float>(maxFileSizeByKB - minFileSizeByKB) / 2.0f;
	//const float sigma = mean / 3.0f;
	//std::normal_distribution<float> fileSize_normalDistribution(mean, sigma);
	
	//std::uniform_real_distribution<float> fileSize_uniformDistribution(static_cast<float>(minFileSizeByKB), static_cast<float>(maxFileSizeByKB));
	std::exponential_distribution<float> fileSize_exponentialDistribution(lamdaOfExpDistributionForFileSize);

	//std::uniform_real_distribution<float> processingTime_uniformDistribution(minProcessingTimeByms, maxProcessingTimeByms);
	std::exponential_distribution<float> processingTime_exponentialDistribution(lamdaOfExpDistributionForProcessingTime);

	std::uniform_int_distribution<size_t> dependencyNum_uniformDistribution(0, maxNumOfDependency);

	for (size_t i = 0; i < numOfFiles; ++i)
	{
		XMLElement* fileInfo = doc.NewElement("FileInfo");
		fileInfo->SetAttribute("idx", i);
		fileList->InsertEndChild(fileInfo);

		XMLElement* fileName = doc.NewElement("FileName");
		fileName->SetText(i);
		fileInfo->InsertEndChild(fileName);

		XMLElement* fileSize = doc.NewElement("FileSize");
		//const float size = fileSize_normalDistribution(generator) + static_cast<float>(minFileSizeByKB);
		//const float size = fileSize_uniformDistribution(generator);
		//const float expDstMax = -log(0.00001f / fileSize_exponentialDistribution.lambda()) / fileSize_exponentialDistribution.lambda();
		//const float sizeCalibration = static_cast<float>(maxFileSizeByKB) / expDstMax;
		const float sizeCalibration = maxFileSizeByKB - minFileSizeByKB;
		float size = fileSize_exponentialDistribution(generator) * sizeCalibration + minFileSizeByKB;
		if (maxFileSizeByKB < size)
		{
			size = static_cast<float>(maxFileSizeByKB);
		}
		fileSize->SetText(size);
		fileInfo->InsertEndChild(fileSize);

		XMLElement* processingTime = doc.NewElement("ProcessingTime");
		//const float time = processingTime_uniformDistribution(generator);
		const float timeCalibration = maxProcessingTimeByms - minProcessingTimeByms;
		float time = processingTime_exponentialDistribution(generator) * timeCalibration + minProcessingTimeByms;
		if (maxProcessingTimeByms < time)
		{
			time = maxProcessingTimeByms;
		}
		processingTime->SetText(time);
		fileInfo->InsertEndChild(processingTime);

		XMLElement* dependencies = doc.NewElement("Dependencies");
		if (i < numOfFiles - 1)
		{
			const size_t numOfDependency = dependencyNum_uniformDistribution(generator);
			std::uniform_int_distribution<size_t> dependency_uniformDistribution(i + 1, numOfFiles - 1);
			std::vector<size_t> dependency_vector;

			for (size_t d = 0; d < numOfDependency; ++d)
			{
				if (i + d == numOfFiles - 1)
				{
					break;	// end of list
				}

				size_t dep = dependency_uniformDistribution(generator);
				while (std::find(dependency_vector.begin(), dependency_vector.end(), dep) != dependency_vector.end())
				{
					dep = dependency_uniformDistribution(generator);	// find not duplicated dependency
				}
				dependency_vector.push_back(dep);

				XMLElement* dependency = doc.NewElement("Dependency");
				dependency->SetText(dep);
				dependencies->InsertEndChild(dependency);

				if ((d == numOfDependency - 1) || (i + d + 1 == numOfFiles - 1))
				{
					break;	// got all dependency OR end of list
				}
			}
		}
		fileInfo->InsertEndChild(dependencies);

		std::cout << "Setting information of file " << i << " has done.\n";
	}

	fs::path outPath = fs::absolute(fs::current_path().parent_path().append("output/filesInfo"));
	if (false == fs::exists(outPath))
	{
		fs::create_directories(outPath);
	}
	fs::path outFilePath(outPath.append("FilesInformation.xml"));
	doc.SaveFile(outFilePath.string().c_str());
}
