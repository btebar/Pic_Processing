#include "PicLibrary.hpp"
#include "Colour.hpp"

mutex m1;

// TODO: implement the PicLibrary class

bool PicLibrary::isInLibrary(string filename) {
	return (picLibrary.find(filename) != picLibrary.end());
}

void PicLibrary::print_picturestore() {
	lib_lock();
	// a map is already sorted, so there is no need to sort it again
	for(auto& s : picLibrary){
		cout << s.first << endl;
	} 
	lib_unlock();
}

void PicLibrary::loadpicture(string path, string filename) {
	if(isInLibrary(filename)) {
		cerr << "File already in library" << endl;
	} else {
		Picture *file = new Picture(path);
		concurrent_pic *conc_pic = new concurrent_pic(file);
		/* SEQUENTIAL PART:
		picLibrary.insert(pair<string, Picture> (filename, file));*/
		picLibrary.insert({filename, conc_pic});
	}
}

void PicLibrary::unloadpicture(string filename) {
	if(isInLibrary(filename)) {
		/* Joins all the threads of the image before 
		exiting the program, saving or unloading */
		auto conc_pic = (picLibrary[filename]);
		picLibrary.erase(filename);
	} else {
		cerr << "File not in library" << endl;
	}

}

void PicLibrary::savepicture(string filename, string path) {
	if (!(isInLibrary(filename))){
		cerr << "File not in library" << endl;
	} else {
		auto conc_pic = picLibrary[filename];
		conc_pic->save_img(path);
	}
}

void PicLibrary::display(string filename) {
	lib_lock();
	if(!(isInLibrary(filename))) {
		cerr << "File not in library" << endl;
	} else{
		auto conc_pic = get(filename);
		cout<<"about to display"<<endl;
		conc_pic->display_pic();
	}
	lib_unlock();
}

concurrent_pic* PicLibrary::get(string filename) {
	return picLibrary[filename];
}


void PicLibrary::lib_lock() {
	m1.lock();
}

void PicLibrary::lib_unlock() {
	m1.unlock();
}

void PicLibrary::invert(string filename) {
	auto conc_pic = picLibrary[filename];
	lock_guard<mutex> lock(conc_pic->m);

	Picture pic = conc_pic->pic;
	int width = pic.getwidth();
	int height = pic.getheight();
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			int colorBlue = 255 - pic.getpixel(j, i).getblue();
			int colorGreen = 255 - pic.getpixel(j, i).getgreen();
			int colorRed = 255 - pic.getpixel(j, i).getred();
			Colour color(colorRed, colorGreen, colorBlue);
			conc_pic->pic.setpixel(j, i, color);
		}
	}
}

void PicLibrary::grayscale(string filename) {
	auto conc_pic = picLibrary[filename];
	lock_guard<mutex> lock(conc_pic->m);

	Picture pic = conc_pic->pic;
	int width = pic.getwidth();
	int height = pic.getheight();
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			int colorBlue = pic.getpixel(j, i).getblue();
			int colorGreen = pic.getpixel(j, i).getgreen();
			int colorRed = pic.getpixel(j, i).getred();
			int average = (int)(colorBlue+colorRed+colorGreen)/3;
			Colour color(average, average, average);
			conc_pic->pic.setpixel(j, i, color);
		}
	}

}

 void PicLibrary::rotate90(string filename) {
	auto conc_pic = (picLibrary[filename]);
	lock_guard<mutex> lock(conc_pic->m);
	
	Picture pic = conc_pic->pic;
	int width = pic.getwidth();
	int height = pic.getheight();
	Picture copy(height, width);
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			Colour color = pic.getpixel(j, i);
			copy.setpixel(height -1 -i, j, color);
		}
	}
	conc_pic->pic.setimage(copy.getimage());
}
	



void PicLibrary::rotate(int angle, string filename) {
	if (angle == 90) {
		rotate90(filename);
	} else if(angle == 180) {
		rotate90(filename);
		rotate90(filename);
	} else if(angle == 270) {
		rotate90(filename);
		rotate90(filename);
		rotate90(filename);
	}
}

void PicLibrary::flipVH(char plane, string filename) {
	auto conc_pic = picLibrary[filename];
	lock_guard<mutex> lock(conc_pic->m);

	Picture pic = conc_pic->pic;
	int width = pic.getwidth();
	int height = pic.getheight();
	Picture copy(width, height);
	if (plane == 'V') {
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				Colour color = pic.getpixel(j, height -1-i);
				copy.setpixel(j, i, color);
			}
		}
	} else {
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				Colour color = pic.getpixel(width -1-j, i);
				copy.setpixel(j, i, color);
			}
		}
	}
	conc_pic->pic.setimage(copy.getimage());

}

void PicLibrary::blur(string filename) {
	auto conc_pic = picLibrary[filename];
	lock_guard<mutex> lock(conc_pic->m);

	Picture pic = conc_pic->pic;
	int width = pic.getwidth();
	int height = pic.getheight();
	Picture copy(width, height);
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if ((i != 0) && (j != 0) && (i != height - 1)
            && (j != width-1)) {
				int sumRed = 0;
	          	int sumBlue = 0;
	          	int sumGreen = 0;
	          	for (int k = (i - 1); k <= (i + 1); k++) {
	            	for (int l = (j - 1); l <= (j + 1); l++) {
	              	Colour color = pic.getpixel(l, k);
	              	sumRed += color.getred();
	              	sumGreen += color.getgreen();
	              	sumBlue += color.getblue();
	            }
	          }
	          Colour newCol(sumRed / 9, sumGreen / 9, sumBlue / 9);
	          copy.setpixel(j, i, newCol);
	        } else {
	          copy.setpixel(j, i, pic.getpixel(j, i));
	        }
		}
	}
	conc_pic->pic.setimage(copy.getimage());
}

map<string, concurrent_pic*>* PicLibrary::getLibrary() {
	return &(picLibrary);
}
	
/* SEQUENTIAL CODE:

void PicLibrary::invert(string filename) {
	Picture pic = picLibrary[filename];
	int width = pic.getwidth();
	int height = pic.getheight();
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			int colorBlue = 255 - pic.getpixel(j, i).getblue();
			int colorGreen = 255 - pic.getpixel(j, i).getgreen();
			int colorRed = 255 - pic.getpixel(j, i).getred();
			Colour color(colorRed, colorGreen, colorBlue);
			pic.setpixel(j, i, color);
		}
	}
}

void PicLibrary::grayscale(string filename) {
	Picture pic = picLibrary[filename];
	int width = pic.getwidth();
	int height = pic.getheight();
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			int colorBlue = pic.getpixel(j, i).getblue();
			int colorGreen = pic.getpixel(j, i).getgreen();
			int colorRed = pic.getpixel(j, i).getred();
			int average = (int)(colorBlue+colorRed+colorGreen)/3;
			Colour color(average, average, average);
			pic.setpixel(j, i, color);
		}
	}
}

void PicLibrary::rotate(int angle, string filename) {
	int numRot = angle / 90;
	for (int i = 0; i < numRot; i++) {
		Picture pic = picLibrary[filename];
		int width = pic.getwidth();
		int height = pic.getheight();
		Picture copy(height, width);
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				Colour color = pic.getpixel(j, i);
				copy.setpixel(height -1 -i, j, color);
			}
		}
		picLibrary[filename].setimage(copy.getimage());
	}
}

void PicLibrary::flipVH(char plane, string filename) {
	Picture pic = picLibrary[filename];
	int width = pic.getwidth();
	int height = pic.getheight();
	Picture copy(width, height);
	if (plane == 'V') {
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				Colour color = pic.getpixel(j, height -1-i);
				copy.setpixel(j, i, color);
			}
		}
	} else {
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				Colour color = pic.getpixel(width -1-j, i);
				copy.setpixel(j, i, color);
			}
		}
	}
	picLibrary[filename].setimage(copy.getimage());
}

void PicLibrary::blur(string filename) {
	Picture pic = picLibrary[filename];
	int width = pic.getwidth();
	int height = pic.getheight();
	Picture copy(width, height);
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if ((i != 0) && (j != 0) && (i != height - 1)
            && (j != width-1)) {
				int sumRed = 0;
	          	int sumBlue = 0;
	          	int sumGreen = 0;
	          	for (int k = (i - 1); k <= (i + 1); k++) {
	            	for (int l = (j - 1); l <= (j + 1); l++) {
	              	Colour color = pic.getpixel(l, k);
	              	sumRed += color.getred();
	              	sumGreen += color.getgreen();
	              	sumBlue += color.getblue();
	            }
	          }
	          Colour newCol(sumRed / 9, sumGreen / 9, sumBlue / 9);
	          copy.setpixel(j, i, newCol);
	        } else {
	          copy.setpixel(j, i, pic.getpixel(j, i));
	        }

		}
	}
	picLibrary[filename].setimage(copy.getimage());
}

------------------------------------------------------------------------------------

------------------------------BLUR OPTIMISATION-------------------------------------

CREATING A THREAD FOR EACH PIXEL:

void blur_section(int i, int j, Picture *img, Picture *copy) {
	int sumRed = 0;
	int sumBlue = 0;
	int sumGreen = 0;
	if(i != 0 && i != (img->getheight() - 1) 
		&& j != 0 && j != (img->getwidth()-1)) {
		for (int k = (i - 1); k <= (i + 1); k++) {
			for (int l = (j - 1); l <= (j + 1); l++) {
				Colour color = img->getpixel(l, k);
				sumRed += color.getred();
				sumGreen += color.getgreen();
				sumBlue += color.getblue();
			}
		}
		Colour newcol(sumRed / 9, sumGreen / 9, sumBlue / 9);
		copy->setpixel(j, i, newcol);
	} else {
		copy->setpixel(j, i, img->getpixel(j, i));
	}
	
}

void PicLibrary::blur_thread_per_pixel(string filename) {
	vector<thread> threads_per_pixel;
	auto conc_pic = picLibrary[filename];
	lock_guard<mutex> lock(conc_pic->m);

	Picture pic = conc_pic->pic;
	int width = pic.getwidth();
	int height = pic.getheight();
	Picture copy(width, height);
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			threads_per_pixel.push_back(thread(blur_section, i, j, &pic, &copy));
		}
	}	
	for_each(threads_per_pixel.begin(), threads_per_pixel.end(), [](thread &t){t.join();});
	conc_pic->pic.setimage(copy.getimage());
}


CREATING THREAD PER ROW

*/
void blur_row(int i, Picture *img, Picture *copy) {
	int sumRed = 0;
	int sumBlue = 0;
	int sumGreen = 0;
	for(int j = 0; j < img->getwidth(); j++) {
		if(i != 0 && i != (img->getheight() - 1) 
			&& j != 0 && j != (img->getwidth()-1)) {
			for (int k = (i - 1); k <= (i + 1); k++) {
				for (int l = (j - 1); l <= (j + 1); l++) {
					Colour color = img->getpixel(l, k);
					sumRed += color.getred();
					sumGreen += color.getgreen();
					sumBlue += color.getblue();
				}
			}
			Colour newcol(sumRed / 9, sumGreen / 9, sumBlue / 9);
			copy->setpixel(j, i, newcol);
		} else {
			copy->setpixel(j, i, img->getpixel(j, i));
		}
	}
	
}

void PicLibrary::blur_thread_per_row(string filename) {
	vector<thread> threads_per_row;
	auto conc_pic = picLibrary[filename];
	unique_lock<mutex> lock(conc_pic->m);

	Picture pic = conc_pic->pic;
	int width = pic.getwidth();
	int height = pic.getheight();
	Picture copy(width, height);
	for (int i = 0; i < height; i++) {
		threads_per_row.push_back(thread(blur_row, i, &pic, &copy));
	}	
	for_each(threads_per_row.begin(), threads_per_row.end(), [](thread &t){t.join();});
	conc_pic->pic.setimage(copy.getimage());
}
/*
THREAD PER COLUMN:

void blur_col(int j, Picture *img, Picture *copy) {
	int sumRed = 0;
	int sumBlue = 0;
	int sumGreen = 0;
	for(int i = 0; i < img->getheight(); i++) {
		if(i != 0 && i != (img->getheight() - 1) 
			&& j != 0 && j != (img->getwidth()-1)) {
			for (int k = (i - 1); k <= (i + 1); k++) {
				for (int l = (j - 1); l <= (j + 1); l++) {
					Colour color = img->getpixel(l, k);
					sumRed += color.getred();
					sumGreen += color.getgreen();
					sumBlue += color.getblue();
				}
			}
			Colour newcol(sumRed / 9, sumGreen / 9, sumBlue / 9);
			copy->setpixel(j, i, newcol);
		} else {
			copy->setpixel(j, i, img->getpixel(j, i));
		}
	}
}

void PicLibrary::blur_thread_per_col(string filename) {
	vector<thread> threads_per_col;
	auto conc_pic = picLibrary[filename];
	unique_lock<mutex> lock(conc_pic->m);

	Picture pic = conc_pic->pic;
	int width = pic.getwidth();
	int height = pic.getheight();
	Picture copy(width, height);
	for (int j = 0; j < width; j++) {
		threads_per_col.push_back(thread(blur_col, j, &pic, &copy));
	}	
	for_each(threads_per_col.begin(), threads_per_col.end(), [](thread &t){t.join();});
	conc_pic->pic.setimage(copy.getimage());
}

*/

