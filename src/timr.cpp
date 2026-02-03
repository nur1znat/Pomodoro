#include <time.h>
#include <timr.h>
#include <string.h>

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
