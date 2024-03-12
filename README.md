# Enhancer for scanned images
Welcome to the project repo of "Lisa Kim Will" and "Onur Yilmazer" for the Algorithm Engineering course.

Release date: `12.03.2024`

Goal: Executable program to improve readability of scanned documents

## Build instructions:
- Clone the git directory into a suitable folder (e.g. your desktop) and navigate into the "project" folder
  
  `cd ~/Desktop`
  
  `git clone git@github.com:onuryilmazer/algorithmengineering-project.git`

  `cd ./algorithmengineering-project/project`

- Create the build files using cmake

  `cmake -S ./ -B buildfolder`

 - Instruct cmake to build the project

   `cmake --build buildfolder --config Release`

That's it. You will find the executables under ./buildfolder/Release, along with some sample input to get you started.

## Running unit tests:
To make sure everything was compiled correctly, you can run the unit tests that come with the program.

To do that, you will need to open the folder that contains your executables in your terminal and run the tests executable using `./enhancer_tests.exe`. 

The tests use the example images that were copied into the "test_input" folder during build, so make sure you haven't deleted them!

## Usage:
There are two modes of operation that you can use our program with. 

### Interactive mode
- In this mode the program will guide you to provide all the inputs it needs to get started.
- This mode scans the folders in your working directory (i.e. what you get when you call `pwd`) and lets you pick the input folder that way.
- If you add the executable `enhancer.exe` to your system's `PATH` variable, you can call it from anywhere in your system and start working.
- Call `./enhancer.exe --interactive` and follow the instructions.

  ![image](https://github.com/onuryilmazer/algorithmengineering-project/assets/29818337/8be8679f-62cd-41d8-a08d-25c8c1d6a786)


### Manual mode
- In this mode you are expected to provide the required commandline arguments and potentially some optional ones yourself.
- You can use this mode directly when you're working from your terminal, or to call the enhancer from witin other scripts for automated usage.

- Syntax:

  `./enhancer.exe <arguments>`


- Required arguments:
  
  `--inputPath <val>`: Path to the folder that contains your source images. This can be a relative path to your current working directory (e.g. input_images) or an absolute path to anywhere in your system (e.g. `C:/temp` on Windows or `/tmp` on Unix systems)
  
  `--outputDirectory <val>`: This is the name of the folder that will be automatically created inside your source folder to store your enhanced images.
  
  `--windowWidth <val>`: [Optional] This is a value between 0 and 1 that the adaptive thresholding algorithm uses when enhancing your images. It is defined in terms of the width of your images. The algorithm examines all the pixels within a rectangle of this width when deciding whether to make a pixel dark or bright. Default value for this argument is 0.125, as recommended by the authors of the method.
  
  `--tresholdPercentage <val>`: [Optional] This is a value between 0 and 1 that the adaptive thresholding algorithm uses when enhancing your images. It is defined as a percentage. The algorithm makes a pixel bright if its brightness value is this much above the average of all pixels within the given windowWidth.
  
  `--numberOfThreads_adaptiveThresholding <val>`: [Optional] This argument allows you to manually set the number of threads that are used to process individual image files. Its default value is the number of logical cores your system has.

  `--numberOfThreads_grayscaleConversion <val>`: [Optional] This argument allows you to manually set the number of threads that are used while converting a single image file to grayscale format. Its default value is 1. It is recommended to leave it as 1, because issues like "false friends" prevent processing small files simultaneously in different threads and it may actually impact performance negatively.

  `--verbose <true/false>`: [Optional] This argument allows you to surpress the informative lines the program outputs while processing images. Its default value is `true`.

  `--benchmark`: [Optional] This argument starts the program in the benchmark mode, where it runs three different benchmarks using the files in your inputPath and saving the results to your outputDirectory. It will output the csv files containing the benchmark results into your current working directory, and you can examine/plot these using scripting languages like R and Python.

  `--help`: [Optional] Prints a help page that gives information about the commandline arguments you can use.

  ![image](https://github.com/onuryilmazer/algorithmengineering-project/assets/29818337/5508b3d8-8fa0-448e-8f51-6d62ec58d217)


## Used libraries
- STB - single-file public domain libraries for C/C++: Used for reading and writing files from the disk. Source: [STB repo](https://github.com/nothings/stb)
- Catch2 - A modern, C++-native, test framework for unit-tests: Used for the unit tests. Source: [Catch2 repo](https://github.com/catchorg/Catch2)
- Termcolor - A header-only C++ library for printing colored messages to the terminal. Used for colorizing the error/success messages (it is a cross-platform library). Source: [Termcolor](https://github.com/ikalnytskyi/termcolor)


## The results:
- Improving bad lighting conditions:
![beforeafter2](https://github.com/onuryilmazer/algorithmengineering-project/assets/29818337/bf741fc3-b358-4d3f-9279-14c457b32603)

- Improving overall readability:
![beforeafter1](https://github.com/onuryilmazer/algorithmengineering-project/assets/29818337/7fa60250-d0a6-4294-b832-8c7c0789e4ba)



