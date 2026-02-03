#include <iostream>
#include <time.h>
#include <string.h>
using namespace std;

int showTime(const unsigned int t_in_min, const unsigned int type); // type:1 ->work, 2->short brk, 3->long brk

class Record{
    private:
        string cfile;
        int year;
        int month;
        int day;
        int weekday;
        string ctype;

    public:
        Record();
        Record(string file, string type = "General");
        ~Record() {}
        void setFile(string file){cfile = file;}
        void setType(string type){ctype = type;}
        void prnt(){ cout << year << " " << month << " " << day << " " << weekday << endl;}
        int createFile(const string file);
        int addTime(const unsigned int t_in_min);
};
