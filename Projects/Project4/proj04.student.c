//project 4
//author: Clare Kinery

#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include "string.h"
#include <iostream>
#include <cmath>
#include <stdlib.h>
#include <unistd.h>
#include <vector>
#include <time.h>

using namespace std;

/** Intakes a string splits by space or tab
* returns a vector of strings 
*/
vector<string> array_make(string line)
{
  vector<string> splitString;
  string word = "";

  //check see if tab or space
  string check = "";
  for (int i=0; i<line.length(); i++)
  {
    check.push_back(line[i]);
    if(check == " " || check == "/t")
    {
      splitString.push_back(word);
      //reset word
      word = "";
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

/** Replicates a tsch. Allows for user to input
* defined commands and outputs correctly based on
* those commands.
 */ 
int main(int argc, char* argv[], char* envp[])
{
  //count for input line display
  int command_num = 1;

  while(1)
  {
    char input_line[128];
    string user_input;
    string user = getlogin();

    //prompt display
    cout <<'[' << command_num << " " <<  getlogin() << ']';
    memset(input_line,0,128);
    cin.getline(input_line, 128);
    
    //vector of strings to index user input
    vector<string> command = array_make(input_line);
    
    //terminate program
    if (command[0] == "halt")
    {
      exit(0);
    }

    //display command options
    else if(command[0] == "help")
    {
      cout << "halt:   terminate the shell process" << endl;
      cout << "help:   display built-in commands" << endl;
      cout << "date:   display current date and time" << endl;
      cout << "env:    display environment variables" << endl;
      cout << "path:   display current search path" << endl;
      cout << "cwd:    display absolute pathname of current working directory" <<
        endl;
      cout << "cd manage current working directory" << endl;
    }

    //display time/date
    else if(command[0] == "date")
    {
      time_t dtime = time(NULL);
      cout << ctime(&dtime) << endl;
    }

    //display enviorment
    else if(command[0] == "env")
    {
      for(char **env_ptr = envp; *env_ptr != 0; env_ptr++)
      {
        char* tempText = *env_ptr;
        cout << tempText << endl;
      } 
    }

    //display path
    else if(command[0] == "path")
    {
      string path = getenv("PATH");
      
      //display path nicely
      string word= "";
      for(int i=0; i < path.length(); i++)
      {
        if(path[i] == ':')
        {
          cout << word << endl;
          word = "";
        }
        word += path[i]; 
      }
    }
    
    //display current working directory
    else if(command[0] == "cwd")
    {
      string cwd_str = getcwd(input_line, sizeof(input_line));
      cout << cwd_str << endl;
    }
    else if (command[0] == "cd")
    {
      //check for too many directory inputs
      if (command.size() > 2)
      {
        cout << "invaild directory change, too many directory inputs" << endl;
      }
      else if(command.size() > 1)
      {
        //change directory
        if(command[1] == "~")
        {
          chdir(getenv("/user"));
        }
        //change to home of current user
       else if(command[1] == ("~" + user))
        {
          chdir(getenv("HOME"));
        }
        //change directory to path given
        else
        {
          //check if directory is present
          if (chdir(command[1].c_str()) != 0)
          {
            cout << "Invaild directory, can't be accessed" << endl;
          }
        }
      }

      //change to user's home directory
      else
      {
        chdir("/user");    
      }
    }

    //incorrect command was entered
    else
    {
      cout << "incorrect command" << endl;
    }
    //increase display count
    command_num++;
  }
}
