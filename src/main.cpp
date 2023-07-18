#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>
#include <string>
#include <sstream>
#include <iomanip>

/* fix all the shit
 *
 * "[IN]-->: "
 * "[OUT]->: "
 */

int main(int argc, char** argv)
{
	if(argc != 2) // dip out if nothing/wrong thing is given
	{
		std::cout << "Usage: [in/out] to clock in, or clock out.\n";
		return 1;
	}

	std::string arg = argv[1];

	if(!(arg == "in" || arg == "out"))
	{
		std::cout << "Usage: [in/out] to clock in, or clock out.\n";
		return 1;
	}

	bool isout = arg == "out";

	std::time_t now;
	std::time(&now);

	std::fstream logfile;
	logfile.open("time.log", std::ios::in);
	std::stringstream initlog;

	bool isnewfile = false;
	if(!logfile)
	{
		std::cout << "No 'time.log' file found, creating file and clocking in.\n";
		isnewfile = true;
	}

	std::string fline = "[TOTAL]: NULL";
	std::string sline;
	double totalt = 0.0;
	std::time_t outtime = now;
	struct tm* utc;
	utc = std::gmtime(&now);
	utc->tm_hour -= !utc->tm_isdst;
	outtime = std::mktime(utc);
	std::string tstamp = std::ctime(&outtime);
	tstamp.pop_back();
	if(!isnewfile)
	{
		std::getline(logfile, fline);
		std::getline(logfile, sline);
		bool wasout = sline.substr(0, 5) == "[OUT]";

		// 4 casses, iin-win, iout-win, iin-wout, iout-wout
		// Is in, Was in: ask to override
		if(!isout && !wasout)
		{
			std::cout << "clock-in found, wish to overwrite? [Y/n]" << std::endl;
			char ans = std::getchar();
			if(ans != 'n')
			{
				// overwrite the old data
				sline = "[IN]-->: " + tstamp;
			}
		}
		// Is in, Was out: Clock in as normal
		if(!isout && wasout)
		{
			initlog << "[IN]-->: " << tstamp << std::endl;
			std::tm tm = {};
			std::stringstream ss(sline.substr(9));
			ss >> std::get_time(&tm, "%a %b %d %H:%M:%S %Y");
			outtime = std::mktime(&tm);
		}

		// is Out, was In: clock out as normal
		if(isout && !wasout)
		{
			initlog << "[OUT]->: " << tstamp << std::endl;
			std::tm tm = {};
			std::stringstream ss(sline.substr(9));
			ss >> std::get_time(&tm, "%a %b %d %H:%M:%S %Y");
			std::time_t intime = std::mktime(&tm);
			double diff = std::difftime(outtime, intime);
			totalt += diff;
			totalt += 3600.0;
		}

		//is OUT, was OUT: ask to overwrite clock-out
		if(isout && wasout)
		{
			std::cout << "Clock-out found, would you like to overwrite? [Y/n]" << std::endl;
			if(std::getchar() != 'n')
			{
				//overwrite old data
				sline = "[OUT]->: " + tstamp;
				totalt += 3600.0;
			}
			else // set clock-out to recorded cock-out time
			{
				std::tm tm = {};
				std::stringstream ss(sline.substr(9));
				ss >> std::get_time(&tm, "%a %b %d %H:%M:%S %Y");
				outtime = std::mktime(&tm);
			}
		}

		initlog << sline << "\n";

		std::string line;
		while(std::getline(logfile, line))
		{
			initlog << line << "\n";
			if(line.substr(0, 4) == "[IN]")
			{
				std::tm tm = {};
				std::stringstream ss(line.substr(9));
				ss >> std::get_time(&tm, "%a %b %d %H:%M:%S %Y");
				std::time_t intime = std::mktime(&tm);
				double diff = std::difftime(outtime, intime);
				totalt += diff;
			}
			else
			{
				std::tm tm = {};
				std::istringstream ss(line.substr(9));
				ss >> std::get_time(&tm, "%a %b %d %H:%M:%S %Y");
				outtime = std::mktime(&tm);
			}
		}

		totalt /= 3600.0;
	}

	fline = "[TOTAL]  |<" + std::to_string(totalt) + ">|";
	if(isnewfile)
	{
		initlog << "[IN]-->: " << tstamp << "\n";
	}

	logfile.close();

	logfile.open("time.log", std::ios::out | std::ios::trunc);
	if(!logfile)
	{
		std::cout << "Failed to open/create 'time.log' file\n";
		return 1;
	}

	logfile << fline << "\n";
	logfile << initlog.str();
	logfile.close();

	std::cout << "File written.\n";
	std::cout << fline << std::endl;

	return 0;
}
