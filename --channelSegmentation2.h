// path - путь, куда писать;
// info - просто строка, сообщающая канал. бывает: R G B grey
// channel_R - массив - цветовой канал; 
// ширина; 
// высота; 
// номер разрядного изображения; 
// число точек на гистограмме; 
// ширина окна; 
// высота окна
// жёсткоть выбора порогов
unsigned short **channelSegmentation2(string path, string info, unsigned short **channel_R, int width, int height, int bi, int points_number, int frameWidth, int frameHeight, double hardness){
	ofstream f;
	f.open(path + "/output.txt", ios::app);
	f<<"Сегметация канала: " + info<<endl;
	dump("Сегметация канала: " + info);
	f<<to_string(bi)+" бинарное изображение"<<endl;
	dump(to_string(bi)+" binary image");
	// InitializeMagick(*argv);
	Image dst_image(Geometry(width, height), ColorRGB(MaxRGB, MaxRGB, MaxRGB));
	Image temp_image(Geometry(width, height), ColorRGB(MaxRGB, MaxRGB, MaxRGB)); //current BI
	//
	dst_image.modifyImage();
	Pixels dst_view(dst_image);
	//
	temp_image.modifyImage();
	Pixels temp_view(temp_image);
	//0<bi<7 - number of a binary image
	int 
		comporator = 1<<bi, //нужен здесь. Поясняет, какой бит брать.
		frameSquare = frameHeight * frameWidth,
		//points
		current_points_number = 0,
		frameHeightHalf = (frameHeight-1) / 2,
		frameWidthHalf = (frameWidth-1) / 2;
	double 
		accuracy = 1 / ((double) points_number),
		max_point = 0,
		element = 0,
		sequences = 0;
	//points
	double *points = new double[points_number];
	int *vicinities = new int[points_number];
	int *point_index = new int[points_number]; //номера точек для сортировки окрестностей
	bool *minims = new bool[points_number];
	short 
		minims_counter = 0, 
		minims_flag = 0; //это флаг, на каком минимуме мы сейчас находимся, когда идём по гистограмме (чтобы в соответствующий цвет красить)
	//pi
	double **pi = new double*[height]; // (!) pi - для центра окна - среднее по окну (типа симметричные пи1-пи2)
	double **pi_average = new double*[height]; // среднее PI
	double **PI = new double*[height]; //PI = pi1*pi2/pi3
	for(int i = 0; i < height; i++) {
		pi[i] = new double[width];
		PI[i] = new double[width];
		pi_average[i] = new double[width]; //PI average
	}		
	//чтобы рисовать гистограммы
	int histHeight = 600, histWidth = 1800;
	Image hist_image(Geometry(histWidth, histHeight), Color(MaxRGB, MaxRGB, MaxRGB, 0));
	histHeight -= 30;
	histWidth -= 5;
	hist_image.strokeColor("olive"); // Outline color 
	hist_image.fillColor("green"); // Fill color 
	hist_image.strokeWidth(1);
	// Construct drawing list
	std::list<Magick::Drawable> drawList; 
	//axes
	histHeight-=25;
	drawList.push_back(DrawableLine(5, histHeight, histWidth, histHeight)); //X ось
	drawList.push_back(DrawableLine(5, 0, 5, histHeight)); // Y ось
	drawList.push_back(DrawableLine((histWidth+5)/2, 0, histWidth/2+5, histHeight)); // Y ось 0.5
	histHeight+=5;
	//Draw scale 0.1
	histWidth -= 5;
	for (int j = 1; j < 10; j++){
		drawList.push_back(DrawableLine(
			(histWidth * j / 10) + 5,
			histHeight,
			(histWidth * j / 10) + 5,
			histHeight - 10));
	}
	histWidth += 5;
	histHeight+=20;
	//draw and clear
	hist_image.draw(drawList);
	drawList.clear();
	//model
	unsigned short **channel_R_ = new unsigned short*[height]; //Declare a COPY model of an image for current BI
	unsigned short **channel_R_S = new unsigned short*[height]; // Declare an output array
	for(int i = 0; i < height; i++) {
		channel_R_[i] = new unsigned short[width];
		channel_R_S[i] = new unsigned short[width];
	}	
	//обнуление
	for ( ssize_t row = 0; row < height ; row++ ){
		for ( ssize_t column = 0; column < width ; column++ ){
			PI[row][column] = 0.0;
			pi[row][column] = 0.0;					
			pi_average[row][column] = 0.0;					
			channel_R_[row][column] = channel_R[row][column]&comporator; //! вычленение бита сразу
			channel_R_S[row][column] = 0;
		}
	}	
	//
	unsigned short  *biElementsFrame = new unsigned short [frameSquare];
	//
	int 
		index,
		frameWidthHalfLocal = 1,
		frameHeightHalfLocal = 1,
		frameSquareLocal = 1;
	// вероятность pi1 pi2 - работает не чини
	for ( ssize_t row = 0; row < height ; row++ ){
		for ( ssize_t column = 0; column < width ; column++ ){
			if (column > frameWidthHalf && row > frameHeightHalf && column < (width - frameWidthHalf) && row < (height - frameHeightHalf)){
				//make a 1d copy
				index = 0;
				//frame circle
				for (ssize_t frameRow = (row - frameHeightHalf); frameRow <= (row + frameHeightHalf); frameRow++){
					for (ssize_t frameColumn = (column - frameWidthHalf); frameColumn <= (column + frameWidthHalf); frameColumn++){                    	
						biElementsFrame[index] = channel_R_[frameRow][frameColumn];
						index++;
					}
				}								
				//good
				element = 1;
				sequences = 1;
				pi[row][column] = frameSquare;
				for (int j = 1; j < frameSquare; j++){
					if (biElementsFrame[j - 1] == biElementsFrame[j])
						{ element++; }
					else
						{ sequences++; }
					element++;
					pi[row][column] -= (double) sequences / (double) element;
				}                
				pi[row][column] = (double) pi[row][column]/ (double) frameSquare;      
			}
			else{
				if (column <= frameWidthHalf){
					frameWidthHalfLocal = column;
				}
				if (column >= (width - frameWidthHalf)){
					frameWidthHalfLocal = width - column - 1;
				}
				if (row <= frameHeightHalf){
					frameHeightHalfLocal = row;
				}
				if (row >= (height - frameHeightHalf)){
					frameHeightHalfLocal = height - row - 1;
				}				
				frameSquareLocal = (2 * frameHeightHalfLocal + 1) * (2 * frameWidthHalfLocal + 1);
				//make a 1d copy
				index = 0; 
				//frame circle
				for (ssize_t frameRow = (row - frameHeightHalfLocal); frameRow <= (row + frameHeightHalfLocal); frameRow++){
					for (ssize_t frameColumn = (column - frameWidthHalfLocal); frameColumn <= (column + frameWidthHalfLocal); frameColumn++){                    	
						biElementsFrame[index] = channel_R[frameRow][frameColumn];
						index++;
					}
				}
				//good
				element = 1;
				sequences = 1;
				pi[row][column] = frameSquareLocal;
				for (int j = 1; j < frameSquareLocal; j++){
					if (biElementsFrame[j - 1] == biElementsFrame[j])
						{ element++; }
					else
						{ sequences++; }
					element++;
					pi[row][column] -= (double) sequences / (double) element;
				}                
				pi[row][column] = (double) pi[row][column]/ (double) frameSquareLocal;                  	
			}
		}
	}
	// dump("pi1 and pi2 counted...");
	for ( ssize_t row = 0; row < height ; row++ ){
		for ( ssize_t column = 0; column < width ; column++ ){								
			if (pi[row][column] < 0){
				f<<"warning: pi<0: " + to_string(pi[row][column])+" row: " + to_string(row) + " column: " + to_string(column)<<endl;
				f<<" " + to_string(element) + " / " + to_string(sequences) + " / " + to_string(frameSquare) + " = " + to_string(element / sequences / frameSquare)<<endl;
				pi[row][column] = 0.001;
			}
			if (pi[row][column] > 1.0){
				f<<"warning: pi>1: " + to_string(pi[row][column])+" row: " + to_string(row) + " column: " + to_string(column)<<endl;
				f<<" " + to_string(element) + " / " + to_string(sequences) + " / " + to_string(frameSquare) + " = " + to_string(element / sequences / frameSquare)<<endl;
				pi[row][column] = 0.999;
			}						
		}
	}		
	// dump("pi1 and pi2 checked ...");
	double alpha, aalpha, pi3;
	//01 10 - pi=0.5; 11 00 - pi = ?
	for ( ssize_t row = 1; row < height ; row++ ){
		for ( ssize_t column = 1; column < width ; column++ ){
			alpha = pi[row][column];
			aalpha = 1 - alpha;
			//0x1 1x0
			if (channel_R_[row-1][column] != channel_R_[row][column-1]) {
				PI[row][column] = 0.5;
			}
			//0x0 1x1
			else{
				pi3 = (double) (alpha * alpha + aalpha * aalpha);
				//000 111
				if (channel_R_[row][column] == channel_R_[row][column-1]){
					PI[row][column] = (double) (alpha * alpha / pi3); 
				}
				//010 101
				else{					
					PI[row][column] = (double) (aalpha * aalpha / pi3); 
				}
			}
		}
	}		
	// dump("PI 3 counted ...");
	//pi frame avarage
	for ( ssize_t row = 0; row < height ; row++ )//вероятность pi1 pi2
	{
		for ( ssize_t column = 0; column < width ; column++ )
		{
			pi_average[row][column] = 0;
			if (column > frameWidthHalf && row > frameHeightHalf && column < (width - frameWidthHalf) && row < (height - frameHeightHalf)){
				for ( ssize_t frameRow = row - frameHeightHalf; frameRow <= row + frameHeightHalf; frameRow++ ){
					for ( ssize_t frameColumn = column - frameWidthHalf; frameColumn <= column + frameWidthHalf; frameColumn++ ){
						pi_average[row][column] += (double) PI[frameRow][frameColumn];
					}
				}
				pi_average[row][column] = (double) pi_average[row][column] / (double) frameSquare;				                        	
			}
			else{
				if (column <= frameWidthHalf){
					frameWidthHalfLocal = column;
				}
				if (column >= (width - frameWidthHalf)){
					frameWidthHalfLocal = width - column - 1;
				}
				if (row <= frameHeightHalf){
					frameHeightHalfLocal = row;
				}
				if (row >= (height - frameHeightHalf)){
					frameHeightHalfLocal = height - row - 1;
				}				
				frameSquareLocal = (2 * frameHeightHalfLocal + 1) * (2 * frameWidthHalfLocal + 1);
				//frame circle
				for ( ssize_t frameRow = row - frameHeightHalfLocal; frameRow <= row + frameHeightHalfLocal; frameRow++ ){
					for ( ssize_t frameColumn = column - frameWidthHalfLocal; frameColumn <= column + frameWidthHalfLocal; frameColumn++ ){
						pi_average[row][column] += (double) PI[frameRow][frameColumn];
					}
				}
				pi_average[row][column] = (double) pi_average[row][column] / (double) frameSquareLocal;				                        	
			}
		}
	}	
	// dump("Pi3-s frame average counted ...");
	for (int j = 0; j < points_number; j++)
		points[j] = 0;
	// dump("created points ...");
	//если неправильно вычислить пи - будет сегфолт, что логично. смотри выше на глюки. - всё поправлено. 20161023. Ну, давно нормально работает.
	for ( ssize_t row = 0; row < height ; row++ ){
		for ( ssize_t column = 0; column < width ; column++ ){
			pi_average[row][column] = Round(pi_average[row][column], accuracy);
			if (pi_average[row][column] <= 0){
				pi_average[row][column] = 0.001;
			}
			if (pi_average[row][column] >= 1.0){
				pi_average[row][column] = 0.999;
			}				
		}
	}
	// dump("pi rounded ...");
	//нахождение числа одинаковых вероятностей (просто как модуль от деления на шаг)
	for ( ssize_t row = 0; row < height ; row++ ){
		for ( ssize_t column = 0; column < width ; column++ ){
			current_points_number = (int)(pi_average[row][column] / accuracy);
			if (current_points_number >= points_number){
				f<<"warning more: in " + to_string(current_points_number)+" pi is: " + to_string(pi_average[row][column])<<endl;
				current_points_number = points_number - 1;
			}
			if (current_points_number<0){
				f<<"warning zero: in " + to_string(current_points_number)+" pi is: " + to_string(pi_average[row][column])<<endl;				
				current_points_number = 0;
			}
			points[current_points_number]++;					
		}
	}
	// dump("counted histogramm points ...");
	//нахождение наивысшей точки (максимального количества), нормировка по наивысшей точке    
	max_point = 0;
	for (int j = 0; j < points_number; j++)
		if (max_point < points[j])
			max_point = points[j];
	//прорисовка на гистограмме
	hist_image.strokeColor("black"); // Outline color 		   
	histHeight -= 25;
	histWidth -= 5;
	for (int j = 0; j < points_number; j++){
		points[j] /= max_point;
		drawList.push_back(DrawableLine(
			(histWidth * j / points_number) + 5,
			(int)(histHeight - histHeight * points[j]),
			(histWidth * j / points_number) + 5,
			(int)histHeight));
	}
	for (int j = 1; j <= points_number; j++){
		drawList.push_back(DrawableLine(
			(histWidth * (j-1) / points_number) + 5,
			(int)(histHeight - histHeight * points[j-1]),
			(histWidth * j / points_number) + 5,
			(int)(histHeight - histHeight * points[j])));				
	}
	histHeight += 25;
	//add some text // drawList.push_back(DrawableFont("-misc-fixed-medium-o-semicondensed—13-*-*-*-c-60-iso8859-1"));?
	DrawableFont font = DrawableFont("Times New Roman",
									  NormalStyle,
									  400,
									  SemiCondensedStretch
									 );
	
	drawList.push_back(font);	
	drawList.push_back(DrawableStrokeColor(Color("black")));
	drawList.push_back(DrawableFillColor(Color(0, 0, 0, MaxRGB)));	
	drawList.push_back(DrawableText(5,histHeight,"0"));
	drawList.push_back(DrawableText(histWidth,histHeight,"1"));	
	drawList.push_back(DrawableText(histWidth/2,histHeight,"0.5"));
	// drawList.push_back(DrawableText(3*histWidth/4,histHeight+10,"вероятность перехода"));
	drawList.push_back(DrawableText(10,15,""+to_string((int) max_point)+" px"));
	// drawList.push_back(DrawableText(histWidth/2+10,histHeight/2,"количество "));
	// drawList.push_back(DrawableText(histWidth/2+10,histHeight/2 + 15," пикселей"));
	histHeight -= 10;
	// histHeight += 5;
	// histWidth += 5;
	hist_image.draw(drawList);
	drawList.clear();
	// minimumy)
	points_number--;
	minims_counter=1;
	// int vicinity = (int) points_number/20; //for example
	// dump("drew clean histogramm. started to find minimums ...");
	//2
	for (int j = 0; j < points_number; j++){
		point_index[j] = j;
		vicinities[j] = 0;
		minims[j] = false;
	}
	minims[0] = true; // чтобы цветов было на один больше чем минимумов, введём минимум в нуле, чтобы привязать к нему первый цвет
	//вычисляем окрестности, в которых данная точка является минимумом
	int vicinity = 1;
	bool a_vicinity_flag = true;
	for (int j = points_number/2; j < points_number - 1; j++){
		vicinity = 1;
		while(a_vicinity_flag){
			if (!((points[j]<=points[j-vicinity] & points[j]<points[j+vicinity])||(points[j]<points[j-vicinity] & points[j]<=points[j+vicinity]))){
				a_vicinity_flag = false;
			}
			else{
				vicinity++;
			}
		}
		vicinities[j] = vicinity-1;
		a_vicinity_flag = true;	// f<<to_string(vicinities[j]) + " "<<endl;
	}
	// dump("got vicinities ...");
	int start = points_number/2; //начинаем от вероятности = 0.5
	//попробуем просто отсортировать
	for (std::size_t idx_i = 0; idx_i < points_number - 1; idx_i++)
	{
		for (std::size_t idx_j = 0; idx_j < points_number - idx_i - 1; idx_j++)
		{
			if (vicinities[idx_j + 1] > vicinities[idx_j])
			{
				std::swap(vicinities[idx_j], vicinities[idx_j + 1]);
				std::swap(point_index[idx_j], point_index[idx_j + 1]);                
			}
		}
	}
	// dump("sorted vicinities ...");
	drawList.push_back(DrawableStrokeColor(Color("red")));
	minims_counter = 0;
	double margin = pow(vicinities[0], hardness); //допустим так пока
	for (int ii=0; ii<20; ii++){
		if ((double) vicinities[ii] > margin)
			minims_counter++;
	}
	for (int ii=0; ii<minims_counter; ii++){
		minims[point_index[ii]] = true;
		f<<"Есть минимум в точке " + to_string(point_index[ii]) + " равный " + to_string(point_index[ii] * accuracy) + " окрестность " + to_string(vicinities[ii])<<endl;
		drawList.push_back(DrawableText(point_index[ii] * accuracy * histWidth, histHeight, to_string(point_index[ii] * accuracy)));
		drawList.push_back(DrawableLine(
			(histWidth * point_index[ii] * accuracy) + 5,
			5,
			(histWidth * point_index[ii] * accuracy) + 5,
			(int)histHeight-10));    	
	}
	hist_image.draw(drawList);
	drawList.clear();
	//end 2
	//normal colours: step*255/counter
	minims_counter++; //нам нужен ещё один цвет для самого крайнего сегмента. там будет зависимость от яркости
	unsigned short  *Colours = new unsigned short [minims_counter];
	int colorstep = 255 / minims_counter;
	for (int ii = 0; ii <= minims_counter; ii++){
		Colours[ii] = (unsigned short) (colorstep*ii);
	}
	for (int i = 0; i <= minims_counter; ++i){
		f<<"Цвет номер " + to_string(i) + " яркости " + to_string(Colours[i])<<endl;
	}
	minims_counter++; //ибо там нужно на цвет больше, чем есть минимумов
	//segmentation - @TODO: откомментировать потом
	//надо найти последний минимум. как?
	int last_min_number=0;
	for ( ssize_t row = 0; row < height ; row++ ){
		for ( ssize_t column = 0; column < width ; column++ ){
			channel_R_S[row][column] = 0;
			minims_flag = 0;
			for (int j = 0; j <= points_number; j++){
				if (minims[j]){
					last_min_number = j;
					minims_flag++; // f<<to_string(minims_flag)<<endl;					
					if (pi_average[row][column] > j*accuracy){								
						for (int jj = j+1; jj <= points_number; jj++){
							if (minims[jj]){
								// dump("point min is in " + to_string(j) + " equal " + to_string(j*accuracy) + " and in " + to_string(jj) + " equal " + to_string(jj*accuracy) + " pi is " + to_string(pi_average[row][column]));
								if (pi_average[row][column] <= jj*accuracy){									
									channel_R_S[row][column] = Colours[minims_flag];
									if(channel_R_S[row][column]>255){
										f<<"	here :" + to_string(column) + " " + to_string(row) + " is an error_ : " + to_string(channel_R_S[row][column]) + " min flag is " + to_string(minims_flag)<<endl;
										channel_R_S[row][column]=255;				
									}								
								}
								// else - тут нет, потому что крайний тупо остаётся чёрным. вона чё.
							}
						}
					}
				}
			}
		}
	}
	//
	// dump(last_min_number);
	// dump(minims_counter);

	//покрасим последний
	for ( ssize_t row = 0; row < height ; row++ ){
		for ( ssize_t column = 0; column < width ; column++ ){
			if (pi_average[row][column] > last_min_number*accuracy){
				// f<<"	here :" + to_string(column) + " " + to_string(row) + " might be the last color : " + to_string(channel_R_[row][column])<<endl;
				if (channel_R_[row][column] == 0) {
					channel_R_S[row][column] = Colours[minims_counter-1];
					// f<<"	here :" + to_string(column) + " " + to_string(row) + " is the last color : " + to_string(channel_R_S[row][column])<<endl;
				}
				else{
					channel_R_S[row][column] = Colours[minims_counter];
				}
			}
		}
	}	
	points_number++;
	dump(to_string(bi) + " binary image is successfully segmented.");
	Color pixel_color = *(dst_view.get(1,1,1,1));
	Color pixel_color_t = *(temp_view.get(1,1,1,1));
	//рисуем итоговую карту "слоя"
	for ( ssize_t row = 0; row < height ; row++ ){
		for ( ssize_t column = 0; column < width ; column++ ){
			pixel_color = ColorRGB((double) channel_R_S[row][column]/255, (double) channel_R_S[row][column]/255, (double) channel_R_S[row][column]/255);
			*(dst_view.get(column,row,1,1)) = pixel_color;

			if (channel_R_[row][column] != 0)
				pixel_color_t = ColorRGB((double) 1,1,1);
			else
				pixel_color_t = ColorRGB((double) 0,0,0);
			*(temp_view.get(column,row,1,1)) = pixel_color_t;
		}
	}	
	dst_view.sync();
	temp_view.sync();
	//запись результата
	dst_image.write(path + "/" + to_string(bi) +"bi" + info + "out.bmp");
	temp_image.write(path + "/" + to_string(bi) +"bi" + info + "in.bmp");
	//end of segmentation
	histWidth += 10;
	histHeight += 40;
	//сохранение всей гистограммы
	// hist_image.write(path + "/histogramm " + info + " " + to_string(bi) + ".jpg"); // hist_image.write(path + "/histogramm " + to_string(bi) + ".jpg");
	//сохранение половинки гистограммы - то что надо
	hist_image.crop(Geometry(histWidth/2+10, histHeight, histWidth/2-5, 0));
	hist_image.write(path + "/histogramm _ " + info + " " + to_string(bi) + ".jpg"); // hist_image.write(path + "/histogramm half " + to_string(bi) + ".jpg");
	// dump("drew histogramm ...");
	//Destructing stuff
	for(int i = 0; i < height; i++) {
		delete [] pi[i];
		delete [] PI[i];
		delete [] pi_average[i];
		delete [] channel_R_[i]; //Destructing the model of an image
	}	
	delete [] pi;
	delete [] PI;
	delete [] pi_average;
	delete [] channel_R_;
	//more stuff
	delete [] points;
	delete [] vicinities;
	delete [] point_index;
	delete [] minims;
	delete [] Colours;
	//destruct channel
	return channel_R_S;
}