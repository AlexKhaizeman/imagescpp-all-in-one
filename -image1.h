//modified: pixels row are mirrowed!
// path - path to an output txt-file
// image_name - name of the image - argv[i+1] after argv[i] = "-i"
// bi - a number of binary image to process (from 1 to 8). If none all 8 are done.
// colourfull - false or true. If false the image will be prosecces as grey-scale image. If true the color-type will be deteced automatically.
// hardness - 0.1-0.99 - the minimum relative vicinity for the threshholds.
// points_number - number of points on histogramm
// version - 1, 2, 3 - a number of algorythm modification: 1 is for a simple aggorythm and 2 is for a color sensitive aggorythm for high probability parts.
void image1(string path, string image_name, int bi, bool colourfull, double hardness, int points_number, int version, int fS){	
	try{
		//описывает поток для записи данных в файл
		ofstream f; 
		f.open(path + "/output.txt", ios::app);
		string imagepath = "in/" + image_name;
		f << "Путь до изображения: " + imagepath << endl;
		Image src_image;
		// Read a file into image object	
		src_image.read(imagepath);
		//Write the initial image to an output folder
		src_image.write(path + "/" + image_name + ".jpg");
		//variables
		int
			x, y, k, m,
			frameWidth = 0,// frameWidthHalf = 0,
			frameHeight = 0,// frameHeightHalf = 0,
			// frameSquare = 0,
			seg_number = 0; //хитрая переменная: говорит, сколько БИ мы сегментируем. причём одна должна увеличиваться, если в старшем БИ мало сегментов и влезет ещё.. во как.	но пока это значение "от7ой до какой-то (6-0)"
			// points_number = 1000; //ok 500
		//pixel of the source image
		Pixels src_view(src_image);
		//width and height of the source image
		int width = src_image.size().width();
		int height = src_image.size().height();
		int square = width * height;
		if (version == 5){
			frameWidth = 7;
			frameHeight = 7;
		}
		else{
			frameWidth = fS;
			frameHeight = fS;
			// frameWidth = pow(log(width)/log(sqrt(2)), 1.15);
			// if ((frameWidth&1) == 0) frameWidth++;
			// frameHeight = pow(log(height)/log(sqrt(2)), 1.15);
			// if ((frameHeight&1) == 0) frameHeight++;			
		}
		//dump
		f<<"Ширина исходного изображения " + to_string(width) + "px, высота " + to_string(height) + "px"<<endl;		
		f<<"Поэтому ширина окна " + to_string(frameWidth) + "px, " + "высота окна " + to_string(frameHeight) + "px"<<endl;
		double accuracy = 1 / ((double) points_number);				
		f<<"Точность округления вероятности = " + to_string(accuracy) + " Количество точек " + to_string(points_number)<<endl;
		f<<"Жёсткость выбора порогов = " + to_string(hardness) <<endl;
		//dump end
		//результат склеивания каналов в случае цветной обработки
		Image dst_image(Geometry(width, height), ColorRGB(MaxRGB, MaxRGB, MaxRGB));
		dst_image.type(TrueColorType);
		dst_image.modifyImage();
		Pixels dst_view(dst_image);
		//Declare new sizes
		int old_height = height;
		height += 2*frameHeight;
		int old_width = width;
		width += 2*frameWidth;
		//Declare a model of an image.
		unsigned short **channel_R = new unsigned short*[height];
		unsigned short **channel_G = new unsigned short*[height];
		unsigned short **channel_B = new unsigned short*[height];
		unsigned short **channel_Grey = new unsigned short*[height];
		for(int i = 0; i < height; i++) {
			channel_R[i] = new unsigned short[width];
			channel_G[i] = new unsigned short[width];
			channel_B[i] = new unsigned short[width];
			channel_Grey[i] = new unsigned short[width];
		}
		//Declare a model of a result image.
		unsigned short **channel_R_S = new unsigned short*[height];
		unsigned short **channel_G_S = new unsigned short*[height];
		unsigned short **channel_B_S = new unsigned short*[height];
		unsigned short **channel_Grey_S = new unsigned short*[height];
		for(int i = 0; i < height; i++) {
			channel_R_S[i] = new unsigned short[width];
			channel_G_S[i] = new unsigned short[width];
			channel_B_S[i] = new unsigned short[width];
			channel_Grey_S[i] = new unsigned short[width];
		}
		//setting for zero
		for ( ssize_t row = 0; row < height ; row++ ){
			for ( ssize_t column = 0; column < width ; column++ ){			
				channel_R[row][column] = 0;
				channel_G[row][column] = 0;
				channel_B[row][column] = 0;
				channel_R_S[row][column] = 0;
				channel_G_S[row][column] = 0;
				channel_B_S[row][column] = 0;
				channel_Grey[row][column] = 0;
				channel_Grey_S[row][column] = 0;
			}
		}
		//цвет - ImageMagic
		Color pixel_color = *(src_view.get(1,1,1,1));
		ColorRGB pixel_rgb(pixel_color);
		//

		unsigned short r=0,g=0,b=0;
		//переписывание изображения в цвумерный массив-модель
		for ( ssize_t row = 0; row < old_height ; row++ ){
			for ( ssize_t column = 0; column < old_width ; column++ ){
				pixel_color = *(src_view.get(column,row,1,1)); //вот это нельяз ли упростить

				ColorRGB pixel_rgb(pixel_color);

				r=(unsigned short) 255*pixel_rgb.red();
				g=(unsigned short) 255*pixel_rgb.green();
				b=(unsigned short) 255*pixel_rgb.blue();

				//каналы отдельно
				channel_R[row+frameHeight][column+frameWidth] = r;
				channel_G[row+frameHeight][column+frameWidth] = g;
				channel_B[row+frameHeight][column+frameWidth] = b;
				//зеркальное отражение ряда пикселей по края картинки
				//верхняя полоса
				if (row <= frameHeight){
					channel_R[frameHeight-row][column+frameWidth] = r;
					channel_G[frameHeight-row][column+frameWidth] = g;
					channel_B[frameHeight-row][column+frameWidth] = b;
				}
				// правая полоса
				if (column >= old_width - frameWidth){
					channel_R[row+frameHeight][-column+2*old_width+frameWidth-1] = r;
					channel_G[row+frameHeight][-column+2*old_width+frameWidth-1] = g;
					channel_B[row+frameHeight][-column+2*old_width+frameWidth-1] = b;
				}								
				//нижняя полоса
				if (row >= old_height - frameHeight){
					channel_R[-row+2*old_height+frameHeight-1][column+frameWidth] = r;
					channel_G[-row+2*old_height+frameHeight-1][column+frameWidth] = g;
					channel_B[-row+2*old_height+frameHeight-1][column+frameWidth] = b;
				}
				//левая полоса
				if (column <= frameWidth){
					channel_R[frameHeight+row][-column+frameWidth] = r;
					channel_G[frameHeight+row][-column+frameWidth] = g;
					channel_B[frameHeight+row][-column+frameWidth] = b;
				}
				//обнуление результата заранее
				channel_R_S[row][column] = 0;
				channel_G_S[row][column] = 0;
				channel_B_S[row][column] = 0;
			}
		}

		for ( ssize_t row = 0; row < height ; row++ ){
			for ( ssize_t column = 0; column < width ; column++ ){			
				//серость
				channel_Grey[row][column] = (unsigned short) (channel_R[row][column]+channel_G[row][column]+channel_B[row][column])/3;
			}
		}		
		if (bi!=8){
			f<<"Будет обработано " + to_string(bi) + " разрядное изображение."<<endl;
			seg_number = bi;
			bi++;
		}
		//по умолчанию проверим на цветность. а вот если сказали, что не надо, то и не будем.
		if(colourfull){
			colourfull = ColorDetect(channel_R, channel_G, channel_B, width, height);
		}		
		//SEGMENTATION
		if (colourfull){
			f<<"Сегментация будет по всем каналам отдельно."<<endl;
			for (int i = seg_number; i < bi; ++i){
				//надо заводить отдельно каждый раз. Иначе оно не переписывается. Просто пишется несколько раз первая картинка. и всё.
				Image dst_image(Geometry(width, height), ColorRGB(0, 0, 0));
				dst_image.modifyImage();
				Pixels dst_view(dst_image);
				//поканальная обработка
				switch(version)
				{
					case 1:					
						channel_R_S = channelSegmentationCrop(path, "R", channel_R, width, height, i, points_number, frameWidth, frameHeight, hardness);
						channel_G_S = channelSegmentationCrop(path, "G", channel_G, width, height, i, points_number, frameWidth, frameHeight, hardness);
						channel_B_S = channelSegmentationCrop(path, "B", channel_B, width, height, i, points_number, frameWidth, frameHeight, hardness);
					break;
					case 5:					
						channel_R_S = channelSegmentationCrop5(path, "R", channel_R, width, height, i, points_number, frameWidth, frameHeight, hardness);
						channel_G_S = channelSegmentationCrop5(path, "G", channel_G, width, height, i, points_number, frameWidth, frameHeight, hardness);
						channel_B_S = channelSegmentationCrop5(path, "B", channel_B, width, height, i, points_number, frameWidth, frameHeight, hardness);
					break;
				}

				//рисуем итоговую карту "слоя" (цветную)
				for ( ssize_t row = 0; row < height ; row++ ){
					for ( ssize_t column = 0; column < width ; column++ ){
						pixel_color = ColorRGB((double) channel_R_S[row][column]/255, (double) channel_G_S[row][column]/255, (double) channel_B_S[row][column]/255);
						*(dst_view.get(column,row,1,1)) = pixel_color;
					}
				}
				dst_view.sync();
				//запись результата склеивания каналов для текущего РДИ
				// dst_image.write(path + "/" + to_string(i) +"bi.bmp");
				// dump("crop");
				// dst_image.crop(Geometry(frameWidth, frameHeight, width-frameWidth, height-frameHeight));
				dst_image.crop(Geometry(width-2*frameWidth,height-2*frameHeight,frameWidth,frameHeight));
				dst_image.write(path + "/" + to_string(i) +"bi.bmp");
			}
		}
		else{
			f<<"Сегментация будет по одному каналу (оттенки серого)."<<endl;
			for (int i = seg_number; i < bi; ++i){
				//надо заводить отдельно каждый раз. Иначе оно не переписывается. Просто пишется несколько раз первая картинка. и всё.
				Image dst_image(Geometry(width, height), ColorRGB(0, 0, 0));
				dst_image.modifyImage();
				Pixels dst_view(dst_image);

				switch(version)
				{
					case 1:					
						channel_R_S = channelSegmentationCrop(path, "grey", channel_R, width, height, i, points_number, frameWidth, frameHeight, hardness);
					break;
					case 5:					
						channel_R_S = channelSegmentationCrop5(path, "grey", channel_R, width, height, i, points_number, frameWidth, frameHeight, hardness);
					break;
				}
			}
		}
		//Destructing the model of an image
		for(int i = 0; i < height; ++i) {
			delete [] channel_R[i];
			delete [] channel_G[i];
			delete [] channel_B[i];
			delete [] channel_Grey[i];
			delete [] channel_R_S[i];
			delete [] channel_G_S[i];
			delete [] channel_B_S[i];
			delete [] channel_Grey_S[i];
		}
		delete [] channel_R;
		delete [] channel_G;
		delete [] channel_B;
		delete [] channel_Grey;
		delete [] channel_R_S;
		delete [] channel_G_S;
		delete [] channel_B_S;
		delete [] channel_Grey_S;		
		f.close();
	}
	catch( Exception &error_ ){
		cout << "Caught exception: " << error_.what() << endl;
	}
}