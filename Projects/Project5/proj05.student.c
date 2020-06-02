//project 5 
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
#include <string>
#include <time.h>

using namespace std;

void halt()
{
  exit(0);
}

void help()
{
  cout << "halt:   terminate the shell process" << endl;
  cout << "help:   display built-in commands" << endl;
  cout << "date:   display current date and time" << endl;
  cout << "env:    display environment variables" << endl;
  cout << "path:   display current search path" << endl;
  cout << "cwd:    display absolute pathname of current working directory" <<
    endl;
  cout << "cd:     manage current working directory" << endl;
  cout << "set:    manage enviornment variables" << endl;
  cout << "import: read commands from file" << endl;  
}

void date()
{
  time_t dtime = time(NULL);
  cout << ctime(&dtime) << endl;
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

/* Intakes a string and splits removes the first
* letter */
string user_str(string user)
{
  string ret_str = "";
  for (int i=1; i<user.length(); i++)
  {
    ret_str += user[i];
  }
  return ret_str;
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
    int buff = 128;
    char input_line[buff];
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
      cout << "cd:     manage current working directory" << endl;
      cout << "set:    manage enviornment variables" << endl;
      cout << "import: read commands from file" << endl; 
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
      for(char **env_ptr = environ; *env_ptr != 0; env_ptr++)
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
        //check for ~ vs ~user
        string user_given = command[1].c_str();
        string user_name = (user_given).c_str();

        //change directory
        if(command[1] == "~")
        {
          chdir(getenv("/user"));
          setenv("PWD", "/user", true);
        }
        //change to home of current user
       else if(command[1] == ("~" + user_name))
        {
          string user_direct_str = "/user/" + user_name;
          const char * user_direct_char = user_direct_str.c_str();
          chdir(user_direct_char);
          setenv("PWD", user_direct_char, true);
        }
        //change directory to path given
        else
        {
          //check if directory is present
          if (chdir(command[1].c_str()) != 0)
          {
            cout << "Invaild directory, can't be accessed" << endl;
          }
          else
          {
            setenv("PWD", command[1].c_str(), true);
          }
        }
      }

      //change to user's home directory
      else
      {
        chdir("/user");    
        setenv("PWD", "/user", true);
      }
    }
    else if (command[0] == "set")
    {
      if (command.size() > 3)
      {
        cout << "too many inputs for set" << endl;
      }
      if (command.size() < 2)
      {
        cout << "too few inputs for set" << endl;
      }
      //set or overwrite variable
      if (command.size() == 3)
      {
        const char *VAR = command[1].c_str();
        const char *VAL = command[2].c_str();
        setenv(VAR, VAL, true);
      } 
      //remove variable 
      else
      {
        const char *VAR = command[1].c_str();
        unsetenv(VAR);
      }
    }
    //import files
    else if (command[0] == "import")
    {
      if (command.size() == 2)
      {
        FILE * file;
        char * line = NULL;
        size_t len = 0;
        ssize_t ret_in;
  
        file = fopen(command[1].c_str(), "r");
        //file does not exist
        if (file == NULL)
        {
          cout << "file doesn't exist" << endl;
        }
        //file exists
        else
        {
          //go through lines and print
           while((ret_in = getline(&line, &len, file)) != -1)
          {
            vector<string> mini_command =  array_make(line);
            if(mini_command[0].c_str() == "halt")
            {
              halt();
            }
          }
        }
      } 
      else
      {
        cout << "no file name" << endl;
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
