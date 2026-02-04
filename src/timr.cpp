#include <time.h>
#include <timr.h>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <ctime>
#include <vector>

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
    string line;
    bool found = false;
    ifstream rf(cfile);
    ofstream wf("temp.csv");
    string stringToFind = to_string(year) + ',' + to_string(month) + ',' + to_string(day) + ',' + to_string(weekday) + ','+ ctype;
    size_t len = stringToFind.length();
    size_t inc = 0;
    while (getline(rf, line))
    {
        while (true)
        {
            size_t pos = line.find(stringToFind);
            if (pos != string::npos){
                while(line[pos+len+1+inc] != ','){
                    inc++;
                }
                line.replace(pos+len+1, inc, to_string(stoi(line.substr(pos+len+1,inc)) + t_in_min));
                pos = pos + len + inc + 2;
                inc = 0;
                while(line[pos+inc] >= '0' && line[pos+inc] <='9'){
                    inc++;
                }
                line.replace(pos, inc, to_string(stoi(line.substr(pos,inc)) + 1));
                found = true;
                break;
            }else{
                break;
            }
        }

        wf << line << '\n';
    }
    rf.close();
    wf.close();
    remove(&cfile[0]);

    if (found){
        rename("temp.csv",&cfile[0]);
    }
    else{
        ofstream wf(cfile);
        ifstream rf("temp.csv");
        wf << "Year,Month,Day,Weekday,Type,Worktime,NumSesh\n";
        wf << to_string(year) + "," + to_string(month) + "," + to_string(day) + "," + to_string(weekday) + "," + ctype + "," + to_string(t_in_min) + "," + "1\n";
        getline(rf,line);
        wf << rf.rdbuf();
        rf.close();
        wf.close();
        std::remove("temp.csv");
    }

    return 0;
}
