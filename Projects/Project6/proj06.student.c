//project 6

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
#include <time.h>
#include <pthread.h>
#include <math.h>
#include <ctype.h>
#include <iostream>
#include <iomanip>
#include <semaphore.h>
using namespace std;
#define c_buffer 20
int producers = 1;
int thread_cnt = 0;
sem_t thread_cnt_sem;
sem_t consume;
sem_t prod;
sem_t crit;
//default producer size
int p = 1;

// bank vector
vector<vector<string> > bank_data; 
 
//default bounder buffer size
int b_buff = 5;

/** buffer structure **/
struct buffer_t
{
  string account;
  string action;
  string amount;
  string thread_num;
  bool flag;
};

/* defines the buffer */
typedef struct {
  buffer_t buffer_arr[c_buffer];
  int b_head = 0;
  int b_tail = 0;
} buffer_global;

//global buffer
buffer_global BUFF;

/* Intakes a string and splits removes the first
* letter */
string user_str(string user)
{
  string ret_str = "";
  for (unsigned int i=1; i<user.length(); i++)
  {
    ret_str += user[i];
  }
  return ret_str;
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
  return (splitString);
}

/* producer function */
void * producer(void *arg)
{
  long long thrNum = (long long)(arg);
  char filename[8];
  memset(filename, 0, 8);
  sprintf(filename, "trans%i", int(thrNum));

  //open file and read
  FILE * file;
  char * line = NULL;
  size_t len = 0;
  ssize_t ret_in;

  file = fopen(filename, "r");
  if(file == NULL)
  {
    cout << "file not found" << endl;
  }
  else
  {

    while((ret_in = getline(&line, &len, file)) != -1)
    {
      vector<string> line_data = array_make(line); 
      buffer_t data;
      data.account = line_data[0];
      data.action = line_data[1];
      data.amount = line_data[2];
      data.thread_num = to_string(thrNum);

      //sem producer and critical state
      sem_wait(&prod); 
      sem_wait(&crit);
      BUFF.buffer_arr[BUFF.b_tail] = data;
    
      BUFF.b_tail++;
      if (BUFF.b_tail > b_buff)
      {
        BUFF.b_tail = 0;
      } 

      sem_post(&crit);
      sem_post(&consume);
    }
  }
  buffer_t end_data;
  end_data.flag = 1;
  
  //setting the flag
  sem_wait(&prod);
  sem_wait(&crit);

  BUFF.buffer_arr[BUFF.b_tail] = end_data;
  BUFF.b_tail++;
  if(BUFF.b_tail > b_buff)
  {
    BUFF.b_tail = 0;
  }

  sem_post(&crit);
  sem_post(&consume);

  //increase thread count
  sem_wait(&thread_cnt_sem);
  thread_cnt++;
  sem_post(&thread_cnt_sem);
  pthread_exit(NULL);
}

/* consumer function */
void * consumer(void * arg)
{
  ofstream ofile("accounts.log");

 bool stop = 0;
 //until flag found
 while(stop!= 1)
 {
   sem_wait(&consume);
   sem_wait(&crit);

   buffer_t t = BUFF.buffer_arr[BUFF.b_head];
   BUFF.b_head++;
   if(BUFF.b_head > b_buff)
   {
     BUFF.b_head = 0;
   }
   if(thread_cnt == p && BUFF.b_head == BUFF.b_tail && t.flag == 1)
   {
     sem_wait(&thread_cnt_sem);
     if (thread_cnt == p && BUFF.b_head == BUFF.b_tail)
     {
       stop = 1;
     }
     sem_post(&thread_cnt_sem);
   }
   else
   {
     string prev_balance;
     int index = 0;
     int cnt = 0;
     int set = 0;

     //find data in vector
     for (vector<string> account : bank_data)
     {
       if (account[0] == t.account)
       {
         prev_balance = account[1];
         index = cnt;
         set = 1;
       }
       cnt++;
     }
     double new_balance; 
     if (t.action == "withdraw")
     {
      if(atof(prev_balance.c_str()) > atof(t.amount.c_str()))
      {
        new_balance = atof(prev_balance.c_str()) - atof(t.amount.c_str());
      }
      else
      {
        new_balance = atof(prev_balance.c_str());
      }
     }
     else
     {
       new_balance = atof(prev_balance.c_str()) +  atof(t.amount.c_str());
     }
     if (set == 1) //acount exists
     {
       bank_data[index][1] = to_string(new_balance);
       //output info to file
       ofile <<  t.thread_num << " ";
       ofile <<  t.account << " ";
       ofile << t.action << " ";
       ofile <<   t.amount << " "; 
       //previous balance
       ofile << setprecision(2) << prev_balance << " ";
       //update balance
       ofile << to_string(new_balance);
       ofile << endl;
       ofile << endl;
     }
     else if (t.flag == 1)//ignore flag data
     {

     }
     else//accounts that don't exist
     {
       //output info to file
       ofile <<  t.thread_num << " ";
       ofile <<  t.account << " ";
       ofile << t.action << " ";
       ofile <<   t.amount << " "; 
       //previous balance
       ofile << setprecision(2) << prev_balance << " ";
       //update balance
       ofile << to_string(new_balance);
       ofile << endl;
       ofile << endl;
     }

   sem_post(&crit);
   sem_post(&prod);
   }
 }
 pthread_exit(NULL);
}

/* main fuction */
int main(int argc, char* argv[], char* envp[])
{
  char input_line[128]; //is this correct the buff sz??
    
  //get input from user
  cin.getline(input_line, 128);

  //change input to vector 
  vector<string> commands = array_make(input_line);

  //set p and b
  for (unsigned int i = 0; i < commands.size(); i++)
  {
    string command = commands[i];
    if(command == "-p")
    {
      unsigned int is_dig = 1;

      //stay in range of vector
      if (i + 1 < commands.size())
      {
        //check if digit
        for (char i : commands[i+1])
        {
          if( isdigit(i) == false)
          {
            is_dig = 0;
          }
        }
        if (is_dig == 1)
        {
          //check not exceed limit
          int digit = atoi(commands[i+1].c_str());
          if (digit > 0 &&  digit <= 10)
          {
           p = atoi(commands[i+1].c_str());
          }
        }
      }
     }
     else if (command == "-b")
     {
       unsigned int b_dig = 1;
        
       //stay in range of vector
       if (i +1 < commands.size())
       {
        //check if digit
        for (char j : commands[i+1])
        {
          if (isdigit(j) == false)
          {
            b_dig = 0;
          }
        }
        if (b_dig == 1)
        {
          //check not exceed limit
          int b_digit = atoi(commands[i+1].c_str());
          if (b_digit > 0 && b_digit <= 20)
          {
            b_buff = b_digit;
          }
        }
     }
    }
  }


  //read in lines and save in bank structure
  FILE * file;
  char * line = NULL;
  size_t len = 0;
  ssize_t ret_in;

  file = fopen("accounts.old", "r");
  while((ret_in = getline(&line, &len, file)) != -1)
  {
    vector<string> data = array_make(line);
    bank_data.push_back(data);
  }

  // create semaphore
  buffer_t buffer[b_buff];
  sem_init(&consume, 0, 0); 
  sem_init(&crit,0 ,1);
  sem_init(&prod,0 ,b_buff);
  sem_init(&thread_cnt_sem, 0, 1);


  //create threads for producer
  pthread_t threads[p]; 
  for(long thr = 0; thr<p; thr++)
  {
    pthread_create(&threads[thr], NULL, producer, (void *) thr);
  }


  //consumer thread
  pthread_t c_thread;
  pthread_create(&c_thread, NULL, consumer, NULL);

  //join threads
  for(long thr =0; thr<p; thr++)
  {
    pthread_join(threads[thr],NULL);
  }

  pthread_join(c_thread, NULL);

  //write to accounts.new
  ofstream nfile("accounts.new");

  for(vector<string>account : bank_data)
  {
    nfile << account[0] << " " << account[1] << endl;
  }
}
