//факториал
int fact(int input){
	int fact=1;
	while (input>0){
		fact *=input--;
	}
	return fact;
}

//
//
//
//
//
//
//
void channelCompare(string path, string info, unsigned short **channel_R, unsigned short **channel_R_S, unsigned short **channel_R_C, int width, int height){
	ofstream f;
	f.open(path + "/output.txt", ios::app);	
	// dump(info);
	int square = width * height, index = 0;
	double seg_error=0;
	std::list<unsigned short> colours_;
	std::list<unsigned short> colours_S;
	std::list<unsigned short>::iterator it_ = colours_.begin();
	std::list<unsigned short>::iterator it_S = colours_.begin();

	//just a check
	dump("Pshhh");
	f<<"Pshhh"<<endl;
	seg_error = 0;
	for ( ssize_t row = 0; row < height ; row++ ){
		for ( ssize_t column = 0; column < width ; column++ ){
			if (channel_R[row][column] == channel_R_S[row][column]){
				channel_R_C[row][column] = 255;
			}
			else{
				seg_error++;
				channel_R_C[row][column] = 0;
			}				
		}
	}
	seg_error /= square/100;

	dump("Blind error " + to_string(seg_error));
	f<<"Blind error " + to_string(seg_error)<<endl;

	seg_error=0;
	//just a check

	//надо типа не так, а использовать вероятностный подход
	/*std::list<double> part_;
	std::list<double> part_S;
	std::list<double>::iterator it_part_ = part_.begin();
	std::list<double>::iterator it_part_S = part_S.begin();
	double* p;*/
	bool thereis = false;
	
	/*for ( ssize_t row = 0; row < height ; row++ ){
		for ( ssize_t column = 0; column < width ; column++ ){								
			thereis = false;
			it_part_ = part_.begin();  			
  			for (std::list<unsigned short>::iterator it = colours_.begin(); it != colours_.end(); ++it){
  				it_part_++;
  				if (*it == channel_R[row][column]){
  					thereis = true;  					
  					*it_part_ = *it_part_ + 1;
  				}
  			}

  			if (!thereis){
				colours_.push_back(channel_R[row][column]);
				part_.push_back(1);
			}

			thereis = false;
			it_part_S = part_S.begin();
  			for (std::list<unsigned short>::iterator it = colours_S.begin(); it != colours_S.end(); ++it){
  				it_part_S++;
  				if (*it == channel_R_S[row][column]){
  					thereis = true;
  					*it_part_S = *it_part_S + 1;
  				}
  			}
  			if (!thereis){
				colours_S.push_back(channel_R_S[row][column]);
				part_S.push_back(1);
			}
		}
	}*/		

	for ( ssize_t row = 0; row < height ; row++ ){
		for ( ssize_t column = 0; column < width ; column++ ){								
			thereis = false;
  			for (std::list<unsigned short>::iterator it = colours_.begin(); it != colours_.end(); ++it)
  				if (*it == channel_R[row][column])
  					thereis = true;  		
  			if (!thereis)
				colours_.push_back(channel_R[row][column]);
			thereis = false;
  			for (std::list<unsigned short>::iterator it = colours_S.begin(); it != colours_S.end(); ++it)
  				if (*it == channel_R_S[row][column])
  					thereis = true;
  			if (!thereis)
				colours_S.push_back(channel_R_S[row][column]);  				
		}
	}		

	// dump("push done");

	double *part_ = new double[colours_.size()];
	int *colours_array = new int[colours_.size()];
	double *part_S = new double[colours_S.size()];
	int *colours_array_S = new int[colours_S.size()];

	for (int i = 0; i < colours_.size(); i++)
		part_[i] = 0;

	for (int i = 0; i < colours_S.size(); i++)
		part_S[i] = 0;

	index = 0;
	for (std::list<unsigned short>::iterator it = colours_.begin(); it != colours_.end(); ++it)
		colours_array[index++] = *it;

	index = 0;
	for (std::list<unsigned short>::iterator it = colours_S.begin(); it != colours_S.end(); ++it)
		colours_array_S[index++] = *it;

	for ( ssize_t row = 0; row < height ; row++ ){
		for ( ssize_t column = 0; column < width ; column++ ){
			for (int i = 0; i < colours_.size(); i++)
				if (channel_R[row][column] == colours_array[i])
					part_[i]++;
			for (int i = 0; i < colours_S.size(); i++)
				if (channel_R_S[row][column] == colours_array_S[i])
					part_S[i]++;
		}
	}		

    //попробуем просто отсортировать
    for (std::size_t idx_i = 0; idx_i < colours_.size() - 1; idx_i++)
    {
        for (std::size_t idx_j = 0; idx_j < colours_.size() - idx_i - 1; idx_j++)
        {
            if (part_[idx_j + 1] > part_[idx_j])
            {
                std::swap(part_[idx_j], part_[idx_j + 1]);
                std::swap(colours_array[idx_j], colours_array[idx_j + 1]);
            }
        }
    }

    for (std::size_t idx_i = 0; idx_i < colours_S.size() - 1; idx_i++)
    {
        for (std::size_t idx_j = 0; idx_j < colours_S.size() - idx_i - 1; idx_j++)
        {
            if (part_S[idx_j + 1] > part_S[idx_j])
            {
                std::swap(part_S[idx_j], part_S[idx_j + 1]);
                std::swap(colours_array_S[idx_j], colours_array_S[idx_j + 1]);
            }
        }
    }

	f<<info<<endl;
	// f<<"	"+info<<endl;
	// f<<"etalon"<<endl;
	for (int i = 0; i < colours_.size(); i++){
		part_[i] = (double) part_[i] * 100 / square;
		// f<<to_string(colours_array[i]) + " is colour from 1 part " + to_string(part_[i])<<endl;
		// f<<to_string(colours_array[i]) + " цвет на эталонном изображении " + to_string(part_[i])<<endl;
	}
		
    // f<<"secondary"<<endl;
	for (int i = 0; i < colours_S.size(); i++){
		part_S[i] = (double) part_S[i] * 100 / square;
		// f<<to_string(colours_array_S[i]) + " is colour from 2 part " + to_string(part_S[i])<<endl;		
		// f<<to_string(colours_array_S[i]) + " цвет на сравниваемом изображении " + to_string(part_S[i])<<endl;		
	}

    	/*//playground
		for ( ssize_t row = 0; row < height ; row++ ){
			for ( ssize_t column = 0; column < width ; column++ ){
				channel_R_C[row][column] = (channel_R[row][column] + channel_R_S[row][column])/2;
			}
		}*/		

	/*
    //если в эталоне больше цветов
    if(colours_.size()>colours_S.size()){
		for ( ssize_t row = 0; row < height ; row++ ){
			for ( ssize_t column = 0; column < width ; column++ ){
				for (int i = 0; i < colours_S.size(); i++)
					if (channel_R[row][column] == colours_array[i]){
						if (channel_R_S[row][column] == colours_array_S[i])
							channel_R_C[row][column] = 255;
						else{
							seg_error++;
							channel_R_C[row][column] = 0;
						}							
					}
			}
		}
	    seg_error = 100*seg_error/square;
		for (int i = colours_S.size(); i < colours_.size(); i++)
			seg_error += part_[i];
    } //если в эталоне цветов меньше или столько же
    else{
		for ( ssize_t row = 0; row < height ; row++ ){
			for ( ssize_t column = 0; column < width ; column++ ){
				for (int i = 0; i < colours_.size(); i++)
					if (channel_R_S[row][column] == colours_array_S[i]){
						if (channel_R[row][column] == colours_array[i])	
							channel_R_C[row][column] = 255;
						else{
							seg_error++;
							channel_R_C[row][column] = 0;
						}							
					}
			}
		}
		seg_error = 100*seg_error/square;		
		for (int i = colours_.size(); i < colours_S.size(); i++)
			seg_error += part_S[i];			
    }
    dump(info + " old segmentation error " + to_string(seg_error) + "%");
    f<<"old segmentation error " + to_string(seg_error) + "%"<<endl;			
    */

	//реализуем табличку пересечений (хотя бы для себя)
	int **match = new int*[colours_S.size()];
	//не можешь контролировать ситуацию - создай дублёра
	int **match_ = new int*[colours_S.size()];
	for (int j = 0; j < colours_S.size(); j++){
		match[j] = new int [colours_.size()];
		match_[j] = new int [colours_.size()];		
	}
	for (int i = 0; i < colours_S.size(); i++){
		for (int j = 0; j < colours_.size(); j++){
			match[i][j] = 0;
			match_[i][j] = 0;
		}
	}	

	//совпадения цветов
	for ( ssize_t row = 0; row < height ; row++ ){
		for ( ssize_t column = 0; column < width ; column++ ){
			for (int i = 0; i < colours_S.size(); i++){
				for (int j = 0; j < colours_.size(); j++){
					if (channel_R[row][column] == colours_array[j]){
						if (channel_R_S[row][column] == colours_array_S[i]){
							match[i][j] += 1;
						}
					}
				}
			}
		}
	}

    /*f<<"match table "<<endl;
    f<<"таблица сравнениея "<<endl;
    f<<"           ";
	for (int j = 0; j < colours_.size(); j++){
		f<<"colour " + to_string(colours_array[j])<<' ';
	}
	f<<endl;

	for (int i = 0; i < colours_S.size(); i++){
		f<<"colour " + to_string(colours_array_S[i])<<" | ";
		for (int j = 0; j < colours_.size(); j++){
			f<<match[i][j]<<"      ";
		}
		f<<endl;
	}*/

	//количество цветов в коротком массиве?
	int colours_number = colours_.size();
	if (colours_number > colours_S.size())
		colours_number = colours_S.size();
	int current_max = 0, current_i = 0, current_j = 0;
	seg_error = 0;

	//подбор соответствия цветов
	for (int k=0; k<colours_number; k++){
		//найти максимум 
		current_max = 0;
		for (int i = 0; i < colours_S.size(); i++){
			for (int j = 0; j < colours_.size(); j++){
				if (match[i][j] > current_max){
					current_max = match[i][j];
					current_i = i;
					current_j = j;
				}
			}
		}		
		match_[current_i][current_j] = match[current_i][current_j];
		//и зачистить столбец и строку с ним
		for (int i = 0; i < colours_S.size(); i++){
			// if (i != current_i)
				match[i][current_j] = 0;
		}
		for (int j = 0; j < colours_.size(); j++){
			// if (j != current_j)
				match[current_i][j] = 0;
		}
		
	}

    /*f<<"match table - analyzed"<<endl;
    f<<"           ";
	for (int j = 0; j < colours_.size(); j++){
		f<<"colour " + to_string(colours_array[j])<<' ';
	}
	f<<endl;*/

	//расчёт ошибки
	seg_error = 0;
	for (int i = 0; i < colours_S.size(); i++){
		// f<<"colour " + to_string(colours_array_S[i])<<" | ";
		for (int j = 0; j < colours_.size(); j++){
			// f<<match_[i][j]<<"      ";
			seg_error += match_[i][j];
		}
		// f<<endl;
	}

	seg_error = square - seg_error;
	seg_error /= square/100;

    //раскраска
	for ( ssize_t row = 0; row < height ; row++ ){
		for ( ssize_t column = 0; column < width ; column++ ){
			for (int i = 0; i < colours_S.size(); i++){
				for (int j = 0; j < colours_.size(); j++){
					if (channel_R[row][column] == colours_array[j]){
						if (channel_R_S[row][column] == colours_array_S[i]){
							if (match_[i][j] != 0)
								channel_R_C[row][column] = 255;
							else
								channel_R_C[row][column] = 0;
						}
					}
				}
			}
		}
	}	
	
    dump(info + " segmentation error " + to_string(seg_error) + "%");
    // f<<"segmentation error " + to_string(seg_error) + "%"<<endl;
    f<<"ошибка сегментации " + to_string(seg_error) + "%"<<endl;

/*	if (seg_error > 0){
		//тут какая-то гадость происходит
		dump("Started full check variant");
		f<<"Started full check variant"<<endl;
		seg_error = 0;

		double seg_error_min = 1;
		colours_number--;
		int variants = fact(colours_number);
		dump("colour number is " + to_string(colours_number));
		dump("colour 1 number is " + to_string(colours_.size()));
		dump("colour 2 number is " + to_string(colours_S.size()));

		//смотрим какой массив короче
		for (int k=0; k<colours_number; k++){
			for (int l=0; l<colours_number; l++){
				//совпадения цветов
				for ( ssize_t row = 0; row < height ; row++ ){
					for ( ssize_t column = 0; column < width ; column++ ){

						if (channel_R[row][column] == colours_array[l]){
							if (channel_R_S[row][column] == colours_array_S[l]){
								seg_error += 1;
								// channel_R_C[row][column] = 255;
							}
							// else{
								// channel_R_C[row][column] = 0;
							// }
						}

					}
				}
				std::swap(colours_array_S[l], colours_array_S[l+1]);

				seg_error = square - seg_error;
				seg_error /= square/100;

				if (seg_error < seg_error_min){
					seg_error_min = seg_error;
				}

				dump(seg_error);
			}
		}
	}*/

    for (int j = 0; j < colours_S.size(); j++){
    	delete [] match[j];
    	delete [] match_[j];
    }
    delete [] match;
    delete [] match_;
}

//
//
//
// 
void compareImages(string path, Image etalon_image, Image secondary_image, string info){
	ofstream f;
	f.open(path + "/output.txt", ios::app);	
	Pixels etalon(etalon_image);
	Pixels secondary(secondary_image);
	bool colourfull = false;
	if (etalon_image.size() == secondary_image.size())
	{
		try{
			int width = etalon_image.size().width();
			int height = etalon_image.size().height();
			int square = width * height;

			Image dst_image(Geometry(width, height), ColorRGB(MaxRGB, MaxRGB, MaxRGB));
			dst_image.modifyImage();
			Pixels dst_view(dst_image);

			//Declare a model of an etalon image.
			unsigned short **channel_R = new unsigned short*[height];
			unsigned short **channel_G = new unsigned short*[height];
			unsigned short **channel_B = new unsigned short*[height];
			for(int i = 0; i < height; i++) {
				channel_R[i] = new unsigned short[width];
				channel_G[i] = new unsigned short[width];
				channel_B[i] = new unsigned short[width];
			}

			//Declare a model of a secondary image.
			unsigned short **channel_R_S = new unsigned short*[height];
			unsigned short **channel_G_S = new unsigned short*[height];
			unsigned short **channel_B_S = new unsigned short*[height];
			for(int i = 0; i < height; i++) {
				channel_R_S[i] = new unsigned short[width];
				channel_G_S[i] = new unsigned short[width];
				channel_B_S[i] = new unsigned short[width];
			}

			//Declare a model of a comporation.
			unsigned short **channel_R_C = new unsigned short*[height];
			unsigned short **channel_G_C = new unsigned short*[height];
			unsigned short **channel_B_C = new unsigned short*[height];
			for(int i = 0; i < height; i++) {
				channel_R_C[i] = new unsigned short[width];
				channel_G_C[i] = new unsigned short[width];
				channel_B_C[i] = new unsigned short[width];
			}

			Color pixel_color = *(etalon.get(1,1,1,1));
			ColorRGB pixel_rgb(pixel_color);

			Color pixel_color_ = *(secondary.get(1,1,1,1));
			ColorRGB pixel_rgb_(pixel_color);

			for ( ssize_t row = 0; row < height ; row++ ){
				for ( ssize_t column = 0; column < width ; column++ ){
					pixel_color = *(etalon.get(column,row,1,1)); //вот это нельяз ли упростить
					ColorRGB pixel_rgb(pixel_color);					
					channel_R[row][column] = (unsigned short) 255*pixel_rgb.red();
					channel_G[row][column] = (unsigned short) 255*pixel_rgb.green();
					channel_B[row][column] = (unsigned short) 255*pixel_rgb.blue();		

					pixel_color_ = *(secondary.get(column,row,1,1)); //вот это нельяз ли упростить
 	 				ColorRGB pixel_rgb_(pixel_color_);
					channel_R_S[row][column] = (unsigned short) 255*pixel_rgb_.red();
					channel_G_S[row][column] = (unsigned short) 255*pixel_rgb_.green();
					channel_B_S[row][column] = (unsigned short) 255*pixel_rgb_.blue();								

					channel_R_C[row][column] = 0;
					channel_G_C[row][column] = 0;
					channel_B_C[row][column] = 0;

					//провекрка на цветность				
					if ((channel_R[row][column]!=channel_G[row][column])||(channel_R[row][column]!=channel_B[row][column])||(channel_B[row][column]!=channel_G[row][column])){
						colourfull = true;
					}

					if ((channel_R_S[row][column]!=channel_G_S[row][column])||(channel_R_S[row][column]!=channel_B_S[row][column])||(channel_B_S[row][column]!=channel_G_S[row][column])){
						colourfull = true;
					}
				}
			}

			if (colourfull){
				Image dst_image_R(Geometry(width, height), ColorRGB(MaxRGB, MaxRGB, MaxRGB));
				dst_image_R.modifyImage();
				Pixels dst_view_R(dst_image_R);
				Image dst_image_G(Geometry(width, height), ColorRGB(MaxRGB, MaxRGB, MaxRGB));
				dst_image_G.modifyImage();
				Pixels dst_view_G(dst_image_G);
				Image dst_image_B(Geometry(width, height), ColorRGB(MaxRGB, MaxRGB, MaxRGB));
				dst_image_B.modifyImage();
				Pixels dst_view_B(dst_image_B);

				channelCompare(path, "comparing test RED", channel_R, channel_R_S, channel_R_C, width, height);
				channelCompare(path, "comparing test GREEN", channel_G, channel_G_S, channel_G_C, width, height);
				channelCompare(path, "comparing test BLUE", channel_B, channel_B_S, channel_B_C, width, height);

				//рисуем итоговую разницу карт R
				for ( ssize_t row = 0; row < height ; row++ ){
					for ( ssize_t column = 0; column < width ; column++ ){
						pixel_color = ColorRGB((double) channel_R_C[row][column]/255, (double) channel_R_C[row][column]/255, (double) channel_R_C[row][column]/255);
						*(dst_view_R.get(column,row,1,1)) = pixel_color;
					}
				}		

				dst_view_R.sync();
				dst_image_R.write(path + "/comporation R" + info + ".bmp");

				//рисуем итоговую разницу карт G
				for ( ssize_t row = 0; row < height ; row++ ){
					for ( ssize_t column = 0; column < width ; column++ ){
						pixel_color = ColorRGB((double) channel_G_C[row][column]/255, (double) channel_G_C[row][column]/255, (double) channel_G_C[row][column]/255);
						*(dst_view_G.get(column,row,1,1)) = pixel_color;
					}
				}		

				dst_view_G.sync();
				dst_image_G.write(path + "/comporation G" + info + ".bmp");

				//рисуем итоговую разницу карт B
				for ( ssize_t row = 0; row < height ; row++ ){
					for ( ssize_t column = 0; column < width ; column++ ){
						pixel_color = ColorRGB((double) channel_B_C[row][column]/255, (double) channel_B_C[row][column]/255, (double) channel_B_C[row][column]/255);
						*(dst_view_B.get(column,row,1,1)) = pixel_color;
					}
				}		

				dst_view_B.sync();
				dst_image_B.write(path + "/comporation B" + info + ".bmp");


				//рисуем итоговую разницу карт
				for ( ssize_t row = 0; row < height ; row++ ){
					for ( ssize_t column = 0; column < width ; column++ ){
						pixel_color = ColorRGB((double) channel_R_C[row][column]/255, (double) channel_G_C[row][column]/255, (double) channel_B_C[row][column]/255);
						*(dst_view.get(column,row,1,1)) = pixel_color;
					}
				}		

				dst_view.sync();
				dst_image.write(path + "/comporation" + info + ".bmp");
			}
			else
			{
				channelCompare(path, "comparing test grey", channel_R, channel_R_S, channel_R_C, width, height);

				//рисуем итоговую разницу карт
				for ( ssize_t row = 0; row < height ; row++ ){
					for ( ssize_t column = 0; column < width ; column++ ){
						pixel_color = ColorRGB((double) channel_R_C[row][column]/255, (double) channel_R_C[row][column]/255, (double) channel_R_C[row][column]/255);
						*(dst_view.get(column,row,1,1)) = pixel_color;
					}
				}		

				dst_view.sync();
				dst_image.write(path + "/comporation" + info + ".bmp");
			}

			//Destructing the model of an image
			for(int i = 0; i < height; ++i) {
				delete [] channel_R[i];
				delete [] channel_G[i];
				delete [] channel_B[i];
				delete [] channel_R_S[i];
				delete [] channel_G_S[i];
				delete [] channel_B_S[i];
				delete [] channel_R_C[i];
				delete [] channel_G_C[i];
				delete [] channel_B_C[i];
			}
			delete [] channel_R;
			delete [] channel_G;
			delete [] channel_B;
			delete [] channel_R_S;
			delete [] channel_G_S;
			delete [] channel_B_S;
			delete [] channel_R_C;
			delete [] channel_G_C;
			delete [] channel_B_C;

		}
		catch( Exception &error_ ){       
			cout << "Caught exception: " << error_.what() << endl;
			f << "Caught exception: " << error_.what() << endl;
		}    		
	}
	else{
		dump("Image sizes don't match.");
	}
}
// path - путь до папки с картинками
// base_image_name - имя эталонного изображения
// check_image_name - имя сравниваемого изображения
void CompareImages(string path, string base_image_name, string check_image_name){
	//declare images
	Image seg_etalon;
	Image seg_other;
	//images are in in-folder
	string imagepath_etalon = "in/";
	imagepath_etalon += base_image_name;
	string imagepath_other = "in/";
	imagepath_other += check_image_name;	
	try{
		seg_etalon.read(imagepath_etalon);
		seg_other.read(imagepath_other);
		compareImages(path, seg_etalon, seg_other, "");
		seg_etalon.write(path + "/etalon.bmp");
		seg_other.write(path + "/other.bmp");		
	}
	catch( Exception &error_ ){       
		cout << "Caught exception: " << error_.what() << endl;
	}
}