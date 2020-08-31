#include <iostream>
#include <stack>
#include <vector>

using namespace std;

#include "olcConsoleGameEngineSDL.h"

class OneLoneCoder_GameOfLife : public olcConsoleGameEngine
{
public:
	OneLoneCoder_GameOfLife()
	{
		m_sAppName = L"Game Of Life";
	}

private:
	int *m_output, *m_state;
	long int nAlive, nDead, iteraciones = 0;
	int nNeighbours, nCells;
	double shannon = 0, probability_alive = 0, probability_dead = 0;
	double average, variance_sum = 0;
	ofstream avg_writer, shannon_writer, still_writer, oscilator_writer, chaos_writer;
	ifstream configuration_reader;
	int birth_rules[10], survive_rules[10];
	int birth_index, survive_index;
	bool survive, birth;
	long int i, j;
	//vector< stack<char> > stacks;
	int sumNeighbours, still_counter, oscilator_counter;
	bool nOne, nTwo, nThree, nFour, nSix, nSeven, nEight, nNine;
	string general_option;
	int pattern_switch;

protected:
	// Called by olcConsoleGameEngine
	virtual bool OnUserCreate()
	{
		nCells = ScreenWidth() * ScreenHeight();
		m_output = new int[ScreenWidth() * ScreenHeight()];
		m_state = new int[ScreenWidth() * ScreenHeight()];
		string numberOfOnesString, rules;
		string input_line, file_configuration_name;
		int numberOfOnesNumerator, numberOfOnesDenominator, counterOfOnes;
		long int random_index, line_offset, y;

		// CLEANING UP MEMORY SPACE
		memset(m_output, 0, ScreenWidth() * ScreenHeight() * sizeof(int));
		memset(m_state, 0, ScreenWidth() * ScreenHeight() * sizeof(int));

		// ADDING AND INITIALIZING A STACK FOR EVERY CELL
		/*
		for(i = 0; i < ScreenWidth() * ScreenHeight(); i++)
		{
			stack<char> aux;	
			aux.push('Z');
			stacks.push_back(aux);	
			// cout << aux.top() << endl;
		}
		*/
		
		// READING RULES
		cout << "Enter rules: ";
		getline(cin, rules);		
		birth_index = 0;
		survive_index = 0;
		if(rules[0] == 'B' || rules[0] == 'b')
		{
			i = 1; //Skipping 'B'
			while(rules[i] != '/')
				birth_rules[birth_index++] = rules[i++] - '0';

			if(rules[i + 1] != 'S') // If there's no 'S'
			{
				cout << "Error: Wrong syntax, should be an 'S' after '/'" << endl;
				exit(EXIT_FAILURE);
			}
			i += 2;	//Skipping '/' and 'S'

			while(i < rules.length())
				survive_rules[survive_index++] = rules[i++] - '0';
		}
		else
		{
			cout << "Error: Wrong syntax" << endl;
			exit(EXIT_FAILURE);
		}

		// ASKING FOR INITIAL CONFIGURATION
		cout << "Do you want to load a configuration? Yes[Y-y] | No[N-n]: ";
		getline(cin, general_option);
		if(general_option[0] == 'Y' || general_option[0] == 'y')
		{
			// READING FILE INPUT NAME
			cout << "Enter the file name: ";
			getline(cin, file_configuration_name);
			ifstream configuration_reader(file_configuration_name);
			if(configuration_reader.is_open())
			{
				// CUSTOM SET FUNCTION	
				auto set = [&](int x, int y, wstring s)
				{
					int p = 0;
					for (auto c : s)
					{
						m_state[y * ScreenWidth() + x + p] = c == L'#' ? 1 : 0;
						p++;
					}
				};
				// SETTING CONFIGURATION INTO M_STATE
				y = 0;
				while(getline(configuration_reader, input_line))
				{
					line_offset = 0;
					for(auto c: input_line)
					{
						m_state[y * ScreenWidth() + line_offset] = c == L'1' ? 1 : 0;
						line_offset++;
					}
					y++;
				}
				configuration_reader.close();
			}
			else
			{
				cout << "Error: Unable to open configuration file.";
				exit(EXIT_FAILURE);		
			}
		}
		else
		{
			// READING PERCENTAGE OF 1'S ACCURACY INCREMENTED V2
			cout << "Enter percentage of 1's: ";
			getline(cin, numberOfOnesString);
			if(numberOfOnesString[0] == '0') // i.e. 0.5%
			{
				numberOfOnesNumerator = 0;
				numberOfOnesDenominator = numberOfOnesString[2] - '0';
				numberOfOnesDenominator *= 10; // Applying scale i.e. 0.5% = 5(user input) * 10 = 50 of a total of 1000;	
			}
			else if(numberOfOnesString.length() == 2) // i.e. 5%
			{
				numberOfOnesString = numberOfOnesString.substr(0, 1);
				numberOfOnesNumerator = stoi(numberOfOnesString);
				numberOfOnesNumerator *= 10; // Applying scale i.e. 5% = 5(user input) * 10 = 50 of a total of 1000;
				numberOfOnesDenominator = 0;
			}
			else if(numberOfOnesString.length() == 3) // i.e. 50%
			{
				numberOfOnesString = numberOfOnesString.substr(0, 2);
				numberOfOnesNumerator = stoi(numberOfOnesString);
				numberOfOnesNumerator *= 10; // Applying scale i.e. 50% = 50(user input) * 10 = 500 of a total of 1000;
				numberOfOnesDenominator = 0;
			}
			else // i.e. 70.5%
			{
				numberOfOnesString = numberOfOnesString.substr(0, 2);
				numberOfOnesNumerator = stoi(numberOfOnesString);
				numberOfOnesNumerator *= 10; // Applying scale i.e. 70.5% = 70(user input) * 10 = 700 of a total of 1000;
				numberOfOnesDenominator = numberOfOnesString[3] - '0';
				numberOfOnesDenominator *= 10; // Applying scale i.e. 70.5% = 5(user input) * 10 = 50 of a total of 50;
			}

			// GENERATING FIRST M_STATE
			counterOfOnes = 0;
			for (i = 0; i < ScreenWidth() * ScreenHeight(); i++)
			{
				if(((i + 1) % 1001) == 0)
				{	
					while(counterOfOnes < (numberOfOnesNumerator + numberOfOnesDenominator))
					{
						//cout << "Counter of ones: " << counterOfOnes << " < Number of ones: " << numberOfOnes << endl;
						random_index = (i - 1000) + (rand() % 1000);
						if(m_state[random_index] == 0)
						{
							m_state[random_index] = 1;
							counterOfOnes++;
						}
					}

					while(counterOfOnes > (numberOfOnesNumerator + numberOfOnesDenominator))
					{
						//cout << "Counter of ones: " << counterOfOnes << " > Number of ones: " << numberOfOnes << endl;
						random_index = (i - 1000) + (rand() % 1000);
						if(m_state[random_index] == 1)
						{
							m_state[random_index] = 0;
							counterOfOnes--;
						}
					}
					counterOfOnes = 0;
				}

				if((rand() % 2) == 1)
				{
					counterOfOnes++;
					m_state[i] = 1;
				}
				else
					m_state[i] = 0;
			}
		}

		// ASKING FOR PATTERN RECOGNITION
		cout << "Pattern Recognition -> 1-[Osc-Still-Chaos] | 2-[Static-NonStatic] | 3-[NoRecognition] : ";
		getline(cin, general_option);
		if(general_option[0] == '1')
			pattern_switch = 1;
		else if(general_option[0] == '2')
			pattern_switch = 2;
		else
			pattern_switch = 3;
		
		/*
		//Diehard
		set(80, 50, L"       #");
		set(80, 51, L" ##  ");
		set(80, 52, L"  #   ###");
		*/

		/*
		//Acorn
		set(80, 50, L"  #");
		set(80, 51, L"    #");
		set(80, 52, L" ##  ###");
		*/

		/*
		// R-Pentomino
		set(80, 50, L"  ## ");
		set(80, 51, L" ##  ");
		set(80, 52, L"  #  ");
		*/
		
		
		// Gosper Glider Gun
		// set(60, 45, L"........................#............");
		// set(60, 46, L"......................#.#............");
		// set(60, 47, L"............##......##............##.");
		// set(60, 48, L"...........#...#....##............##.");
		// set(60, 49, L"##........#.....#...##...............");
		// set(60, 50, L"##........#...#.##....#.#............");
		// set(60, 51, L"..........#.....#.......#............");
		// set(60, 52, L"...........#...#.....................");
		// set(60, 53, L"............##.......................");
		

		/*
		// ORIGIN
		set(60, 53, L"#..");
		set(60, 54, L"..#");
		set(60, 55, L"..#");
		set(60, 56, L"#..");
		
		set(84, 53, L"..#");
		set(84, 54, L"#..");
		set(84, 55, L"#..");
		set(84, 56, L"..#");
		*/

		// Infinite Growth
		//set(20, 50, L"########.#####...###......#######.#####");

		//OPENING FILES
		avg_writer.open("average.txt", ios_base::out | ios_base::trunc);
		if(!avg_writer)
		{
			cout << "Error: Cannot open 'average' file" << endl;
			return true;
		}
		shannon_writer.open("shannon.txt", ios_base::out | ios_base::trunc);
		if(!shannon_writer)
		{
			cout << "Error: Cannot open 'shannon' file" << endl;
			return true;
		}
		still_writer.open("still.txt", ios_base::out | ios_base::trunc);
		if(!still_writer)
		{
			cout << "Error: Cannot open 'still' file" << endl;
			return true;
		}
		oscilator_writer.open("oscilator.txt", ios_base::out | ios_base::trunc);
		if(!oscilator_writer)
		{
			cout << "Error: Cannot open 'oscilator' file" << endl;
			return true;
		}
		chaos_writer.open("chaos.txt", ios_base::out | ios_base::trunc);
		if(!chaos_writer)
		{
			cout << "Error: Cannot open 'chaos' file" << endl;
			return true;
		}
		// SETTING PRECISION
		cout.precision(8);		
		avg_writer.precision(8);
		shannon_writer.precision(8);
		still_writer.precision(8);
		oscilator_writer.precision(8);
		chaos_writer.precision(8);

		return true;
	}

	

	// Called by olcConsoleGameEngine
	virtual bool OnUserUpdate(float fElapsedTime)
	{
		// HANDLING USER INPUT
		if(m_keys[VK_ESCAPE].bPressed)
		{
			cout << "Do you want to save the configuration? Yes[Y-y] | No[N-n]: ";
			getline(cin, general_option);
			// SAVING CONFIGURATION
			if(general_option[0] == 'Y' || general_option[0] == 'y')
			{
				ofstream configuration_writer("saved.txt");
				if(configuration_writer.is_open())
				{
					for(i = 0; i < ScreenWidth() * ScreenHeight(); i++)
					{
						if(!(i % ScreenWidth()) && i != 0)
							configuration_writer << endl;
						if(m_output[i])
							configuration_writer << '1';
						else
							configuration_writer << '0';
					}
				}
				else
					cout << "Error: Unable to open configuration writer" << endl;
				configuration_writer.close();
			}
			// WRITTING VARIANCE
			ofstream variance_writer("variance.txt");
			if(variance_writer.is_open())
			{
				variance_writer.precision(8);
				variance_writer << (double) variance_sum / nCells << endl;
			}
			else
				cout << "Error: Unable to open variance writer" << endl;
			// CLOSING ALL WRITERS
			variance_writer.close();
			avg_writer.close();
			shannon_writer.close();
			still_writer.close(); // What is green
			oscilator_writer.close(); // What is yellow
			chaos_writer.close(); // What is white
			exit(EXIT_SUCCESS);
		}
		if(m_keys[VK_SPACE].bHeld)	
			return true;
		else if(m_keys[VK_F1].bHeld)
			this_thread::sleep_for(100ms);	
		else if(m_keys[VK_F2].bHeld)
			this_thread::sleep_for(200ms);	
		else if(m_keys[VK_F3].bHeld)
			this_thread::sleep_for(300ms);	
		else if(m_keys[VK_F5].bHeld)
			this_thread::sleep_for(500ms);	
		else if(m_keys[VK_F10].bHeld)
			this_thread::sleep_for(1000ms);	
		else if(m_keys[VK_F11].bHeld)
			this_thread::sleep_for(2000ms);	

		// ITERATIONS STOP			
		if(iteraciones > 10000)
			return true;
		// TWO HANDLY FUNCTIONS
		auto cell = [&](int x, int y)
		{
			return m_output[y * ScreenWidth() + x];
		};
		auto cellt1 = [&](int x, int y)
		{
			return m_state[y * ScreenWidth() + x];
		};
		// STORE OUTPUT STATE
		for (i = 0; i < ScreenWidth() * ScreenHeight(); i++)
			m_output[i] = m_state[i];

		// 
		nAlive = 0;
		still_counter = 0;
		oscilator_counter = 0;
		for (int x = 0; x < ScreenWidth(); x++)
			for (int y = 0; y < ScreenHeight(); y++)
			{
				nNeighbours = 0;
				
				// COUNTING NEIGHBOURS AND THEIR POSITIONS REQUIRED WHEN PATTERN RECOGNITION OSC-STILL-CHAOS IS ON
				if(pattern_switch == 1)
				{
					nOne = false;
					nTwo = false;
					nThree = false;
					nFour = false;
					nSix = false;
					nSeven = false;
					nEight = false;
					nNine = false;
				
					if(cell((x - 1 + ScreenWidth()) % ScreenWidth(), (y - 1 + ScreenHeight()) % ScreenHeight()) == 1)
					{
						nNeighbours++;
						nOne = true;
					}
					if(cell((x - 0 + ScreenWidth()) % ScreenWidth(), (y - 1 + ScreenHeight()) % ScreenHeight()) == 1)
					{
						nNeighbours++;
						nTwo = true;
					}
					if(cell((x + 1 + ScreenWidth()) % ScreenWidth(), (y - 1 + ScreenHeight()) % ScreenHeight()) == 1)
					{
						nNeighbours++;
						nThree = true;
					}
					if(cell((x - 1 + ScreenWidth()) % ScreenWidth(), (y - 0 + ScreenHeight()) % ScreenHeight()) == 1)
					{
						nNeighbours++;
						nFour = true;
					}
					if(cell((x + 1 + ScreenWidth()) % ScreenWidth(), (y - 0 + ScreenHeight()) % ScreenHeight()) == 1)
					{
						nNeighbours++;
						nSix = true;
					}
					if(cell((x - 1 + ScreenWidth()) % ScreenWidth(), (y + 1 + ScreenHeight()) % ScreenHeight()) == 1)
					{
						nNeighbours++;
						nSeven = true;
					}
					if(cell((x - 0 + ScreenWidth()) % ScreenWidth(), (y + 1 + ScreenHeight()) % ScreenHeight()) == 1)
					{
						nNeighbours++;
						nEight = true;
					}
					if(cell((x + 1 + ScreenWidth()) % ScreenWidth(), (y + 1 + ScreenHeight()) % ScreenHeight()) == 1)
					{
						nNeighbours++;
						nNine = true;
					}
				}
				else
				{
					// NORMAL WAY OF COUNTING NEIGHBOURS
					for(i = -1; i < 2; i++)
						for(j = -1; j < 2; j++)
							nNeighbours += cell((x + i + ScreenWidth()) % ScreenWidth(), (y + j + ScreenHeight()) % ScreenHeight());
							
					nNeighbours -= cell(x, y);
				}

				// APPLYING SURVIVE RULES
				if(cell(x, y) == 1)
				{
					survive = false;
					for(i = 0; i < survive_index; i++)
						if(nNeighbours == survive_rules[i])
						{
							m_state[y * ScreenWidth() + x] = 1;
							survive = true;
							break;
						}
					
					if(!survive)
						m_state[y * ScreenWidth() + x] = 0;
				}
				else // APPLYING BIRTH RULES
				{
					birth = false;
					for(int i = 0; i < birth_index; i++)
						if(nNeighbours == birth_rules[i])
						{
							m_state[y * ScreenWidth() + x] = 1;
							birth = true;
							break;
						}
					
					if(!birth)
						m_state[y * ScreenWidth() + x] = 0;
				}	

				// PATTERN RECOGNITION PROCESS
				if(pattern_switch == 1) // Handle Oscilators-Still-Gliders-Chaos
				{
					// AUTOMATA NEIGHBOURS POSITIONS V1
					if(cell(x, y) == 1)
					{
						nAlive++;
						if(cellt1(x, y))
						{
							still_counter++;
							//Block
							if(nSix && nEight && nNine)
								Draw(x, y, PIXEL_SOLID, FG_DARK_GREEN);
							else if(nFour && nSeven && nEight)
								Draw(x, y, PIXEL_SOLID, FG_DARK_GREEN);
							else if(nTwo && nThree && nSix)
								Draw(x, y, PIXEL_SOLID, FG_DARK_GREEN);
							else if(nOne && nTwo && nFour)
								Draw(x, y, PIXEL_SOLID, FG_DARK_GREEN);
							//Bee-hive
							else if(nThree && nNine)
								Draw(x, y, PIXEL_SOLID, FG_DARK_GREEN);
							else if(nSix && nSeven)
								Draw(x, y, PIXEL_SOLID, FG_DARK_GREEN);
							else if(nFour && nNine)
								Draw(x, y, PIXEL_SOLID, FG_DARK_GREEN);
							else if(nOne && nSeven)
								Draw(x, y, PIXEL_SOLID, FG_DARK_GREEN);
							else if(nOne && nSix)
								Draw(x, y, PIXEL_SOLID, FG_DARK_GREEN);
							else if(nThree && nFour)
								Draw(x, y, PIXEL_SOLID, FG_DARK_GREEN);
							else if(nThree && nEight)
								Draw(x, y, PIXEL_SOLID, FG_DARK_GREEN);
							else if(nTwo && nNine)
								Draw(x, y, PIXEL_SOLID, FG_DARK_GREEN);
							// Loaf
							else if(nThree && nNine)
								Draw(x, y, PIXEL_SOLID, FG_DARK_GREEN);
							else if(nSix && nSeven)
								Draw(x, y, PIXEL_SOLID, FG_DARK_GREEN);
							else if(nFour && nNine)
								Draw(x, y, PIXEL_SOLID, FG_DARK_GREEN);
							else if(nOne && nNine)
								Draw(x, y, PIXEL_SOLID, FG_DARK_GREEN);
							else if(nOne && nThree)
								Draw(x, y, PIXEL_SOLID, FG_DARK_GREEN);
							else if(nOne && nEight)
								Draw(x, y, PIXEL_SOLID, FG_DARK_GREEN);
							else if(nTwo && nSeven)
								Draw(x, y, PIXEL_SOLID, FG_DARK_GREEN);
							// Boat
							else if(nSix && nEight)
								Draw(x, y, PIXEL_SOLID, FG_DARK_GREEN);
							else if(nFour && nSeven && nNine)
								Draw(x, y, PIXEL_SOLID, FG_DARK_GREEN);
							else if(nTwo && nThree && nSeven)
								Draw(x, y, PIXEL_SOLID, FG_DARK_GREEN);
							else if(nOne && nSeven)
								Draw(x, y, PIXEL_SOLID, FG_DARK_GREEN);
							else if(nOne && nThree)
								Draw(x, y, PIXEL_SOLID, FG_DARK_GREEN);
							else if(nSix && nSeven && nNine)
								Draw(x, y, PIXEL_SOLID, FG_DARK_GREEN);
							else if(nTwo && nFour)
								Draw(x, y, PIXEL_SOLID, FG_DARK_GREEN);
							// Tub
							else if(nSeven && nNine)
								Draw(x, y, PIXEL_SOLID, FG_DARK_GREEN);
							else if(nThree && nNine)
								Draw(x, y, PIXEL_SOLID, FG_DARK_GREEN);
							else if(nOne && nSeven)
								Draw(x, y, PIXEL_SOLID, FG_DARK_GREEN);
							else if(nOne && nThree)
								Draw(x, y, PIXEL_SOLID, FG_DARK_GREEN);
							// Steady
							else if(nOne && nSeven && nNine)
								Draw(x, y, PIXEL_SOLID, FG_DARK_GREEN);
							// Blinker
							else if(nFour && nSix)
							{
								Draw(x, y, PIXEL_SOLID, FG_YELLOW);
								still_counter--;
								oscilator_counter++;
							}
							else if(nTwo && nEight)
							{
								Draw(x, y, PIXEL_SOLID, FG_YELLOW);
								still_counter--;
								oscilator_counter++;
							}
							else
							{
								Draw(x, y, PIXEL_SOLID, FG_WHITE);
								still_counter--;
							}
						}
						else
						{
							oscilator_counter++;
							if(nSix && m_state[y * ScreenWidth() + x] == 0)
								Draw(x, y, PIXEL_SOLID, FG_YELLOW);
							else if(nFour && m_state[y * ScreenWidth() + x] == 0)
								Draw(x, y, PIXEL_SOLID, FG_YELLOW);
							else if(nEight && m_state[y * ScreenWidth() + x] == 0)
								Draw(x, y, PIXEL_SOLID, FG_YELLOW);
							else if(nTwo && m_state[y * ScreenWidth() + x] == 0)
								Draw(x, y, PIXEL_SOLID, FG_YELLOW);
							else
							{
								Draw(x, y, PIXEL_SOLID, FG_WHITE);
								oscilator_counter--;
							}
						}										
						
					// 	// Glider Fase 1
					// 	else if(nNine)
					// 		Draw(x, y, PIXEL_SOLID, FG_DARK_RED);
					// 	else if(nOne && nThree && nSix)
					// 		Draw(x, y, PIXEL_SOLID, FG_DARK_RED);
					// 	else if(nTwo && nFour)
					// 		Draw(x, y, PIXEL_SOLID, FG_DARK_RED);
					// 	else if(nTwo && nSeven && nEight)
					// 		Draw(x, y, PIXEL_SOLID, FG_DARK_RED);
					// 	else if(nEight)
					// 		Draw(x, y, PIXEL_SOLID, FG_DARK_RED);
					// 	// Glider Fase 2
					// 	else if(nThree && nSix)
					// 		Draw(x, y, PIXEL_SOLID, FG_DARK_RED);
					// 	else if(nTwo && nThree && nFour)
					// 		Draw(x, y, PIXEL_SOLID, FG_DARK_RED);
					// 	else if(nOne && nSix && nSeven && nEight)
					// 		Draw(x, y, PIXEL_SOLID, FG_DARK_RED);
					// 	else if(nFour && nSeven)
					// 		Draw(x, y, PIXEL_SOLID, FG_DARK_RED);
					// 	// Glider Fase 3
					// 	else if(nSix)
					// 		Draw(x, y, PIXEL_SOLID, FG_DARK_RED);
					// 	else if(nThree && nFour && nSix)
					// 		Draw(x, y, PIXEL_SOLID, FG_DARK_RED);
					// 	else if(nTwo && nFour)
					// 		Draw(x, y, PIXEL_SOLID, FG_DARK_RED);
					// 	else if(nOne && nSeven && nEight)
					// 		Draw(x, y, PIXEL_SOLID, FG_DARK_RED);
					// 	// Glider Fase 4
					// 	else if(nSeven && nEight)
					// 		Draw(x, y, PIXEL_SOLID, FG_DARK_RED);
					// 	else if(nOne && nThree && nSix && nEight)
					// 		Draw(x, y, PIXEL_SOLID, FG_DARK_RED);
					// 	else if(nTwo && nFour && nSeven)
					// 		Draw(x, y, PIXEL_SOLID, FG_DARK_RED);
					// 	else if(nTwo && nThree)
					// 		Draw(x, y, PIXEL_SOLID, FG_DARK_RED);
					// 	else if(m_state[y * ScreenWidth() + x] == 1)
					// 		Draw(x, y, PIXEL_SOLID, FG_DARK_GREEN);
					// 	else
					// 		Draw(x, y, PIXEL_SOLID, FG_WHITE);
					}
					else
						Draw(x, y, PIXEL_SOLID, FG_BLACK);
				}
				else if(pattern_switch == 2) // Hanldes Static and Non Static Pattern
				{
					// DRAWS STATIC AND NON STATIC CELLS
					if (cell(x, y) == 1)
					{
						if(m_state[y * ScreenWidth() + x] == 1)
						{
							Draw(x, y, PIXEL_SOLID, FG_DARK_GREEN);	
							still_counter++;
						}
						else
						{
							Draw(x, y, PIXEL_SOLID, FG_WHITE);
							oscilator_counter++;
						}
							nAlive++;
					}
					else
						Draw(x, y, PIXEL_SOLID, FG_BLACK);
				}
				else
				{
					// DRAWS CELLS OLD FASHION
					if (cell(x, y) == 1)
					{
						Draw(x, y, PIXEL_SOLID, FG_WHITE);
						nAlive++;
					}
					else
						Draw(x, y, PIXEL_SOLID, FG_BLACK);	
				}
			}

		// CALCULUS SHANNON
		nDead = ScreenHeight() * ScreenWidth() - nAlive;
		probability_alive = (double) nAlive / (ScreenWidth() * ScreenHeight());
		probability_dead = (double) nDead / (ScreenWidth() * ScreenHeight());
		shannon = - probability_alive * log2(probability_alive) - probability_dead * log2(probability_dead);
		// CALCULUS VARIANCE
		average = ((double) nAlive / nCells) * 100;
		variance_sum += pow((double) nAlive - average, 2);

		// WRITTING
		avg_writer << average << endl;
		shannon_writer << shannon << endl;
		shannon = 0;
		
		if(pattern_switch == 1 || pattern_switch == 2)
		{
			still_writer << ((double) still_counter / nCells) * 100 << endl;
			oscilator_writer << ((double) oscilator_counter / nCells) * 100 << endl;
			chaos_writer << ((double) (nCells - nDead - still_counter - oscilator_counter) / nCells) * 100 << endl;
		}
		
		cout << iteraciones++ << endl;

		return true;
	}
};


int main()
{
	// Seed random number generator
	srand(clock());

	// Use olcConsoleGameEngine derived app
	OneLoneCoder_GameOfLife game;
	game.ConstructConsole(500, 500, 2, 2);
	game.Start();
	
	return 0;
}
