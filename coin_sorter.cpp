// define each coin to a number so that when addressing things
// like COIN_ARRAY or MAX_ROLL we can just type which coin type
// we want instead of the number
#define NICKEL 0
#define DIME 1
#define QUARTER 2
#define LOONIE 3
#define TOONIE 4
#define NUM_OF_COINS 5

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>

using namespace std;
// COIN_ARRAY stores weight and diameter with the tolerance for each coin
// format is weight, weight_tolerance, diameter, diameter_tolerance
const double COIN_ARRAY[NUM_OF_COINS][4] =
{
	{3.95, 0.35, 21.0, 0.8}, // nickel
	{1.75, 0.45, 18.0, 0.7}, // dime
	{4.40, 0.40, 23.7, 0.9}, // quarter
	{7.00, 0.50, 26.0, 1.0}, // loonie
	{7.30, 0.55, 28.0, 1.1}  // toonie
};
// allows us to combine cout statements when different coins are
// being added to the wrappers, so that we just address the array
// element here and it will give back which coin we are using
const string COIN_NAME[NUM_OF_COINS] = {
	"nickel",
	"dime",
	"quarter",
	"loonie",
	"toonie"
};
// maximum number of coins in a roll, order depends on coin defines above
const int MAX_ROLL[NUM_OF_COINS] = { 40, 50, 40, 25, 25 };
// check and see if the value is within the tolerances
// returns false if not, true if it is
bool within(double val, double ideal, double err)
{
	if (val > (ideal + err) || val < (ideal - err)) return false;
	return true;
}
// define a coin class which will hold the sensor readings for the current
// coin being processed
class coin_t {
public:
	int weight;
	double mapped_weight;
	int diameter;
	double mapped_diameter;
	string bent;
	string match;
};
// declare input and output file before hand so double_stream and end_pgm()
// have access to them
ifstream ifile;
ofstream ofile;
// we can declare something that will write to both the file and cout
// this is useful for lots of duplicate lines in the program and it can save on
// program space
class double_stream {};
// define a << operator for the double_stream class so we can pass variables
// and text to it just like cout
template<class T>
inline double_stream & operator << (double_stream &s, T val)
{
	// print to ofile and cout
	cout << val;
	ofile << val;
	return s;
}
// just a small function that can take care of closing the input and output files,
// or just one of them since we do it many times in the main
void end_pgm(ifstream *i, ofstream *o)
{
	if (i) i->close();
	if (o) o->close();
	exit(0);
}

int main(void)
{
	string fname;
	string line[5000];
	string dummy;

	istringstream iss;

	double_stream dout;

	coin_t filecoin;

	bool fail[5] = { false };

	int numOfLines = -1;
	int other_coins = 0;
	int number_of_other_container_weights = 0;
	int coins_in_current_other = 0;
	int actualLineNumber = 0;
	int i = 0;
	int j = 0;
	int coin = -1;
	int advertised_lines = 0;

	int wrapper[NUM_OF_COINS] = { 0 };
	int rolls[NUM_OF_COINS] = { 0 };

	double bent_container = 0.00;
	double other_container_weight = 0.00;
	double total_other_weight = 0.00;
	double tmp;

	while (1)
	{
		cout << "Type the name of the input file containing sensor readings:\n";
		cin >> fname;
		ifile.open(fname);
		if (!ifile)
		{
			if (i == 2)
			{
				cerr << "ERROR: You exceeded maximum number of tries allowed while entering the output file name\n";
				end_pgm(&ifile, &ofile);
			}
			cerr << "ERROR: File " << fname << " could not be opened for input\n";
		}
		else break;
		i++;
	}

	i = 0;

	while (1)
	{
		cout << "Type the name of the output file which will hold the simulation results:\n";
		cin >> fname;
		ofile.open(fname);
		if (!ofile)
		{
			if (i == 2)
			{
				cerr << "ERROR: You exceeded maximum number of tries allowed while entering the output file name\n";
				end_pgm(&ifile, &ofile);
			}
			cerr << "ERROR: File " << fname << " could not be opened for output\n";
		}
		else break;
		i++;
	}
	// read all the lines from the input file and store them into
	// a line string
	// numOfLines stores the actual number of lines in the file,
	// not how many are stated in line 1
	while (getline(ifile, line[++numOfLines]))
	{
		if (numOfLines == 5000)
		{
			cerr << "ERROR: The number of sensor readings is out of range\n";
			end_pgm(&ifile, &ofile);
		}
	}
	// if there is nothing in the file
	// numOfLines is incremented to zero even if there is nothing read
	if (numOfLines == 0)
	{
		cerr << "ERROR: Input data file is empty\n";
		end_pgm(&ifile, &ofile);
	}
	// is there a successful conversion to an integer on the first line
	// of the file?
	iss.str(line[0]);
	if (!(iss >> advertised_lines))
	{
		cerr << "ERROR: First piece of data in the file is not an integer\n";
		end_pgm(&ifile, &ofile);
	}
	// repeat this block of code for how many actual lines are in the file
	// we determined how many lines are in the file above
	// start at i = 1 and not i = 0 because the first line is always the number
	// of entries in the file
	for (i = 1; i < numOfLines; i++)
	{
		// make an istringstream for each line so we can read formatted data out from 
		iss.str(line[i]);
		// test each element of the line while reading it to see if it is able to be read
		// clear every time it fails so that the fails don't carry over to the next read
		iss.clear();
		fail[0] = !bool(iss >> filecoin.weight);
		iss.clear();
		fail[1] = !bool(iss >> filecoin.diameter);
		iss.clear();
		fail[2] = !bool(iss >> filecoin.bent);
		iss.clear();
		fail[3] = !bool(iss >> filecoin.match);
		iss.clear();
		fail[4] = !bool(iss >> dummy);
		// there is a blank line in the file, so ignore it and move to the next one
		if (fail[0] && fail[1] && fail[2] && fail[3])
		{
			continue;
		}
		// if we've made it this far then we have a line that isn't blank
		// keep track of how many lines of actual data are in the file
		actualLineNumber++;
		// the following block is error handling for all the various types of
		// problems that could happen while reading the input file
		if (fail[0])
		{
			cerr << "ERROR: Weight sensor value read on line " << i << " is not an integer\n"
				<< "Simulation terminated early: Please correct your data file\n";
			end_pgm(&ifile, &ofile);
		}
		else if (fail[1] && fail[2] && fail[3])
		{
			cerr << "ERROR: Weight sensor measurement only\n"
				<< "Ignoring line " << actualLineNumber << " of the input file\n";
			continue;
		}
		else if (fail[1])
		{
			cerr << "ERROR: Diameter sensor value read on line " << actualLineNumber << " is not an integer\n"
				<< "Simulation terminated early: Please correct your data file\n";
			end_pgm(&ifile, &ofile);
		}
		else if (fail[2] && fail[3])
		{
			cerr << "ERROR: Weight and diameter sensor measurements only\n"
				<< "Ignoring line " << actualLineNumber << " of the input file\n";
			continue;
		}
		else if (filecoin.bent != "bent" && filecoin.bent != "usable")
		{
			cerr << "ERROR: Result of test to determine if coin is bent at line " << actualLineNumber << " is invalid\n"
				<< "Ignoring this line of data\n";
			continue;
		}
		else if (fail[3])
		{
			cerr << "ERROR: Weight and diameter sensor measurements and bent string only\n"
				<< "Ignoring line " << actualLineNumber << " of the input file\n";
			continue;
		}
		else if (filecoin.match != "BothMatch" && filecoin.match != "OneMatch" && filecoin.match != "NoMatch")
		{
			cerr << "ERROR: image processing result at line " << actualLineNumber << " is invalid\n"
				<< "Ignoring this line of data\n";
			continue;
		}
		else if (!fail[4])
		{
			cerr << "ERROR: Extra data at line " << actualLineNumber << ".  Ignoring extra data\n";
		}
		if (filecoin.weight < 0 || filecoin.weight > 255 || filecoin.diameter < 0 || filecoin.diameter > 255)
		{
			cerr << "ERROR: Sensor reading out of range, ignoring line " << actualLineNumber << " in the input file\n";
			continue;
		}
		// map the sensor readings to their actual values
		filecoin.mapped_weight = double(filecoin.weight) * 2.0 / 51.0;
		filecoin.mapped_diameter = 10.0 + (double(filecoin.diameter) * (30.0 / 255.0));
		// set coin to something that doesn't match any of the coin pre-defines above
		coin = -1;
		for (j = 0; j < NUM_OF_COINS; j++)
		{
			// checks for matches for all possible coins
			if (within(filecoin.mapped_weight, COIN_ARRAY[j][0], COIN_ARRAY[j][1]) &&
				within(filecoin.mapped_diameter, COIN_ARRAY[j][2], COIN_ARRAY[j][3]))
			{
				coin = j;
			}
		}
		// bent coin handler
		if (filecoin.bent == "bent")
		{
			// store new bent coin container weight in temporary variable
			// so that we don't have to do addition twice
			tmp = bent_container + filecoin.mapped_weight;
			// this is the same for both cases
			dout << "The Coin Sorter has sent this coin to the bent coin container\n";
			// will the container be too big?
			if (tmp > 100.0)
			{
				// put the coin into a new bent container
				bent_container = filecoin.mapped_weight;
				dout << "This coin does not fit in the bent coin container\n"
					<< "The bent coin container has been replaced\n"
					<< "The coin in the new bent coin container weigh " << fixed << setw(6) << setprecision(2) << bent_container << " grams\n";
			}
			else
			{
				// add the coin to the current bent container
				bent_container = tmp;
				dout << "The coins in the bent coin container now weigh " << fixed << setw(6) << setprecision(2) << bent_container << " grams\n";
			}
		}
		// other coin handler
		// this will execute when the image processor fails or when
		// there is no common weight and diameter match
		else if (coin == -1 || filecoin.match != "BothMatch")
		{
			// total_other_weight is the total weight of all the other coins
			// processed in every bin
			total_other_weight += filecoin.mapped_weight;
			// store new bent coin container weight in temporary variable
			// so that we don't have to do addition twice
			tmp = other_container_weight + filecoin.mapped_weight;
			// this is the same for both cases
			dout << "The Coin Sorter has sent this coin to the other coin container\n";
			// total number of other coins processed
			other_coins++;
			// will the container be too big?
			if (tmp > 200.0)
			{
				// put the coin into a new other container
				other_container_weight = filecoin.mapped_weight;
				coins_in_current_other = 1;
				number_of_other_container_weights++;
				// it's not in the assignment to print an message when the
				// other container overflows, but it would go here
			}
			else
			{
				// add the coin to the current other container
				other_container_weight = tmp;
				coins_in_current_other++;
			}
			// this is the same for both cases
			dout << "The coins in the other coin container now weigh " << fixed << setw(6) << setprecision(2) << other_container_weight << " grams\n";
		}
		else
		{
			// if we've made it this far, then the image processor is OK,
			// the coin isn't bent and there is a valid match from both
			// weight and diameter sensors
			// using arrays and predefined coin names (above), we can combine
			// every cout statement into one which will change based on the
			// value of coin
			dout << "The Coin Sorter has sent one coin to the " << COIN_NAME[coin] << "s wrapper\n";
			// add one to the wrapper but is the new size bigger than the maximum
			// number of coins allowed in a roll?
			if (++wrapper[coin] < MAX_ROLL[coin])
			{
				dout << "There are now " << wrapper[coin] << " coins in the " << COIN_NAME[coin] << "s wrapper\n";
			}
			else
			{
				// reset the number of coins in the wrapper
				wrapper[coin] = 0;
				// a new completed roll has been created
				rolls[coin]++;
				dout << "The " << COIN_NAME[coin] << " wrapper is now full\n"
					<< "The " << COIN_NAME[coin] << " wrapper has now been replaced\n";
			}
		}
	}
	// if the actual number of entries in the file is less than the number
	// of entries displayed in the first line
	if (actualLineNumber < advertised_lines)
	{
		cerr << "ERROR: No more data\n"
			<< "Simulation completed early before line " << actualLineNumber + 1 << " of input\n";
	}
	dout << fixed << "\n\n\nSUMMARY\n";
	// again using arrays to simplify the summary, combine all cout statements
	// into one loop
	for (i = 0; i < NUM_OF_COINS; i++)
	{
		dout << "The Coin Sorter made " << setw(3) << rolls[i] << " rolls of " << COIN_NAME[i] << "s.\n"
			<< "    There are " << setw(2) << wrapper[i] << " " << COIN_NAME[i] << "s in the partially full roll.\n";
	}
	dout << "The Coin Sorter processed " << setw(5) << other_coins << " other coins.\n"
		<< "    The other coins completely filled " << setw(3) << number_of_other_container_weights << " containers\n"
		<< "    There were " << setw(2) << coins_in_current_other << " other coins in the partially full container\n"
		<< "    The total weight of the other coins was " << setw(9) << setprecision(3) << total_other_weight << " grams\n"
		<< "The Coin Sorter processed " << setw(11) << setprecision(4) << bent_container << " g of bent coins\n";
	end_pgm(&ifile, &ofile);
}
