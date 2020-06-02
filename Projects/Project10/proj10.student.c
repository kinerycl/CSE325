// Project 10 

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
  int page_fault = 0;
  int write_back = 0;
};

//page counts
page_cnt counts;

vector<page_entry> page_table;
/* replacement */
vector<unsigned int> Update_Queue(vector<unsigned int> free_frames, unsigned int
    frame, string replace)
{
  if (replace == "CLOCK")
  {
    while(free_frames[0] != frame)
    {
      free_frames.push_back(free_frames[0]);
      free_frames.erase(free_frames.begin());
    }
    free_frames.push_back(free_frames[0]);
    free_frames.erase(free_frames.begin());
  }
  else
  {
    free_frames.push_back(free_frames[0]);
    free_frames.erase(free_frames.begin());
  }
  return free_frames;
  
}

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

/** Displays the counts */
void display_counts(page_cnt counts)
{
  cout << "Memory References: " << counts.mem_refs << endl;
  cout << "Read Operations: " << counts.read_op << endl;
  cout << "Write Operations: " << counts.write_op << endl;
  cout << "Page Faults: " << counts.page_fault << endl;
  cout << "Write Backs: " << counts.write_back << endl;
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

void file_display(string filename, int debug_cnt, vector<page_entry> page_table,
    vector<unsigned int> free_frames, string replace)
{
  //check
  //FIFO(free_frames);
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
    //run through file display and process
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
      int victim = 0;
      unsigned int frame_num = free_frames[0];
      unsigned int page_num;
      unsigned int page_off;
      unsigned int physical_add;
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
      //physical address = framenum + offset
      physical_add = (frame_num << 11) | page_off;

     
      //check if vaild
      if(page_table[page_num].V == 1)
      {
        //no w/b or page fault
        cout << "   ";
        page_table[page_num].R = 1;
        if(line_vect[1] == "W")
        {
          page_table[page_num].M = 1; 
        }
        //update queue
        if(replace == "CLOCK")
        {
          free_frames = Update_Queue(free_frames, page_table[page_num].frame_num, replace);
        }
      }
      else
      {
       //page fault processing 
        counts.page_fault++;
        cout << " " << "F" << " ";
        //find replacement
        for(int i = 0; i < page_table.size(); i++)
        {
          //finding replacement if needed
          if(page_table[i].V == 1 && page_table[i].frame_num == free_frames[0])
          {
//            cout << "victim: " << i << " "<< page_table[i].frame_num << endl;
           //set victim
            victim =1; 
            page_table[i].V = 0;
            //write back
            if (page_table[i].M == 1)
            {
              counts.write_back++;
              printf("B");
              printf(" ");
            }
            else
            {
              cout << "  ";
            }
          }
        }
        //use free frame 
        page_table[page_num].V = 1; 
        page_table[page_num].R = 0;
        page_table[page_num].frame_num = free_frames[0];

        //set the table
        page_table[page_num].R = 1;
        if(line_vect[1] == "W")
        {
          page_table[page_num].M = 1;
        }
        free_frames = Update_Queue(free_frames, frame_num, replace);

      }  
      if(victim == 0)
      {
        cout << "  ";
      }
      printf("%06x", physical_add);
      
      printf("\n");
      if(debug_cnt == 1)
      {
        display_page_table(page_table);
      }
    }
  }
  display_page_table(page_table);
}

int main(int argc, char* argv[], char* envp[])
{
  int debug_cnt = 0;
  int vaild_refs = 0;
  string filename;

  //open and save config contents
  string replace;
  int frame_num;
  vector<unsigned int> frames;

  FILE * file;
  char * line = NULL;
  size_t len =0;
  ssize_t ret_in;

  file = fopen("config", "r");
  //run through file
  int cnt = 0;
  while((ret_in = getline(&line, &len, file)) != -1)
  {
    cnt++;
    vector<string> line_vect = array_make(line);
    //loop through array
    if(cnt == 1)//replacement
    {
      replace = line_vect[0];
    }
    else if (cnt == 2)//frame number
    {
      frame_num = atoi(line_vect[0].c_str());
    }
    else
    {
      //loop through vector and convert strings to ints
      for(string num: line_vect)
      {
        //convert string to hex number
        unsigned int hex_num = strtol(num.c_str(), nullptr, 16);
        frames.push_back(hex_num); 
      }
    }
  }

  //free frames
  vector<unsigned int> free_frames = frames;
 
  //create page table
  //vector<page_entry> page_table;
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
    file_display(filename, 1, page_table, free_frames, replace);
    //display_page_table(page_table);
    //display stimulation
    cout << "Stimulation Parameters: "<<
     replace << " " << frame_num << " ";
   for(auto hex_num : frames)
   {
     printf("%04x", hex_num); 
     printf(" ");
   }
    printf("\n"); 
    display_counts(counts);
  }
  //only refs option set
  else if(vaild_refs == 1)
  {
    file_display(filename, 0, page_table, free_frames, replace);
    //display_page_table(page_table);
    //display stimulation
    cout << "Stimulation Parameters: "<<
     replace << " " << frame_num << " ";
   for(auto hex_num : frames)
   {
     printf("%04x", hex_num); 
     printf(" ");
   }
    printf("\n"); 
 
    display_counts(counts);
  } 
  //no vaild input    
  else
  {
    cout << "not a valid command" << endl;
  }
}
