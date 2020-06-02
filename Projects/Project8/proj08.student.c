// Project 8 

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

//ram struct
struct ram_data
{
  vector<string> data = {"00 00 00 00", "00 00 00 00", "00 00 00 00", "00 00 00 00"};
};

// return from read
struct read_data
{
  string data;
  int hit = 0;
};

//initalize ram
//ram_data RAM[1048576/16];

struct cache_line
{
  string address;
  char index;
  string tag = "000";
  int valid = 0; //initally not vaild
  int modified = 0; //initally not modified
  vector<string> data = {"00 00 00 00", "00 00 00 00", "00 00 00 00", "00 00 00 00"};
};

//initalize cache
vector<cache_line> cache;

/* write the data in cache*/
int write(string address, char index, string tag, string offset,
    string data, ram_data *RAM)
{
  string set_address = address.substr(0, 4) + "0";
  int hit =0;
  int offset_int = atoi(offset.c_str());
  int c_index = offset_int / 4;
  for(int i = 0; i < 16; i++ )
  {
    if(cache[i].index == index)
    {
      //vaild to set, initally setting
      if(cache[i].valid == 0)
      {
        cache[i].valid = 1;
        cache[i].modified = 1;

        //set the cache
        cache[i].data[c_index] = data;
        cache[i].tag = tag;
        cache[i].address = set_address;
      }
      //modify current cache
      else if(cache[i].valid == 1 && cache[i].tag == tag)
      {
        hit = 1;
        cache[i].valid = 1;
        cache[i].modified = 1;
      }
      //put current and new in ram and write to cache
      else if(cache[i].modified == 1)
      {
        //set current in ram
        int decimal = stoi(cache[i].address.substr(0,4)+ "0", 0, 16);
        decimal /= 16;
        RAM[decimal].data = cache[i].data;
        
        //load new cache
        int decimal_2 = stoi(set_address, 0, 16);
        decimal_2 /= 16;
        cache[i].data = RAM[decimal_2].data;

        //set new to cache
        cache[i].valid = 1;
        cache[i].modified = 1;
        
        //set the cache
        cache[i].data[c_index] = data;
        cache[i].tag = tag;
        cache[i].address = set_address;

        //set the Ram with new, for inital time
        RAM[decimal_2].data = cache[i].data;
      }
      //load other data
      else if(cache[i].modified == 0)
      {
        //load new cache
        int decimal_2 = stoi(set_address, 0, 16);
        decimal_2 /= 16;
        cache[i].data = RAM[decimal_2].data;

      }
      //set the cache
      cache[i].data[c_index] = data;
      cache[i].tag = tag;
      cache[i].address = set_address;
    }
  } 
  return hit;
}

/* read into the cache */
read_data read(string address, char index, string tag, string offset,ram_data *RAM)
{
  string set_address = address.substr(0, 4) + "0";
  int decimal_2 = stoi(set_address, 0, 16);
  decimal_2 /= 16;
 
  int offset_int = atoi(offset.c_str());
  int c_index = offset_int / 4;
  read_data ret_data; 

  for(int i = 0; i < 16; i++ )
  {
    if(cache[i].index == index)
    {
      //read from cache
      if(cache[i].tag == tag)
      {
        ret_data.data = cache[i].data[c_index];
        ret_data.hit = 1;
        return ret_data;
      }
      //set to ram if m = 1, set cache 
      else if(cache[i].modified == 1)
      {
        //move cache to ram
          int decimal = stoi(cache[i].address.substr(0,4)+ "0", 0, 16);
          decimal /= 16;
          RAM[decimal].data = cache[i].data;
          
          //load new cache
          int decimal_2 = stoi(set_address, 0, 16);
          decimal_2 /= 16;
          cache[i].data = RAM[decimal_2].data;

          //set new to cache
          cache[i].valid = 1;
          cache[i].modified = 0;
          
          //set the cache
          cache[i].tag = tag;
          cache[i].address = set_address;

      }
      //set cache
      else if(cache[i].modified == 0)
      {
         //load new cache
          int decimal_2 = stoi(set_address, 0, 16);
          decimal_2 /= 16;
          cache[i].data = RAM[decimal_2].data;

          //set new to cache
          cache[i].valid = 1;
          cache[i].modified = 0;
          
          //set the cache
          cache[i].tag = tag;
          cache[i].address = set_address;

      }
      ret_data.data = cache[i].data[c_index];//RAM[decimal_2].data[c_index];
    }
  }
   ret_data.hit = 0;
  
  return ret_data;
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
    for(auto duo: line.data)
    {
      cout << duo << " ";
    }
    cout << endl;

    //cout << line.data << endl;
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

  //initalize cache
//  vector<cache_line> cache;
  //initalize ram
  ram_data RAM[1048576/16];



  //get input from user
  cin.getline(input_line, 128);

  //chage input to vector
  vector<string> commands = array_make(input_line);

  //filename
  string filename;

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
        string new_data;

        //new data to write in cache
        if( line_vect[1] == "W")
        {
          new_data = line_vect[2] + " " +line_vect[3]+ " " + line_vect[4] + " "
            + line_vect[5];
        }
       
        // display file
        string address = line_vect[0];
        string r_w = line_vect[1];
        char c_index = line_vect[0].at(3);
        string tag = line_vect[0].substr(0,3);
        string offset = line_vect[0].substr(4); 
        int hit;

        //read/write function calls
        if (r_w == "R")
        {
         read_data read_ret = read(address, c_index, tag, offset, RAM);
         new_data = read_ret.data;
         hit = read_ret.hit;

        }
        else
        {
          hit = write(address, c_index, tag, offset, new_data, RAM);
        }

        char hit_out = 'M'; 
        if(hit == 1)
        {
          hit_out = 'H';
        }
        cout << address << " ";
        cout << r_w << " ";
        cout << c_index << " ";
        cout << tag << " ";
        cout << offset << " ";
        cout << hit_out << " ";
        cout << new_data <<  endl;;
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
        string new_data;

        //new data to write in cache
        if( line_vect[1] == "W")
        {
          new_data = line_vect[2] + " " +line_vect[3]+ " " + line_vect[4] + " "
            + line_vect[5];
        }
       
        // display file
        string address = line_vect[0];
        string r_w = line_vect[1];
        char c_index = line_vect[0].at(3);
        string tag = line_vect[0].substr(0,3);
        string offset = line_vect[0].substr(4); 
        int hit;

        //read/write function calls
        if (r_w == "R")
        {
         read_data read_ret = read(address, c_index, tag, offset, RAM);
         new_data = read_ret.data;
         hit = read_ret.hit;
        }
        else
        {
          hit = write(address, c_index, tag, offset, new_data, RAM);
        }

        char hit_out = 'M'; 
        if(hit == 1)
        {
          hit_out = 'H';
        }
        cout << address << " ";
        cout << r_w << " ";
        cout << c_index << " ";
        cout << tag << " ";
        cout << offset << " ";
        cout << hit_out << " ";
        cout << new_data <<  endl;;
 
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
