// Project 9 

//Author: Clare Kinery

#include <stdlib.h>
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

struct page_entry
{
  int V = 0;
  int R = 0;
  int M = 0;
  unsigned int frame_num = 0;
};

struct page_cnt
{
  int mem_refs = 0;
  int read_op = 0;
  int write_op = 0;
};

//page counts
page_cnt counts;

/* Display the page table */
void display_page_table(vector<page_entry> page_table)
{
  cout << endl;
  int i = 0;

  //header
  cout<< "I" << " " << "V" << " "<< "R" << " " << "M" <<" " << "Frame Number" << endl; 
  for(page_entry entry : page_table)
  {
    cout << i << " ";

    cout << entry.V << " ";

    cout << entry.R << " ";

    cout << entry.M << " ";

    printf("%04x", entry.frame_num);
    printf("\n");
    i++;
  }
  cout << endl;

}

void display_counts(page_cnt counts)
{
  cout << "Memory References: " << counts.mem_refs << endl;
  cout << "Read Operations: " << counts.read_op << endl;
  cout << "Write Operations: " << counts.write_op << endl;
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

void file_display(string filename, int debug_cnt, vector<page_entry> page_table)
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
    if(debug_cnt == 1)
    {
      display_page_table(page_table);
    }
    //run through file
    while((ret_in = getline(&line, &len, file)) != -1)
    {
      vector<string> line_vect = array_make(line);
      counts.mem_refs+=1;
      if(line_vect[1] == "R")
      {
        counts.read_op++;
      }
      else
      {
        counts.write_op++;
      }
      
      unsigned int page_num;
      unsigned int page_off;
      //convert string to hex number
      unsigned int hex_num = strtol(line_vect[0].c_str(), nullptr, 16);
      printf("%04x", hex_num);
      printf(" ");

      //read/write
      cout << (line_vect[1]);
      printf(" ");

      //get page number
      page_num = hex_num >> 11;
      
      //get page offset
      page_off = (hex_num << 21) >> 21;

      printf("%01x", page_num);
      printf(" ");

      printf("%03x", page_off);
      printf("\n");

      if(debug_cnt == 1)
      {
        display_page_table(page_table);
      }
    }
  }
}

int main(int argc, char* argv[], char* envp[])
{
  int debug_cnt = 0;
  int vaild_refs = 0;
  string filename;

  //create page table
  vector<page_entry> page_table;
  for(int i=1; i<=8; i++)
  {
    page_entry entry;
    page_table.push_back(entry);
  }

  //run commands
  for(int i = 1; i < argc; i++)
  {
    // command variable
    string command = argv[i];

    //option -refs
    if(command == "-refs")
    {
      //check for file
      if(i + 1 < argc)
      {
        //check not a command, file is correct
        if(argv[i+1] != "-refs" && argv[i+1] != "-debug")
        {
          filename = argv[i+1];
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
    file_display(filename, 1, page_table);
    display_page_table(page_table);
    display_counts(counts);
  }
  //only refs option set
  else if(vaild_refs == 1)
  {
    file_display(filename, 0, page_table);
    display_page_table(page_table);
    display_counts(counts);
  } 
  //no vaild input    
  else
  {
    cout << "not a valid command" << endl;
  }
}
