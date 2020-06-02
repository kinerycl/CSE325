// Project 7 

//Author: Clare Kinery

#include <fstream>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include "string.h"
#include <iostream>
#include <cmath>
#include <stdlib.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <math.h>
#include <ctype.h>
#include <iostream>
#include <iomanip>
using namespace std;

struct cache_line
{
  char index;
  string tag = "000";
  int valid = 0; //initally not vaild
  int modified = 0; //initally not modified
  string data = "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00"; 
};

/* display cache */
void display_cache(vector<cache_line> cache)
{
  //header
  cout << "Index" << " " << "V " << "M " << "Tag " << "Data" << endl; 
  for(cache_line line : cache)
  {
    cout << "    " << line.index << " ";
    cout << line.valid << " ";
    cout << line.modified << " ";
    cout << line.tag << " ";
    cout << line.data << endl;
  }
  cout << "" << endl;
}

/** Intakes a string splits by space or tab
* returns a vector of strings 
*/
vector<string> array_make(string line)
{
  vector<string> splitString;
  string word = "";

  //check see if tab or space
  string check = "";
  for (unsigned int i=0; i<line.length(); i++)
  {
    check.push_back(line[i]);
    if(check == " " || check == "\t" || check == "\n")
    {
      if(word != "")
      {
        splitString.push_back(word);
        //reset word
        word = "";
      }
    }
    else
    {
      word += line[i];
    }

    //reset check
    check = "";
  } 

  //append final word if present
  if (word.length() > 0)
  {
    splitString.push_back(word);
  }
  return splitString;
}

int main(int argc, char* argv[], char* envp[])
{
  char input_line[128];

  //get input from user
  cin.getline(input_line, 128);

  //chage input to vector
  vector<string> commands = array_make(input_line);

  //filename
  string filename;

  //initalize cache
  vector<cache_line> cache;

  string hexnums = "0123456789abcdef";

  for(char num : hexnums)
  {
    cache_line line;
    line.index = num;
    cache.push_back(line);
  }

  int debug_cnt = 0;
  int vaild_refs = 0;

  //run commands
  for(unsigned int i = 0; i < commands.size(); i++)
  {
    // command variable
    string command = commands[i];

    //option -refs
    if(command == "-refs")
    {
      //check for file
      if(i + 1 < commands.size())
      {
        //check not a command, file is correct
        if(commands[i+1] != "-refs" && commands[i+1] != "-debug")
        {
          filename = commands[i+1];
          i++;
          vaild_refs = 1;

            }
        //2nd command is not a file
        else
        {
          cout << "file needed" << endl;
        }
      }
      //2nd command is not present, no file given
      else
      {
        cout << "file needed" << endl;
      }
    }
    //debug
    else if (command == "-debug") 
    {
      debug_cnt = 1;
    }
  }

  //debug and refs both set
  if(debug_cnt == 1 && vaild_refs == 1)
  {
    //loop through file
     
    FILE * file;
    char * line = NULL;
    size_t len =0;
    ssize_t ret_in;

    file = fopen(filename.c_str(), "r");
    if (file == NULL)
    {
      cout << "file doesn't exist" << endl;
    }
    else
    {
      //display contents at start of stimulation
      display_cache(cache);

      //run through file
      while((ret_in = getline(&line, &len, file)) != -1)
      {
        vector<string> line_vect = array_make(line);
        // display file
        cout << line_vect[0] << " ";
        cout << line_vect[1] << " ";
        cout << line_vect[0].at(3) << " ";
        cout << line_vect[0].substr(0,3) << " ";
        cout << line_vect[0].substr(4) << endl;;
        cout << "" << endl;
        //display contents after each memory reference
        display_cache(cache);
      }
      //display cache after simulation
      display_cache(cache);
    }
  }
  //only refs option set
  else if(vaild_refs == 1)
  {
   //loop through file
     
    FILE * file;
    char * line = NULL;
    size_t len =0;
    ssize_t ret_in;

    file = fopen(filename.c_str(), "r");
    if (file == NULL)
    {
      cout << "file doesn't exist" << endl;
    }
    else
    {
      //run through file
      while((ret_in = getline(&line, &len, file)) != -1)
      {
        vector<string> line_vect = array_make(line);
        // display file
        cout << line_vect[0] << " ";
        cout << line_vect[1] << " ";
        cout << line_vect[0].at(3) << " ";
        cout << line_vect[0].substr(0,3) << " ";
        cout << line_vect[0].substr(4) << endl;;
      }
      cout << "" << endl;
      //display cache after simulation
      display_cache(cache);

    }
  } 
  //no vaild input    
  else
  {
    cout << "not a vaild command" << endl;
  }
}
