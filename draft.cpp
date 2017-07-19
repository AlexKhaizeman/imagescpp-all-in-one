	//1
	for ( ssize_t row = 0; row < height ; row++ ){
		for ( ssize_t column = 0; column < width ; column++ ){			
			if (pi_average[row][column] <= 0.7){
				channel_R_S[row][column] = 255;
			}
			channel_R_S[row][column] = 255 - channel_R_S[row][column];
		}
	}
	//2
	//frame
	for ( ssize_t row = frameHeightHalf; row < height - frameHeightHalf; row++ ){
		for ( ssize_t column = frameWidthHalf; column < width - frameWidthHalf; column++ ){

			for ( ssize_t frameRow = row - frameHeightHalf; frameRow < row + frameHeightHalf; frameRow++ ){
				for ( ssize_t frameColumn = column - frameWidthHalf; frameColumn < column + frameWidthHalf; frameColumn++ ){

				}
			}

		}
	}
	//3