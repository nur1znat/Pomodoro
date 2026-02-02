#include <time.h>
#include <timr.h>

int showTime(const unsigned int t_in_min){
    unsigned int mins = 0;
    unsigned int sec = 0;

    unsigned int s_w_time = t_in_min * 60;

    while(s_w_time > 0){
        mins = s_w_time / 60;
        sec = s_w_time % 60;
        printf("\r%2d:%2d", mins, sec);
        fflush(stdout);
        clock_t stop = clock() + CLOCKS_PER_SEC;
        while (clock() < stop) {}

        s_w_time--;

    }
        printf("\rTime is up!\n");

	return 0;
}
