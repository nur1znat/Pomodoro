#include <iostream>
#include <string>
using namespace std;

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
        Record(string type);
        ~Record() {}
        void setFile(string file){cfile = file;}
        void setType(string type){ctype = type;}
        void prnt(){ cout << year << " " << month << " " << day << " " << weekday << endl;}
        int createFile(const string file);
        int addTime(const unsigned int t_in_min, const unsigned int incr = 0);
        int showTime(const unsigned int t_in_min, const unsigned int type=1, const bool svRcrd = 0); // type:1 ->work, 2->short brk, 3->long brk
};
