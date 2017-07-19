#include <ImageMagick-6/Magick++.h>
#include <iostream>
using namespace std;
using namespace Magick;
#include <fstream>
#include <iomanip>
#include <list>     // подключаем заголовок списка
#include <iterator> // заголовок итераторов
#include <ctime>
#include <sys/stat.h>
#include <tiffio.h>
#include <cmath>

#include "dumphelp.h"
#include "--channelSegmentation.h"
#include "--channelSegmentation2.h"
#include "--channelSegmentation3.h"
#include "--channelSegmentation4.h"
#include "--channelSegmentation5.h"
#include "--channelSegmentations.h"
#include "-image.h"
#include "-image1.h"
#include "-images.h"
#include "-compare.h"
#include "-compare1.h"

// g++ image.cpp -o image `Magick++-config --cppflags --cxxflags --ldflags --libs` && ./image -h

int main(int argc, char const *argv[])
{	
	//time of start
	std::time_t t_start = std::time(0);
	string path = "out/" + to_string(t_start);
	//an array of arguments
	string *input_arguments;
	input_arguments = new string[argc-1];
	//dump error if nothing is input
	if (argc == 1){
		return dumperr();
	}
	//parse input
	for (int i = 0; i < argc-1; ++i){	
		input_arguments[i] = "";
		input_arguments[i] += argv[i+1];
		if (input_arguments[i] == "-h" || input_arguments[i] == "-help"){
			return dumphelp();;
		}
	}
	//work with input
	for (int i = 0; i < argc-1; ++i){
		if ((input_arguments[i]=="-i") || (input_arguments[i]=="-i1") || (input_arguments[i]=="-i2") || (input_arguments[i]=="-i3") || (input_arguments[i]=="-i4")|| (input_arguments[i]=="-i5") || (input_arguments[i]=="-c") || (input_arguments[i]=="-is")){
			dump("Started.");
			// const char *path_ = path.c_str();
			// mkdir(path_, 0777);
			ofstream f; //описывает поток для записи данных в файл	
			// f.open(path + "/output.txt", ios::app); //открываем файл в режиме записи, режим ios::out устанавливается по умолчанию
			InitializeMagick(*argv);
			double hardness = 0.7;
			int points_number = 500;
			int fS = 21;
			for (int ii = i; ii < argc-1; ++ii){
				if (input_arguments[ii]=="-h*"){
					try{
						hardness = stod(input_arguments[ii+1]);
						if (hardness > 0.99){
							hardness = 0.99;
						}
						if (hardness < 0.1){
							hardness = 0.1;
						}
					}
					catch( Exception &error_ ){
						cout << "Caught exception: " << error_.what() << endl;
					}
				}			
				if (input_arguments[ii]=="-p*"){
					// points_number = std::stoi(input_arguments[i+1]);
					points_number = stoi(input_arguments[ii+1]);
					if (points_number > 2000){
						points_number = 2000;
					}
					if (points_number < 10){
						points_number = 10;
					}					
				}
			}
			//-i mode
			if (input_arguments[i]=="-i"){
				//preparation
				path += " -i ";
				path += input_arguments[i+1];
				int bi=8;
				bool colorfull = false;
				for (int ii = i; ii < argc-1; ++ii){
					if (input_arguments[ii]=="-bi*"){
						bi = stoi(input_arguments[ii+1]);
						if (bi>8 || bi<0){
							bi = 8;
							dump("Invalid binary image argument. All binary images will be proccessed.");
						}
						else{
							path += " bi " + to_string(bi);
						}
					}					
					if (input_arguments[ii]=="-cl*"){
						if (input_arguments[ii+1] == "1"){
							colorfull = true;
							path += " grey";
						}
					}
				}
				const char *path_ = path.c_str();
				mkdir(path_, 0777);
				f.open(path + "/output.txt", ios::app);
				f<<"Selected mode is one image mode."<<endl;
				//RUN IMAGE
				image(path, input_arguments[i+1], bi, colorfull, hardness, points_number, 1);
			}
			if (input_arguments[i]=="-i1"){
				//preparation
				path += " -i ";
				path += input_arguments[i+1];
				int bi=8;
				bool colorfull = false;
				for (int ii = i; ii < argc-1; ++ii){
					if (input_arguments[ii]=="-f*"){
						try{
							fS = stoi(input_arguments[ii+1]);
							if (fS > 31){
								fS = 31;
							}
							if (fS < 7){
								fS = 7;
							}
						}
						catch( Exception &error_ ){
							cout << "Caught exception: " << error_.what() << endl;
						}
					}						
					if (input_arguments[ii]=="-bi*"){
						bi = stoi(input_arguments[ii+1]);
						if (bi>8 || bi<0){
							bi = 8;
							dump("Invalid binary image argument. All binary images will be proccessed.");
						}
						else{
							path += " bi " + to_string(bi);
						}
					}					
					if (input_arguments[ii]=="-cl*"){
						if (input_arguments[ii+1] == "1"){
							colorfull = true;
							path += " grey";
						}
					}
				}
				const char *path_ = path.c_str();
				mkdir(path_, 0777);
				f.open(path + "/output.txt", ios::app);
				f<<"Selected mode is one modified image mode. Row of pixels wil be mirrowed."<<endl;
				//RUN IMAGE
				image1(path, input_arguments[i+1], bi, colorfull, hardness, points_number, 1, fS);
			}
			if (input_arguments[i]=="-i2"){
				//preparation
				path += " -i2 ";
				path += input_arguments[i+1];
				int bi=8;
				bool colorfull = false;
				for (int ii = i; ii < argc-1; ++ii){
					if (input_arguments[ii]=="-bi*"){
						bi = stoi(input_arguments[ii+1]);
						if (bi>8 || bi<0){
							bi = 8;
							dump("Invalid binary image argument. All binary images will be proccessed.");
						}
						else{
							path += " bi " + to_string(bi);
						}
					}					
					if (input_arguments[ii]=="-cl*"){
						if (input_arguments[ii+1] == "1"){
							colorfull = true;
							path += " grey";
						}
					}
				}
				const char *path_ = path.c_str();
				mkdir(path_, 0777);
				f.open(path + "/output.txt", ios::app);
				f<<"Selected mode is one image mode."<<endl;
				//RUN IMAGE
				image(path, input_arguments[i+1], bi, colorfull, hardness, points_number, 2);
			}
			if (input_arguments[i]=="-i3"){
				//preparation
				path += " -i3 ";
				path += input_arguments[i+1];
				int bi=8;
				bool colorfull = false;
				for (int ii = i; ii < argc-1; ++ii){
					if (input_arguments[ii]=="-bi*"){
						bi = stoi(input_arguments[ii+1]);
						if (bi>8 || bi<0){
							bi = 8;
							dump("Invalid binary image argument. All binary images will be proccessed.");
						}
						else{
							path += " bi " + to_string(bi);
						}
					}					
					if (input_arguments[ii]=="-cl*"){
						if (input_arguments[ii+1] == "1"){
							colorfull = true;
							path += " grey";
						}
					}
				}
				const char *path_ = path.c_str();
				mkdir(path_, 0777);
				f.open(path + "/output.txt", ios::app);
				f<<"Selected mode is one image mode."<<endl;
				//RUN IMAGE
				image(path, input_arguments[i+1], bi, colorfull, hardness, points_number, 3);
			}
			if (input_arguments[i]=="-i4"){
				//preparation
				path += " -i4 ";
				path += input_arguments[i+1];
				int bi=8;
				bool colorfull = false;
				for (int ii = i; ii < argc-1; ++ii){
					if (input_arguments[ii]=="-bi*"){
						bi = stoi(input_arguments[ii+1]);
						if (bi>8 || bi<0){
							bi = 8;
							dump("Invalid binary image argument. All binary images will be proccessed.");
						}
						else{
							path += " bi " + to_string(bi);
						}
					}					
					if (input_arguments[ii]=="-cl*"){
						if (input_arguments[ii+1] == "1"){
							colorfull = true;
							path += " grey";
						}
					}
				}
				const char *path_ = path.c_str();
				mkdir(path_, 0777);
				f.open(path + "/output.txt", ios::app);
				f<<"Selected mode is one image mode."<<endl;
				//RUN IMAGE
				image(path, input_arguments[i+1], bi, colorfull, hardness, points_number, 4);
			}	
			if (input_arguments[i]=="-i5"){
				//preparation
				path += " -i ";
				path += input_arguments[i+1];
				int bi=8;
				bool colorfull = false;
				for (int ii = i; ii < argc-1; ++ii){
					if (input_arguments[ii]=="-bi*"){
						bi = stoi(input_arguments[ii+1]);
						if (bi>8 || bi<0){
							bi = 8;
							dump("Invalid binary image argument. All binary images will be proccessed.");
						}
						else{
							path += " bi " + to_string(bi);
						}
					}					
					if (input_arguments[ii]=="-cl*"){
						if (input_arguments[ii+1] == "1"){
							colorfull = true;
							path += " colorcheck";
						}
					}
				}
				const char *path_ = path.c_str();
				mkdir(path_, 0777);
				f.open(path + "/output.txt", ios::app);
				f<<"Selected mode is one modified image mode. Row of pixels wil be mirrowed."<<endl;
				//RUN IMAGE
				image1(path, input_arguments[i+1], bi, colorfull, hardness, points_number, 5, fS);
			}			
			//compare mode
			if (input_arguments[i]=="-c"){
				//preparation
				path += " -c ";
				// path += " -c " + input_arguments[i+1] + " vs " + input_arguments[i+2];
				const char *path_ = path.c_str();
				mkdir(path_, 0777);
				f.open(path + "/output.txt", ios::app);
				f<<"Selected mode is an image compare mode."<<endl;
				//RUN COMPARE
				CompareImages(path, input_arguments[i+1], input_arguments[i+2]);
			}
			//compare mode
			if (input_arguments[i]=="-c1"){
				//preparation
				path += " -c1 ";
				// path += " -c " + input_arguments[i+1] + " vs " + input_arguments[i+2];
				const char *path_ = path.c_str();
				mkdir(path_, 0777);
				f.open(path + "/output.txt", ios::app);
				f<<"Selected mode is an image compare mode."<<endl;
				//RUN COMPARE
				CompareImages1(path, input_arguments[i+1], input_arguments[i+2]);
			}
			if (input_arguments[i]=="-is"){
/*				//preparation
				path += " -i ";
				path += input_arguments[i+1];
				int bi=8;
				bool colorfull = false;
				for (int ii = i; ii < argc-1; ++ii){
					if (input_arguments[ii]=="-bi*"){
						bi = stoi(input_arguments[ii+1]);
						if (bi>8 || bi<0){
							bi = 8;
							dump("Invalid binary image argument. All binary images will be proccessed.");
						}
						else{
							path += " bi " + to_string(bi);
						}
					}					
					if (input_arguments[ii]=="-cl*"){
						if (input_arguments[ii+1] == "1"){
							colorfull = true;
							path += " grey";
						}
					}
				}
				const char *path_ = path.c_str();
				mkdir(path_, 0777);
				f.open(path + "/output.txt", ios::app);
				f<<"Selected mode is one image mode."<<endl;
				//RUN IMAGE
				images(path, input_arguments[i+1], bi, colorfull);	*/			
			}
			if (input_arguments[i]==""){/*code*/}
			if (input_arguments[i]==""){/*code*/}
			std::time_t t_finish = std::time(0);
			f<<"Прошло " + to_string(t_finish - t_start) + " секунд."<<endl;
			dump(to_string(t_finish - t_start) + " seconds past.");
			f.close();
			dump("Ended.");
			return 0;
		}
		else{
			return dumperr();
		}
	}	
	return 0;
}