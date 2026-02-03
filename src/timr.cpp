#include <time.h>
#include <timr.h>
#include <string.h>
#include <fstream>
#include <ctime>

int showTime(const unsigned int t_in_min, const unsigned int type){
    unsigned int mins = 0;
    unsigned int sec = 0;
    unsigned int s_w_time = t_in_min * 60;

    if(type == 2){ cout << "Take a short break...\n";
    }else if(type == 3){ cout << "Take a long break...\n";
    }else{ cout << "\rWork..\n";}

    while(s_w_time > 0){
        mins = s_w_time / 60;
        sec = s_w_time % 60;
        printf("\r%02d:%02d", mins, sec);
        fflush(stdout);
        clock_t stop = clock() + CLOCKS_PER_SEC;
        while (clock() < stop) {}

        s_w_time--;

    }
        printf("\rTime is up!\n");

	return 0;
}
Record::Record(){
    ctype = "General";
    time_t now = time(0);
    tm *ltm = localtime(&now);

    year = 1900 + ltm->tm_year;
    month = 1 + ltm->tm_mon;
    day = 0 + ltm->tm_mday;
    weekday = 0 + ltm->tm_wday;
}

Record::Record(string type){
    ctype = type;
    time_t now = time(0);
    tm *ltm = localtime(&now);

    year = 1900 + ltm->tm_year;
    month = 1 + ltm->tm_mon;
    day = 0 + ltm->tm_mday;
    weekday = 0 + ltm->tm_wday;
}

int Record::createFile(const string file){
    cfile = file;
    ofstream f;
    f.open(file);

    // if(!f.is_open()){ cout << "Error creating a file\n"; return 1;}
    f << "Year,Month,Day,Weekday,Type,Worktime,NumSesh\n";
    f.close();
    return 0;
}

int Record::addTime(const unsigned int t_in_min){
    ofstream f;
    f.open(cfile, ios_base::app);

    // if(!f.is_open()){ cout << "Error opening the file\n"; return 1;}
    f << to_string(year) + "," + to_string(month) + "," + to_string(day) + "," + to_string(weekday) + "," + ctype + "," + to_string(t_in_min) + "," + "1\n";

    f.close();

    return 0;
}
