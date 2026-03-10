#include <timr.h>
#include <fstream>
#include <thread>
#include <chrono>
using namespace std::chrono_literals;

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

int Record::showTime(const unsigned int t_in_min, const unsigned int type, const bool svRcrd){
    unsigned int mins = 0;
    unsigned int sec = 0;
    unsigned int s_w_time = t_in_min * 60;
    bool timerRunning;
    if(type == 2){ cout << "Take a short break...\n";
    }else if(type == 3){ cout << "Take a long break...\n";
    }else{ cout << "\rWork..\n";}
    const int write_incr = 60;
    int next_write = s_w_time - write_incr;
    bool frst = true;

    while(s_w_time > 0){
        mins = s_w_time / 60;
        sec = s_w_time % 60;
        timerRunning = true;
        printf("\r%02d:%02d", mins, sec);
        fflush(stdout);
        auto start = chrono::steady_clock::now();
        this_thread::sleep_for(900ms);
        while(timerRunning){
            auto now = chrono::steady_clock::now();
            if (chrono::duration_cast<chrono::milliseconds>(now-start) >= 1000ms) {
                timerRunning = false;
                s_w_time--;
            }
            this_thread::sleep_for(2ms);
        }

        if(type == 1 && svRcrd && s_w_time <= next_write){
            next_write = next_write - write_incr;
            if(next_write > 0){
                if (frst){
                    this->addTime(write_incr/60, 1);
                    frst = false;
                }else {
                    this->addTime(write_incr/60);
                }
            }else{
                this->addTime(1);
            }
        }
    }

    // if(type == 1 && svRcrd){
    //     this->addTime((write_incr - next_write)/60);
    // }

    printf("\rTime is up!\n");

	return 0;
}

int Record::addTime(const unsigned int t_in_min, const unsigned int incr){
    if(t_in_min == 0){
        return 0;
    }

    string line;
    bool found = false;
    ifstream rf(cfile);
    ofstream wf("temp.csv");
    string stringToFind = to_string(year) + ',' + to_string(month) + ',' + to_string(day) + ',' + to_string(weekday) + ','+ ctype;
    size_t len = stringToFind.length();
    size_t inc = 0;
    int bef, aft;
    while (getline(rf, line))
    {
        while (true)
        {
            size_t pos = line.find(stringToFind);
            if (pos != string::npos){
                while(line[pos+len+1+inc] != ','){
                    inc++;
                }
                bef = stoi(line.substr(pos+len+1,inc));
                aft = bef + t_in_min;
                line.replace(pos+len+1, inc, to_string(aft));

                if((bef < 10 && aft >= 10) || (bef < 100 && aft >= 100) || (bef < 1000 && aft >= 1000)){
                    pos++;
                }

                pos = pos + len + inc + 2;

                inc = 0;
                while(line[pos+inc] >= '0' && line[pos+inc] <='9'){
                    inc++;
                }
                line.replace(pos, inc, to_string(stoi(line.substr(pos,inc)) + incr));
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
