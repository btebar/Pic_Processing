#ifndef PICLIB_H
#define PICLIB_H

#include "Picture.hpp"
#include "Utils.hpp"
using namespace std;

/* Thread safety implementation:
  The command line interprets the commands typed by the user. According to them, 
  it calls the corresponding method.Load/unload/safe/display/liststore are commands
  that affect the whole library, since we can't load an image while printing the 
  library if liststore was called before load. Therefore, those commands are executed
  sequentially and there is a mutex to avoid race conditions.
  Picture transformation methods:
  Since different pictures can be edited at the same time, but we have to be careful
  with the changes that take part on the same picture, I decided to change the structure
  of my library in order to be able to assign a mutex and different threads to each image.
  The threads are being added to the vector of threads of each library picture in order,
  while executing their tasks. When a thread starts running, the concurrent_pic elem of 
  the library is locked by the guard_lock, which remains locked until it leaves the scope of
  the method call.
  Therefore, the other threads have to wait until the first one is finished.
  They continue executing, in the order they were created, locking the elem everytime one 
  runs.
  Before calling unload, save or exit, the threads of the image being unloaded/saved or every
  thread of the program in the exit-program case, have to be .join(). The vector of threads 
  has to be cleared as well. We do that in order to finish all the remaining tasks that were
  supposed to be executed before the save/load/exit command was called.
  */


class PicLibrary {
  private:
    //map<string, Picture> picLibrary;
  map<string, concurrent_pic*> picLibrary;
    
  public:
  // defaiult constructor/deconstructor
  PicLibrary(){};
  ~PicLibrary(){};

  concurrent_pic* get(string filename);
  void lib_lock();
  void lib_unlock();
  bool isInLibrary(string filename);

  // command-line interpreter routines
  void print_picturestore();
  void loadpicture(string path, string filename);
  void unloadpicture(string filename);
  void savepicture(string filename, string path);
  void display(string filename);
  
  
  // picture transformation routines
  void invert(string filename);
  void grayscale(string filename);
  void rotate90(string filename);
  void rotate(int angle, string filename);
  void flipVH(char plane, string filename);
  void blur(string filename);

  map<string, concurrent_pic*>* getLibrary();

  // Blur optimisation
  //void blur_thread_per_pixel(string filename);
  void blur_thread_per_row(string filename);
  //void blur_thread_per_col(string filename);
};

#endif

