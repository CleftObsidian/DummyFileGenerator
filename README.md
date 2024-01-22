# DummyFileGenerator
Generate dummy files with some arguments  
Using exponential distribution for file size and processing time  
It is tested on Windows 10, Visual Studio 19, C++17  

## Make Information File
1. Set the 'makeInfoFile' project as Startup Project.
2. Set the arguments as you want.  
If you set lamda values high, mean of random values decrease.  
Below is an example.
```
constexpr size_t numOfFiles = 1000;
constexpr size_t minFileSizeByKB = 1;
constexpr size_t maxFileSizeByKB = 5120;
constexpr float lamdaOfExpDistributionForFileSize = 5.0f;
constexpr float minProcessingTimeByms = 0.5f;
constexpr float maxProcessingTimeByms = 5.0f;
constexpr float lamdaOfExpDistributionForProcessingTime = 10.0f;
constexpr size_t maxNumOfDependency = 0;
```
3. Build and Run
4. You can find 'FilesInformation.xml' file in '(Solution directory)/output/filesInfo'

## Generate Files
1. Set the 'generateFiles' project as Startup Project.
2. Build and Run
3. You can find generated files in '(Solution directory)/output/generatedFiles'
