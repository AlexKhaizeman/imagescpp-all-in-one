int dumphelp(){
	cout <<"A few words about this program."<< endl;
	cout <<"Possible input arguments are the follows:"<< endl;
	cout<<"One of neccessary arguments:"<<endl;
	cout <<"-i name.bmp — an image to process. Image should be in 'in'-folder. Pure algorythm."<< endl;
	cout <<"-i1 name.bmp — an image to process. Image should be in 'in'-folder. Mirrors a row of pixels all though perimeter. Best modification yet."<< endl;
	cout <<"-i2 name.bmp — an image to process. Image should be in 'in'-folder. Color sensitive for high probability parts."<< endl;
	cout <<"-i3 name.bmp — an image to process. Image should be in 'in'-folder. Color sensitive for high probability parts. Probability sensitive window size. For larger probability we use the larger window size. Two-step algorythm. First step uses a large window."<< endl;
	cout <<"-i4 name.bmp — an image to process. Image should be in 'in'-folder. Color sensitive for high probability parts. Probability sensitive window size. For larger probability we use the larger window size. Two-step algorythm. First step uses a small window. Experimental."<< endl;
	cout <<"-i5 name.bmp — an image to process. Image should be in 'in'-folder. Probability sensitive window size: ^. Two-step algorythm. First step uses a small window. Experimental."<< endl;
	cout <<"-is name.bmp ... name-n.bmp — a bundle of images to process. The results of the first image segmentation will be used to segment the rest images. Images should be in 'in'-folder. "<< endl;
	cout <<"-c name1.bmp name2.bmp — to compare two images. The first image is used like etalon. Images should be in 'in'-folder."<< endl;
	cout <<"Optional arguments should be added after the method arguments:"<< endl;
	cout <<"-bi* — a number of binary image to process (from 1 to 8). If none all 8 are done."<< endl;
	cout <<"-cl* — 0 or 1. If 0 or ommited the image will be prosecces as grey-scale image. If 1 the color-type will be deteced automatically."<< endl;
	cout <<"-h* — 0.1 .. 0.99. How hard is it to get a threshhold."<< endl;
	cout <<"-p* — 10-2000. Number of points on histogramm. 500 is ok."<< endl;
	cout <<"-f* — 7-31. Frame size. 21 on default."<< endl;
	// cout<<""<<endl;
	return 0;
}

int dumperr(){
	cout<<"The programm reqiures correct arguments. Run with -h to get help."<<endl;
	return 0;
}

void dump(string variable){
	cout << variable << endl;
}

void dump(int variable){
	cout << to_string(variable) << endl;
}

void dump(double variable){
	cout << to_string(variable) << endl;
}