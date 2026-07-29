#include <boost/program_options.hpp>
#include "timr.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <random>
#include <cstdlib>
#include <cstring>

using namespace std;
using namespace boost::program_options;
namespace fs = std::filesystem;

// =====================================================================
// "pomodoro plot" support -- builds the dashboard HTML from a CSV log
// and opens it in the browser. Mirrors the standalone view_focus_log
// tool, but auto-picks the CSV from ~/.pomodoro the same way the rest
// of this app does.
// =====================================================================

static const string PLACEHOLDER = "\"__CSV_DATA_PLACEHOLDER__\"";

static string readFile(const fs::path& p) {
    ifstream f(p, ios::binary);
    if (!f) throw runtime_error("Could not open file: " + p.string());
    ostringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

static void writeFile(const fs::path& p, const string& content) {
    ofstream f(p, ios::binary);
    if (!f) throw runtime_error("Could not write file: " + p.string());
    f << content;
}

// Equivalent to Python's json.dumps(csv_text) -- produces a safely
// escaped, quoted JS string literal so the CSV content can't break
// out of the <script> block regardless of quotes/newlines inside it.
static string jsonEscape(const string& s) {
    ostringstream out;
    out << '"';
    for (unsigned char c : s) {
        switch (c) {
            case '"':  out << "\\\""; break;
            case '\\': out << "\\\\"; break;
            case '\n': out << "\\n"; break;
            case '\r': out << "\\r"; break;
            case '\t': out << "\\t"; break;
            case '\b': out << "\\b"; break;
            case '\f': out << "\\f"; break;
            default:
                if (c < 0x20) {
                    char buf[8];
                    snprintf(buf, sizeof(buf), "\\u%04x", c);
                    out << buf;
                } else {
                    out << static_cast<char>(c);
                }
        }
    }
    out << '"';
    return out.str();
}

static fs::path makeTempHtmlPath() {
    fs::path dir = fs::temp_directory_path();
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dist(100000, 999999);
    fs::path p;
    do {
        p = dir / ("focus_log_" + to_string(dist(gen)) + ".html");
    } while (fs::exists(p));
    return p;
}

static void openInBrowser(const fs::path& path) {
    string cmd = "xdg-open \"" + fs::absolute(path).string() + "\" >/dev/null 2>&1 &";
    system(cmd.c_str());
}

// Same "auto-pick, or prompt if more than one" behavior as the existing
// record-file logic further down in main().
static fs::path pickCsvFile(const fs::path& pomodoroDir) {
    vector<fs::path> files;
    for (const auto& entry : fs::directory_iterator(pomodoroDir))
        if (entry.path().extension() == ".csv")
            files.push_back(entry.path());

    if (files.empty()) {
        cerr << "No CSV records found in " << pomodoroDir.string() << ".\n";
        exit(1);
    }
    if (files.size() == 1) {
        return files[0];
    }

    cout << "Found more than 1 csv file. Please specify which one is yours!\n";
    for (size_t i = 0; i < files.size(); i++)
        cout << i + 1 << ": " << files[i].filename().string() << "\n";
    cout << "Your choice: ";
    int choice;
    cin >> choice;
    if (choice < 1 || static_cast<size_t>(choice) > files.size()) {
        cerr << "Invalid choice.\n";
        exit(1);
    }
    return files[choice - 1];
}

static int runPlotCommand(int argc, char* argv[]) {
    string csvArg, templateArg, outArg;
    bool noOpen = false;

    try {
        options_description desc("Pomodoro plot options");
        desc.add_options()
            ("help", "Usage:")
            ("csv", value(&csvArg), "CSV file to plot (default: auto-detect in ~/.pomodoro)")
            ("template", value(&templateArg), "Dashboard HTML template (default: ~/.pomodoro/pomodoro-dashboard.html)")
            ("out", value(&outArg), "Where to write the generated HTML (default: a temp file that opens automatically)")
            ("no-open", bool_switch(&noOpen), "Only generate the HTML file, don't open a browser");

        variables_map vm;
        store(parse_command_line(argc, argv, desc, command_line_style::unix_style ^ command_line_style::allow_short), vm);
        notify(vm);

        if (vm.count("help")) {
            cout << desc << "\n";
            return 0;
        }
    } catch (exception& e) {
        cerr << e.what() << "\n";
        return 1;
    }

    const char* home = getenv("HOME");
    if (!home) {
        cerr << "Could not determine HOME directory.\n";
        return 1;
    }
    fs::path pomodoroDir = fs::path(home) / ".pomodoro";

    fs::path csvPath      = csvArg.empty()      ? pickCsvFile(pomodoroDir)                        : fs::path(csvArg);
    fs::path templatePath = templateArg.empty() ? (pomodoroDir / "pomodoro-dashboard.html")        : fs::path(templateArg);
    fs::path outPath      = outArg.empty()       ? makeTempHtmlPath()                              : fs::path(outArg);

    if (!fs::is_regular_file(csvPath)) {
        cerr << "CSV file not found: " << csvPath.string() << "\n";
        return 1;
    }
    if (!fs::is_regular_file(templatePath)) {
        cerr << "Dashboard template not found: " << templatePath.string() << "\n"
             << "Place pomodoro-dashboard.html there, or pass --template PATH.\n";
        return 1;
    }

    string csvText      = readFile(csvPath);
    string templateHtml = readFile(templatePath);

    auto pos = templateHtml.find(PLACEHOLDER);
    if (pos == string::npos) {
        cerr << "Could not find the data placeholder in the template. "
                "Make sure you're using the matching pomodoro-dashboard.html.\n";
        return 1;
    }
    templateHtml.replace(pos, PLACEHOLDER.size(), jsonEscape(csvText));
    writeFile(outPath, templateHtml);

    cout << "Dashboard written to: " << fs::absolute(outPath).string() << "\n";
    if (!noOpen) openInBrowser(outPath);

    return 0;
}

// =====================================================================
// existing pomodoro timer app
// =====================================================================

int main(int argc, char* argv[])
{
    // "pomodoro plot [--csv PATH] [--template PATH] [--out PATH] [--no-open]"
    // Checked before the normal option parsing so it never collides with
    // --wt/--sb/etc, and so a bare "pomodoro plot" just works.
    if (argc > 1 && string(argv[1]) == "plot") {
        return runPlotCommand(argc - 1, argv + 1);
    }

	unsigned int w_time = 25; // bari minutpen Work time
	unsigned int sb_time = 5; // Short break
	unsigned int lb_time = 15; // Long break
	unsigned int nw= 3; // number of Sessions before the long brk
	string task = "General"; // number of Sessions before the long brk
	unsigned int verbosity = 1; // Mnani koya saldim brak paidasi katti zhok. Krch run zhasaganda uakittardin summary-in beredi
    bool saveRecord = 1;
    bool sound = 1;

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
			("vb", value(&verbosity), "Verbosity level (0 silent, 1 verbose)")
			("sd", value(&sound), "Sound (0 silent, 1 notify with Sound)");
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
		cout << "Task Name: " << task << endl;
		cout << "Work Time: " << w_time << endl;
		cout << "Short Break Time: " << sb_time << endl;
		cout << "Number of Work Sessions: " << nw << endl;
		cout << "Long Break Time: " << lb_time << endl;
		cout << "Sound: " << sound << endl;
	}
    cout << "------ Let it rip ------\n";

    Record rec(task);
    if(saveRecord){
        string path = filesystem::current_path();
        vector<string> files;
        string file;
        const char *hmpth = getenv("HOME");
        string pth(hmpth);
        pth = pth + "/.pomodoro";

        if(!filesystem::is_directory(pth))
            filesystem::create_directory(pth);

        for (const auto & entry : filesystem::directory_iterator(pth))
            if (entry.path().extension() == ".csv"){
                files.push_back(entry.path());
            }

        if(files.empty()){
            cout << "No file found on record. Please give a name for a new file...";
            cin >> file;
            cout << pth + "/" + file + ".csv" << endl;
            rec.createFile(pth + "/" + file + ".csv");
        }else if(files.size() > 1){
            int choice;
            cout << "Found more than 1 csv file. Please specify which one is yours!" << endl;
            for (int i = 0; i < files.size(); i++){
                cout << i + 1 << ": " << filesystem::path(files[i]).filename().string() << endl;
            }
            cout << "Your choice: ";
            cin >> choice;

            file = files[choice-1];
            rec.setFile(file);
            cout << "Your choice is " << filesystem::path(file).filename().string() << endl;
        }else{
            file = files[0];
            rec.setFile(file);
        }
    }

    char soundCmd[100] = "paplay ";
    strcat(soundCmd, getenv("HOME"));
    strcat(soundCmd, "/.config/swaync/Sounds/sound.mp3");
    string notify = "notify-send -a Pomo -i ~/.config/swaync/icons/timer.png Pomodoro: ";

    unsigned int temp_nw = nw;
    string str;
    while(true){
        // line 53-59, 1 loop of work sessions >1 dedim cuz if you don't want to continue, long break kerek emes
        while (temp_nw > 1){
            temp_nw--;
            rec.showTime(w_time,1,saveRecord);

            system(&(notify + "\"Take a break.\"")[0]);
            if(sound){ system(&soundCmd[0]);}

            rec.showTime(sb_time,2);

            system(&(notify + "\"Get back to work.\"")[0]);
            if(sound){ system(&soundCmd[0]);}
        }
        rec.showTime(w_time,1, saveRecord);

        // 1 loop bitkesin continue ma zhok pa
        system(&(notify + "\"Do you want to continue?\"")[0]);
        if(sound){ system(&soundCmd[0]);}

        cout << "Do you want to continue? (Y/N)";
        cin >> str;
        if(str != "Y" && str != "y"){
            break;
        }
        temp_nw = nw;
        rec.showTime(lb_time,3);

        system(&(notify + "\"Get back to work.\"")[0]);
        if(sound){ system(&soundCmd[0]);}
    }



	return EXIT_SUCCESS;


}
