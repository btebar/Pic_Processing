#include <iostream>

#include "Colour.hpp"
#include "Utils.hpp"
#include "Picture.hpp"
#include "PicLibrary.hpp"

using namespace std;

// TODO: Implement the picture library command-line interpreter

int main(int argc, char ** argv)
{


  cout << "------------------------------------------------------" << endl; 
  cout << "WELCOME to the C++ Picture Processing Interpreter" << endl;
  cout << "------------------------------------------------------" << endl;

  // write main IO loop that runs the command-line interpreter interactive shell
  PicLibrary picLib;
  int i = 0;
  bool fin = true;

  /* This code(before de while loop) manages the case in which the user commands 
     are of the type: ./picture_lib images/ducks1.jpg  */
  if(argc > 1) {
  	for(int i = 1; i < argc; i++) {
  		string path = argv[i];
  		int index = 0;
  		for(int j = 0; j < sizeof(path); j++) {
  			if(path[j] == '/') {
  				index = j+1;
  				break;
  			}
  		}
  		char* name = &path[index];
  		picLib.loadpicture(path, name);
  	}
  }

  while(fin && cin.peek() != EOF) {
	cout <<"Enter your commands: " <<endl;
	string line;
	cin >> line;
	//LOAD
	if(!strcmp(line.c_str(),"load")) {
		string path;
		cin >> path;
		string filename;
		cin >> filename;
		picLib.loadpicture(path, filename);
		cout<< "picture loaded"<< endl;
	//DISPLAY
	} else if(!strcmp(line.c_str(),"display")) {
		string filename;
		cin >> filename;
		picLib.display(filename);
		cout<< "picture displayed"<< endl;
	//UNLOAD
	} else if(!strcmp(line.c_str(),"unload")) {
		string filename;
		cin >> filename;
		picLib.get(filename)->finish_threads();
		picLib.unloadpicture(filename);
		cout<< "picture unloaded"<< endl;
	//SAVE
	} else if(!strcmp(line.c_str(),"save")) {
		string filename;
		cin >> filename;
		string path;
		cin >> path;
		picLib.get(filename)->finish_threads();
		picLib.savepicture(filename, path);
		cout<< filename << " saved in "<< path << endl;
	//INVERT
	} else if(!strcmp(line.c_str(),"invert")) {
		string filename;
		cin >> filename;
		if(picLib.isInLibrary(filename)) {
			auto conc_pic = (picLib.get(filename));
			(conc_pic->threads).push_back(thread(&PicLibrary::invert, &picLib, filename));
			cout<< "picture inverted"<< endl;
		} else {
			cerr<<"file not in library"<< endl;
		}
	//GRAYSCALE
	} else if(!strcmp(line.c_str(),"grayscale")) {
		string filename;
		cin >> filename;
		if(picLib.isInLibrary(filename)) {
			auto conc_pic = (picLib.get(filename));
			(conc_pic->threads).push_back(thread(&PicLibrary::grayscale,
										 &picLib, filename));
			cout<< "picture grayscaled"<< endl;
		} else {
			cerr<<"file not in library"<< endl;
		}
	//ROTATE
	} else if(!strcmp(line.c_str(),"rotate")) {
		int rotations;
		cin >> rotations;
		string filename;
		cin >> filename;
		if(picLib.isInLibrary(filename)) {
			auto conc_pic = (picLib.get(filename));
			(conc_pic->threads).push_back(thread(&PicLibrary::rotate,
									&picLib, rotations, filename));
		cout<< "picture rotated"<< endl;
		} else {
			cerr<<"file not in library"<< endl;
		}
	} else if(!strcmp(line.c_str(),"flip")) {
		char flip;
		cin >> flip;
		string filename;
		cin >> filename;
		if(picLib.isInLibrary(filename)) {
			auto conc_pic = (picLib.get(filename));
			(conc_pic->threads).push_back(thread(&PicLibrary::flipVH,
									&picLib, flip, filename));
			cout<< "picture flipped "<< endl;
		} else {
			cerr<<"file not in library"<< endl;
		}
	//BLUR
	} else if(!strcmp(line.c_str(),"blur")) {
		string filename;
		cin >> filename;
		if(picLib.isInLibrary(filename)) {
			auto conc_pic = (picLib.get(filename));
			(conc_pic->threads).push_back(thread(&PicLibrary::blur_thread_per_row,
											 &picLib, filename));
			cout<< "picture blurred"<< endl;
		} else {
			cerr<<"file not in library"<< endl;
		}
	//EXIT
	} else if(!strcmp(line.c_str(),"exit")) {
		/* This will be executed at the end of the program,
			waiting for all threads to terminate*/
		if(!((*picLib.getLibrary()).empty())) {
			for(auto& im : (*picLib.getLibrary())) {
				(im.second)->finish_threads();
			}
			for(auto& im : (*picLib.getLibrary())) {
				(im.second)->threads.clear();
			}
		}
		cout<< "byebye..."<<endl;
		fin = false;
		break;
	//LISTSTORE
	}else if (!strcmp(line.c_str(), "liststore")) {
		picLib.print_picturestore();
		cout << "liststore printed" << endl;
	//INCORRECT COMMAND
	} else {
		cerr << " incorrect argument" << endl;
	}
  }

return 0;
}


