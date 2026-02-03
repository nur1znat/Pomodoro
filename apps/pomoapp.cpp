#include <boost/program_options.hpp>
#include "timr.h"
#include <string.h>
#include <filesystem>

using namespace std;
using namespace boost::program_options;

int main(int argc, char* argv[])
{
	unsigned int w_time = 25; // bari minutpen Work time
	unsigned int sb_time = 5; // Short break
	unsigned int lb_time = 15; // Long break
	unsigned int nw= 3; // number of Sessions before the long brk
	string task = "General"; // number of Sessions before the long brk
	unsigned int verbosity = 1; // Mnani koya saldim brak paidasi katti zhok. Krch run zhasaganda uakittardin summary-in beredi
    bool saveRecord = 1;

    // 37-shi line-ga dein bari argument-ke option
	try
	{
		options_description desc("Pomodoro options");
		desc.add_options()
			("help", "Usage:")
			("wt", value(&w_time), "Work Time in min (default = 25), e.g., --wt 30")
			("sb", value(&sb_time), "Short Break Time in min (default = 5), e.g., --sb 10")
			("nw", value(&nw), "# of Work Session before Long break (default = 3), e.g., --nw 4")
			("lb", value(&lb_time), "Long Break Time in min (default = 15), e.g., --lb 20")
			("sv", value(&saveRecord), "Save the Records for Stats (default = 1), e.g., --sv 0")
			("ts", value(&task), "Task or Type name (default = \"General\"), e.g., --ts Math")
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

    Record rec(task);
    if(saveRecord){
        string path = filesystem::current_path();
        string file;

        for (const auto & entry : filesystem::directory_iterator(path))
            if (entry.path().extension() == ".csv"){
                file = entry.path();
                rec.setFile(file);
            }

        if(file.empty()){
            cout << "No file found on record. Please give a name for a new file...";
            cin >> file;
            rec.createFile(path + "/" + file + ".csv");
        }
    }

    unsigned int temp_nw = nw;
    string str;
    while(true){
        // line 53-59, 1 loop of work sessions >1 dedim cuz if you don't want to continue, long break kerek emes
        if (temp_nw > 1){
            showTime(w_time,1);

            if(saveRecord)
                rec.addTime(w_time);

            temp_nw--;
            showTime(sb_time,2);
        }
        showTime(w_time,1);
        if(saveRecord)
            rec.addTime(w_time);

        // 1 loop bitkesin continue ma zhok pa
        cout << "Do you want to continue? (Y/N)";
        cin >> str;
        if(str != "Y" && str != "y"){
            break;
        }
        showTime(lb_time,3);
    }



	return EXIT_SUCCESS;


}
