#include <boost/program_options.hpp>
#include "timr.h"
#include <string.h>


using namespace std;
using namespace boost::program_options;

int main(int argc, char* argv[])
{
	unsigned int w_time = 25;
	unsigned int sb_time = 5;
	unsigned int lb_time = 15;
	unsigned int nw= 3;
	unsigned int verbosity = 1;

	try
	{
		options_description desc("Pomodoro options");
		desc.add_options()
			("help", "Usage:")
			("wt", value(&w_time), "Work Time in min (default = 25), e.g., --wt 30")
			("sb", value(&sb_time), "Short Break Time in min (default = 5), e.g., --sb 10")
			("nw", value(&nw), "# of Work Session before Long break (default = 3), e.g., --nw 4")
			("lb", value(&lb_time), "Long Break Time in min (default = 15), e.g., --lb 20")
			("verbosity", value(&verbosity), "Verbosity level (0 silent, 1 verbose)");
			variables_map vm;
		store(parse_command_line(argc, argv, desc, command_line_style::unix_style ^ command_line_style::allow_short), vm);
		notify(vm);
		if (vm.count("help")) {
			cout << desc << "\n";
			return 0;
		}
	}
	catch (exception& e) {
		cerr << e.what() << "\n";
	}

	if (verbosity)
	{
		cout << "- Pomodoro parameters -\n";
		cout << "Work Time: " << w_time << endl;
		cout << "Short Break Time: " << sb_time << endl;
		cout << "Number of Work Sessions: " << nw << endl;
		cout << "Long Break Time: " << lb_time << endl;
	}
    cout << "------ Let it rip ------\n";

    unsigned int temp_nw = nw;
    string str;
    while(true){
        if (temp_nw > 1){
            showTime(w_time);
            temp_nw--;
            showTime(sb_time);
        }
        showTime(w_time);

        cout << "Do you want to continue? (Y/N)";
        cin >> str;
        if(str != "Y" && str != "y"){
            break;
        }
        showTime(lb_time);
    }



	return EXIT_SUCCESS;


}
