/**************************************************************************************
*                                CMPT 128 ASSIGNMENT #2                               *
* Author: Judd Foster and Kaj Grant-Mathiasen                                         *
* Date Completed: Nov 28th, 2018                                                      *
* Purpose: This is a C++ program that plays a Cylindrical Mule Checkers Game.         *
* This game is similar to checkers but the last row consists of mules. Pieces jump    *
* The opponent's pieces to get rid of them. Pieces can jump around the edge.          *
* A checker becomes a king when it reaches the other side of the board. Kings         *
* can jump backwards and forwards.                                                    *
* A player can win if:                                                                *
* The opponent has no possible moves.                                                 *
* The opponents checkers are completely removed.                                      *
* You remove all of your mules from the board.                                        *
* A mule becomes a king.                                                              *
* Soldier pieces are either WS or RS.                                                 *
* Mule pieces are either WM or RM.                                                    *
* A function description is provided at the begining of each function.                *
***************************************************************************************/

#include <iostream>
#include <iomanip>
#include <string>
#include <cmath>

using namespace std;

const int MAX_ARRAY_SIZE = 18;
const int MIN_ARRAY_SIZE = 8;
const int MAX_PIECES = 72;
const int NOPLAYER = 0;
const int WHITEWINS = 1;
const int REDWINS = 2;
const int NOONEWINS = 0;
const int WHITESOLDIER = 1;
const int WHITEMULE = 2;
const int WHITEKING = 3;
const int REDSOLDIER = 4;
const int REDMULE = 5;
const int REDKING = 6;
const int WHITEPLAYER = 1;
const int REDPLAYER = 2;
const string playerString[] = { "", "White", "Red" };

int otherPlayer(int player);
int whoIsPiece(int piece);
int getXFromSqNum(int sqNum, int numRowsInBoard);
int getYFromSqNum(int sqNum, int numRowsInBoard);
bool isKing(int piece);
int circularIndex(int index, int numRowsInBoard);
int getSquareNumber(int xCoordinate, int yCoordinate, int numRowsInBoard);
void InitializeBoard(int CMCheckersBoard[MAX_ARRAY_SIZE][MAX_ARRAY_SIZE], int numRowsInBoard);
void DisplayBoard(int CMCheckersBoard[MAX_ARRAY_SIZE][MAX_ARRAY_SIZE], int numRowsInBoard);
bool MakeMove(int CMCheckersBoard[MAX_ARRAY_SIZE][MAX_ARRAY_SIZE], int numRowsInBoard, int player, int fromSquareNum, int toSquareNum, bool &jumped);
bool IsJump(int CMCheckersBoard[MAX_ARRAY_SIZE][MAX_ARRAY_SIZE], int numRowsInBoard, int player, int xLoc, int yLoc);
bool IsMove1Square(int CMCheckersBoard[MAX_ARRAY_SIZE][MAX_ARRAY_SIZE], int numRowsInBoard, int player, int xLoc, int yLoc);
int CountJumps(int CMCheckersBoard[MAX_ARRAY_SIZE][MAX_ARRAY_SIZE], int numRowsInBoard, int player, int xLocArray[], int yLocArray[]);
int CountMove1Squares(int CMCheckersBoard[MAX_ARRAY_SIZE][MAX_ARRAY_SIZE], int numRowsInBoard, int player, int xLocArray[], int yLocArray[]);
bool CheckWin(int CMCheckersBoard[MAX_ARRAY_SIZE][MAX_ARRAY_SIZE], int numRowsInBoard);
bool CheckList(int inArray1[], int inArray2[], int xIndex, int yIndex);

//MAIN FUNCTION
int main()
{
	int myCMCheckersBoard[MAX_ARRAY_SIZE][MAX_ARRAY_SIZE] = { 0 };
	int xIndicesMove[MAX_PIECES] = { 0 };
	int yIndicesMove[MAX_PIECES] = { 0 };
	int xIndicesJump[MAX_PIECES] = { 0 };
	int yIndicesJump[MAX_PIECES] = { 0 };
	int numRowsInBoard = 0;
	int initialSquare = 0;
	int xInitialSquare = 0;
	int yInitialSquare = 0;
	int finalSquare = 0;
	int xFinalSquare = 0;
	int yFinalSquare = 0;
	int player = WHITEPLAYER;
	int numJumps = 0;
	int numMove1Squares = 0;
	bool jumped;
	int winner = NOONEWINS;
	char dummy = 0;

	//Asks for board size a maximum of 3x, then ends the function if an invalid board size is input all 3 times
	while (1)
	{
		cout << "Enter the number of squares along each edge of the board" << endl;
		if (!(cin >> numRowsInBoard))
		{
			cerr << "ERROR: Board size is not an integer." << endl << "8 <= number of squares <= 18" << endl;
			cin.clear();
			cin.ignore(1000, '\n');
		}
		else if (numRowsInBoard % 2 == 1)
			cerr << "ERROR: Board size odd." << endl << "8 <= number of squares <= 18" << endl;
		else if (numRowsInBoard > MAX_ARRAY_SIZE)
			cerr << "ERROR: Board size too large." << endl << "8 <= number of squares <= 18" << endl;
		else if (numRowsInBoard < MIN_ARRAY_SIZE)
			cerr << "ERROR: Board size too small." << endl << "8 <= number of squares <= 18" << endl;
		else
			break;
		if (++dummy == 3)
		{
			cerr << "ERROR: Too many errors entering the size of the board.";
			cin.get();
			return -1;
		}
	}

	//Initializes and displays the starting board
	InitializeBoard(myCMCheckersBoard, numRowsInBoard);
	DisplayBoard(myCMCheckersBoard, numRowsInBoard);
	
	//Main repeated chunk, each player alternates turns
	while (!winner)
	{
		fill_n(xIndicesJump, MAX_PIECES, -1);
		fill_n(yIndicesJump, MAX_PIECES, -1);
		fill_n(xIndicesMove, MAX_PIECES, -1);
		fill_n(yIndicesMove, MAX_PIECES, -1);

		jumped = false;
		numJumps = CountJumps(myCMCheckersBoard, numRowsInBoard, player, xIndicesJump, yIndicesJump);
		numMove1Squares = CountMove1Squares(myCMCheckersBoard, numRowsInBoard, player, xIndicesMove, yIndicesMove);

		if (!numJumps && !numMove1Squares) //If there are no more jumps for a certain player, the game ends
		{
			cout << playerString[player] << " is unable to move.\n" << "GAME OVER, " << playerString[otherPlayer(player)] << " has won.\n" << "Enter any character to close the game.\n";
			cin >> dummy;
			return otherPlayer(player);
		}
		cout << "\n\n" << playerString[player] << " takes a turn.\n";
		while (true) //This section makes sure the selected number has a valid piece in play
		{
			cout << "Enter the square number of the checker you want to move.\n";
			if (!(cin >> initialSquare))
			{
				cerr << "ERROR: You did not enter an integer\nTry again\n";
				cin.clear();
				cin.ignore(1000, '\n');
				continue;
			}
			else if (initialSquare < 0 || initialSquare >= (numRowsInBoard * numRowsInBoard)) //Checks to makes sure the selected number is in the board
			{
				cerr << "ERROR: That square is not on the board.\nTry again\n";
				continue;
			}
			xInitialSquare = getXFromSqNum(initialSquare, numRowsInBoard); //Getting the x value of the piece
			yInitialSquare = getYFromSqNum(initialSquare, numRowsInBoard); //Getting the y value of the piece
			if (whoIsPiece(myCMCheckersBoard[yInitialSquare][xInitialSquare]) == otherPlayer(player))
			{
				cerr << "ERROR: That square contains an opponent's checker.\nTry again\n";
				continue;
			}
			else if (myCMCheckersBoard[yInitialSquare][xInitialSquare] == NOPLAYER)
			{
				cerr << "ERROR: That square is empty.\nTry again\n";
				continue;
			}
			else if (!CheckList(xIndicesJump, yIndicesJump, xInitialSquare, yInitialSquare))
			{
				if (numJumps)
				{
					cerr << "ERROR: You can jump with another checker, you may not move your chosen checker.\nYou can jump with checkers on the following squares:";
					for (int i = 0; i < MAX_PIECES; i++)
					{
						if (xIndicesJump[i] == -1 && yIndicesJump[i] == -1)
							break;
						cerr << " " << getSquareNumber(xIndicesJump[i], yIndicesJump[i], numRowsInBoard);
					}
					cerr << "\nTry again\n";
					continue;
				}
				else if (!CheckList(xIndicesMove, yIndicesMove, xInitialSquare, yInitialSquare))
				{
					cerr << "ERROR: There is no legal move for this checker.\nTry again\n";
					continue;
				}
			}
			break;
		}

		while (true) //This section makes sure the selected jump is legal
		{
			cout << "Enter the square number of the square you want to move your checker to." << endl;
			if (!(cin >> finalSquare))
			{
				cerr << "ERROR: You did not enter an integer" << endl << "Try again" << endl;
				cin.clear();
				cin.ignore(1000, '\n');
				continue;
			}
			else if (finalSquare < 0 || finalSquare >= (numRowsInBoard * numRowsInBoard))
			{
				cerr << "ERROR: It is not possible to move to a square that is not on the board." << endl << "Try again" << endl;
				continue;
			}
			xFinalSquare = getXFromSqNum(finalSquare, numRowsInBoard);
			yFinalSquare = getYFromSqNum(finalSquare, numRowsInBoard);
			if (myCMCheckersBoard[yFinalSquare][xFinalSquare] != NOPLAYER)
			{
				cerr << "ERROR: It is not possible to move to a square that is already occupied." << endl << "Try again" << endl;
				continue;
			}
			else if (IsJump(myCMCheckersBoard, numRowsInBoard, player, xInitialSquare, yInitialSquare) && abs(xFinalSquare - xInitialSquare) == 1 && abs(yFinalSquare - yInitialSquare) == 1)
			{
				cerr << "ERROR: You can jump with this checker, you must jump not move 1 space." << endl << "Try again" << endl;
				continue;
			}
			else if (!MakeMove(myCMCheckersBoard, numRowsInBoard, player, initialSquare, finalSquare, jumped))
			{
				cerr << "ERROR: Moving to that square is not legal, Try again." << endl;
				continue;
			}
			if (jumped)
			{
				if (!IsJump(myCMCheckersBoard, numRowsInBoard, player, xFinalSquare, yFinalSquare))
					break;
				DisplayBoard(myCMCheckersBoard, numRowsInBoard);
				initialSquare = finalSquare;
				xInitialSquare = xFinalSquare;
				yInitialSquare = yFinalSquare;
				cout << "You can jump again, Please enter the next square you wish to move your checker to." << endl;
				continue;
			}
			else
				break;
		}
		if (!CheckWin(myCMCheckersBoard, numRowsInBoard))
		{
			DisplayBoard(myCMCheckersBoard, numRowsInBoard);
			player = otherPlayer(player);
			continue;
		}
		cout << "Enter any character to terminate the game then press the enter key" << endl << "The program will terminate after the player presses the enter key" << endl;
		cin >> dummy;
		break;
	}
	return 0;
}

/* otherPlayer Funcion:
* Purpose: To return the player that was not passed
* Pre: The current player
* Post: The other player's number is returned
*/
int otherPlayer(int player)
{
	return 3 - player;
}

/* whoIsPiece Funcion:
* Purpose: To determine which player a piece belongs to
* Pre: The piece in question
* Post: If the piece is a red player piece, it returns the red player number
*		If the piece is a white player piece, it returns the white player number
*/
int whoIsPiece(int piece)
{
	if (piece == WHITEKING || piece == WHITEMULE || piece == WHITESOLDIER)
		return WHITEPLAYER;
	if (piece == REDKING || piece == REDMULE || piece == REDSOLDIER)
		return REDPLAYER;
	return 0;
}

int getXFromSqNum(int sqNum, int numRowsInBoard) { return sqNum % numRowsInBoard; }

int getYFromSqNum(int sqNum, int numRowsInBoard) { return sqNum / numRowsInBoard; }

/* isKing Funcion:
* Purpose: To determine if the piece passed is a king
* Pre: The piece in question
* Post: If the peice is either king, return true, otherwise return false
*/
bool isKing(int piece)
{
	if (piece == WHITEKING || piece == REDKING) return true;
	return false;
}

/* circularIndex Funcion:
* Purpose: Alters the index provided to 'wrap' wrap around the board. Values that jump over the edge are placed on the other side
* Pre: The index/position of the jump in question, the number of rows/columns in the board
* Post: If the jump is greater than the max board value, the index is set back to the lower side of the board
*       If the jump is less than the lowest board value, the index is set bakc to the higher side of the board
*       If neither jumps result in a jump over the edge of the board, the function just return the original index 
*/
int circularIndex(int index, int numRowsInBoard)
{
	if (index >= numRowsInBoard)
	{
		do
		{
			index -= numRowsInBoard;
		} while (index >= numRowsInBoard);
	}
	if (index < 0)
	{
		do
		{
			index += numRowsInBoard;
		} while (index < 0);
	}
	return index;
}

int getSquareNumber(int xCoordinate, int yCoordinate, int numRowsInBoard)
{
	int square = 0;
	square = yCoordinate * numRowsInBoard + xCoordinate;
	return square;
}

/* InitializeBoard Funcion:
* Purpose: Sets the starting positions for all pieces
* Pre: The game board array containing the max board size values, the number of rows/columns in the board
* Post: Sets the the 1st row odd values to 1 (WHITEMULE)
*       Sets the 2nd row even values and 3rd row odd values to 2 (WHITESOLDIER)
*       Sets the 3rd to last even values and 2nd to last odd values to 4 (REDSOLDIER)
*      Sets the last row even values to 5 (REDMULE)
*/
void InitializeBoard(int CMCheckersBoard[MAX_ARRAY_SIZE][MAX_ARRAY_SIZE], int numRowsInBoard)
{
	int row = 0;
	int column = 0;
	int counter = 0;
	//White players
	for (row = 0, column = 0; column <= numRowsInBoard - 1; column++)
	{
		if (column % 2 != 0) CMCheckersBoard[row][column] = WHITEMULE;
	}
	for (counter = 1; counter < (numRowsInBoard / 2) - 2; counter += 2)
	{
		for (row = counter, column = 0; column <= numRowsInBoard - 1; column++)
		{
			if (column % 2) CMCheckersBoard[row + 1][column] = WHITESOLDIER;
			else CMCheckersBoard[row][column] = WHITESOLDIER;
		}
	}
	if ((numRowsInBoard / 2) % 2)
	{
		for (row = counter, column = 0; column <= numRowsInBoard - 1; column += 2)
		{
			CMCheckersBoard[row][column] = WHITESOLDIER;
		}
	}
	//Red players
	for (row = numRowsInBoard - 1, column = 0; column <= numRowsInBoard - 1; column++)
	{
		if (column % 2 == 0) CMCheckersBoard[row][column] = REDMULE;
	}
	for (counter = numRowsInBoard - 2; counter > (numRowsInBoard / 2) + 1; counter -= 2)
	{
		for (row = counter, column = 0; column <= numRowsInBoard - 1; column++)
		{
			if (column % 2) CMCheckersBoard[row][column] = REDSOLDIER;
			else CMCheckersBoard[row - 1][column] = REDSOLDIER;
		}
	}
	if ((numRowsInBoard / 2) % 2 == 1)
	{
		for (row = counter, column = 1; column <= numRowsInBoard - 1; column += 2)
		{
			CMCheckersBoard[row][column] = REDSOLDIER;
		}
	}
}

/* DisplayBoard Function:
* Purpose: Displays the current board to the console
* Pre: The game board array containing the max board size values, the number of rows/columns in the board
* Post: Displays Board to console, does not change any values
*/
void DisplayBoard(int CMCheckersBoard[MAX_ARRAY_SIZE][MAX_ARRAY_SIZE], int numRowsInBoard)
{
	int SQUARENUMBER = 0;
	const string PIECE_NAMES[] = { "", "WS", "WM", "WK", "RS", "RM", "RK" };
	for (int i = 0; i < numRowsInBoard; i++)
	{
		for (int k = 0; k < numRowsInBoard; k++)
		{
			cout << setw(4);
			if (CMCheckersBoard[i][k]) cout << PIECE_NAMES[CMCheckersBoard[i][k]];
			else cout << (i * numRowsInBoard) + k;
		}
		cout << endl;
	}
}

/* MakeMove Function:
* Purpose: Moves the piece selected, prints an error message if an incorrect move is made
* Pre: The game board array containing the max board size values, the number of rows/columns in the board,
* the player number, the current position square, the target position square, boolean to check if it moved
* Post: Displays Board to console, does not change any values
*/
bool MakeMove(int CMCheckersBoard[MAX_ARRAY_SIZE][MAX_ARRAY_SIZE], int numRowsInBoard, int player, int fromSquareNum, int toSquareNum, bool &jumped)
{
	jumped = false;
	int from_x_square_number = getXFromSqNum(fromSquareNum, numRowsInBoard);
	int from_y_square_number = getYFromSqNum(fromSquareNum, numRowsInBoard);
	int to_x_square_number = getXFromSqNum(toSquareNum, numRowsInBoard);
	int to_y_square_number = getYFromSqNum(toSquareNum, numRowsInBoard);

	int xDistance = abs(from_x_square_number - to_x_square_number);
	int dy = to_y_square_number - from_y_square_number;
	int yDistance = abs(from_y_square_number - to_y_square_number);
	int numCheckersInRow = numRowsInBoard;

	if ((from_x_square_number == 0 && to_x_square_number == (numCheckersInRow - 1)) || (from_x_square_number == (numCheckersInRow - 1) && to_x_square_number == 0))
		xDistance = 1;
	if ((from_x_square_number == 0 && to_x_square_number == (numCheckersInRow - 2)) || (from_x_square_number == (numCheckersInRow - 2) && to_x_square_number == 0))
		xDistance = 2;
	if ((from_x_square_number == 1 && to_x_square_number == (numCheckersInRow - 1)) || (from_x_square_number == (numCheckersInRow - 1) && to_x_square_number == 1))
		xDistance = 2;
	if (player == REDPLAYER)
		dy = -dy;
	if (dy < 0 && !isKing(CMCheckersBoard[from_y_square_number][from_x_square_number]))
	{
		cerr << "ERROR: Illegal move.\n";
		return false;
	}
	if (abs(yDistance) != abs(xDistance))
	{
		cerr << "ERROR: Illegal move\n";
		return false;
	}
	else if (abs(yDistance) > 2)
	{
		cerr << "ERROR: Illegal move\n";
		return false;
	}
	else if (abs(yDistance) == 2)
	{
		if (CMCheckersBoard[to_y_square_number][to_x_square_number] == 0)
		{
			int middleX;
			if (abs(to_x_square_number - from_x_square_number) < 2)
			{
				cerr << "ERROR: Illegal move\n";
				return false;
			}
			if (from_x_square_number == 0)
				middleX = numRowsInBoard - 1;
			else if ((from_x_square_number == 1 && to_x_square_number == (numRowsInBoard - 1)) || (from_x_square_number == (numRowsInBoard - 1) && to_x_square_number == 1))
				middleX = 0;
			else if (from_x_square_number == (numRowsInBoard - 2))
				middleX = numRowsInBoard - 1;
			else
				middleX = (from_x_square_number + to_x_square_number) / 2;
			int middleY = (from_y_square_number + to_y_square_number) / 2;
			if (whoIsPiece(CMCheckersBoard[middleY][middleX]) == otherPlayer(player))
			{
				CMCheckersBoard[middleY][middleX] = 0;
				CMCheckersBoard[to_y_square_number][to_x_square_number] = CMCheckersBoard[from_y_square_number][from_x_square_number];
				CMCheckersBoard[from_y_square_number][from_x_square_number] = 0;
			}
			else
			{
				cerr << "ERROR: Illegal move.\n";
				return false;
			}
			jumped = true;
		}
	}
	else if (abs(yDistance) == 1)
	{
		CMCheckersBoard[to_y_square_number][to_x_square_number] = CMCheckersBoard[from_y_square_number][from_x_square_number];
		CMCheckersBoard[from_y_square_number][from_x_square_number] = 0;
	}
	else if (yDistance == 0)
	{
		return false;
	}
	else
	{
		cerr << "ERROR: Illegal move\n";
		return false;
	}
	if (player == WHITEPLAYER)
	{
		if (to_y_square_number == numRowsInBoard - 1 && CMCheckersBoard[to_y_square_number][to_x_square_number] == WHITEMULE)
		{
			cout << "White has created a Mule King, Red wins the game\n";
			cout << "Enter any character to terminate the game then press the enter key\n";
			cout << "The program will terminate after the player presses the enter key.\n";
			int dummy;
			cin >> dummy;
			exit(0);
		}
		if (to_y_square_number == numRowsInBoard - 1 && CMCheckersBoard[to_y_square_number][to_x_square_number] == WHITESOLDIER)
			CMCheckersBoard[to_y_square_number][to_x_square_number] = WHITEKING;
	}
	if (player == REDPLAYER)
	{
		if (to_y_square_number == 0 && CMCheckersBoard[to_y_square_number][to_x_square_number] == REDMULE)
		{
			cout << "Red has created a Mule King, White wins the game\n";
			cout << "Enter any character to terminate the game then press the enter key\n";
			cout << "The program will terminate after the player presses the enter key.\n";
			int dummy;
			cin >> dummy;
			exit(0);
		}
		if (to_y_square_number == 0 && CMCheckersBoard[to_y_square_number][to_x_square_number] == REDSOLDIER)
			CMCheckersBoard[to_y_square_number][to_x_square_number] = REDKING;
	}
	return true;
}

/* IsJump Function:
* Purpose: Retrives the values diagonal to the selected piece and returns true is a move can be made and false if not
* Pre: The game board array containing the max board size values, the number of rows/columns in the board,
* The player number, the X location, and the Y location
* Post: Returns true is a move can be made and false if not, values are not changed
*/
bool IsJump(int CMCheckersBoard[MAX_ARRAY_SIZE][MAX_ARRAY_SIZE], int numRowsInBoard, int player, int xLoc, int yLoc)
{
	int dir = 1;
	int i = 0;
	if (player == REDPLAYER) dir = -dir;

	if (whoIsPiece(CMCheckersBoard[yLoc][xLoc]) != player) return false;

	if (yLoc + abs(dir * 2) >= numRowsInBoard || yLoc - abs(dir * 2) <= 0) return false;

	for (i = 0; i < 2; i++)
	{
		if (whoIsPiece(CMCheckersBoard[yLoc + dir][circularIndex(xLoc + 1, numRowsInBoard)]) == otherPlayer(player) && CMCheckersBoard[yLoc + (2 * dir)][circularIndex(xLoc + 2, numRowsInBoard)] == 0)
			return true;
		if (whoIsPiece(CMCheckersBoard[yLoc + dir][circularIndex(xLoc - 1, numRowsInBoard)]) == otherPlayer(player) && (CMCheckersBoard[yLoc + (2 * dir)][circularIndex(xLoc - 2, numRowsInBoard)] == 0))
			return true;
		if (!isKing(CMCheckersBoard[yLoc][xLoc])) break;
		dir = -dir;
	}
	return false;
}

/* IsMove1Squares Function:
* Purpose: Determines if a piece can move 1 space diagonally
* Pre: The game board array containing the max board size values, the number of rows/columns,
* the X value, and the Y value, the player number
* Post: Returns true if there is no value on a diagonal piece, and if it is a king, also considers backwards cases
* otherwise returns false, no value are changed.
*/
bool IsMove1Square(int CMCheckersBoard[MAX_ARRAY_SIZE][MAX_ARRAY_SIZE], int numRowsInBoard, int player, int xLoc, int yLoc)
{
	int i;
	int dir = 1;
	// if player is red then reverse the direction
	if (player == REDPLAYER) dir = -1;

	for (i = 0; i < 2; i++)
	{
		if (CMCheckersBoard[yLoc + dir][circularIndex(xLoc + 1, numRowsInBoard)] == 0)
			return true;
		if (CMCheckersBoard[yLoc + dir][circularIndex(xLoc - 1, numRowsInBoard)] == 0)
			return true;
		if (!isKing(CMCheckersBoard[yLoc][xLoc]))
			break;
		dir = -dir;
	}
	return false;
}

/* CountJumps Function:
* Purpose: Determine how many jumps are avaliable to each players pieces
* Pre: The game board array containing the max board size values, number of rows/columns in the board,
* The player number, The X values array, the Y values array
* Post: Returns the number of jumps avaliable and stores all X and Y values of moveable peices into the 2 arrays
*/
int CountJumps(int CMCheckersBoard[MAX_ARRAY_SIZE][MAX_ARRAY_SIZE], int numRowsInBoard, int player, int xLocArray[], int yLocArray[])
{
	int arrayCounter = 0;
	int xIndex, yIndex;
	fill_n(xLocArray, MAX_PIECES, -1);
	fill_n(yLocArray, MAX_PIECES, -1);
	for (xIndex = 0; xIndex < numRowsInBoard; xIndex++)
	{
		for (yIndex = 0; yIndex < numRowsInBoard; yIndex++)
		{
			if (IsJump(CMCheckersBoard, numRowsInBoard, player, xIndex, yIndex))
			{
				xLocArray[arrayCounter] = xIndex;
				yLocArray[arrayCounter++] = yIndex;
			}
		}
	}
	return arrayCounter;
}

/* CountMove1Squares Function:
* Purpose: Determines the number of single space moves each piece can make
* Pre: The game board array containing the max board size values, the number of rows/columns, Array1 which  
* contains the x indices, Array2 which contains the y indices, the X value, and the Y value, the player number
* Post: Returns true if the checker has X and Y values within the two arrays, otherwise returns false, no values are * changed.
*/
int CountMove1Squares(int CMCheckersBoard[MAX_ARRAY_SIZE][MAX_ARRAY_SIZE], int numRowsInBoard, int player, int xLocArray[], int yLocArray[])
{
	int Pcheckersmovable = 0;
	int xIndex;
	int yIndex;
	fill_n(xLocArray, MAX_PIECES, -1);
	fill_n(yLocArray, MAX_PIECES, -1);
	for (xIndex = 0; xIndex < numRowsInBoard; xIndex++)
	{
		for (yIndex = 0; yIndex < numRowsInBoard; yIndex++)
		{
			if (player == whoIsPiece(CMCheckersBoard[yIndex][xIndex]))
			{
				if (IsMove1Square(CMCheckersBoard, numRowsInBoard, player, xIndex, yIndex))
				{
					xLocArray[Pcheckersmovable] = xIndex;
					yLocArray[Pcheckersmovable++] = yIndex;
				}
			}
		}
	}
	return Pcheckersmovable;
}

/* CheckWin Function:
* Purpose: to determine if a player has won
* Pre: The game board array contain the max board values, the number of rows/columns in the board
* Post: Returns true if any of the win conditions are met
*/
bool CheckWin(int CMCheckersBoard[MAX_ARRAY_SIZE][MAX_ARRAY_SIZE], int numRowsInBoard)
{
	int column;
	int row;
	int numWhitePieces = 0;
	int numWhiteMules = 0;
	int numRedMules = 0;
	int numRedPieces = 0;
	for (row = 0; row < numRowsInBoard; row++)
	{
		for (column = 0; column < numRowsInBoard; column++)
		{
			if (!CMCheckersBoard[row][column])
				continue;
			else if (whoIsPiece(CMCheckersBoard[row][column]) == WHITEPLAYER)
			{
				numWhitePieces++;
				if (CMCheckersBoard[row][column] == WHITEMULE)
					numWhiteMules++;
			}
			else
			{
				numRedPieces++;
				if (CMCheckersBoard[row][column] == REDMULE)
					numRedMules++;
			}
		}
	}
	if (!numWhiteMules)
	{
		cout << "The White Player has won the game by losing all of the White Mules" << endl;
		return true;
	}
	if (numWhiteMules == numWhitePieces)
	{
		cout << "The Red Player has won by capturing all of the white players soldiers and kings" << endl;
		return true;

	}
	if (!numRedMules)
	{
		cout << "The Red Player has won the game by losing all of the red mules" << endl;
		return true;
	}
	if (numRedMules == numRedPieces)
	{
		cout << "The White Player has won by capturing all of the red players soldiers and kings" << endl;
		return true;
	}
	return false;
}

/* CheckList Function:
* Purpose: Determines if the selected checker has x and y values within the two arrays
* Pre: Array1 which contains the x indices, Array2 which contains the y indices, the X value, and the Y value
* Post: Returns true if the checker has X and Y values within the two arrays, otherwise returns false, no value are changed.
*/
bool CheckList(int inArray1[], int inArray2[], int xIndex, int yIndex)
{
	int i;
	int j;
	for (i = 0; i < MAX_PIECES; i++)
	{
		if (inArray2[i] == yIndex)
		{
			for (j = i; j < MAX_PIECES; j++)
			{
				if (inArray1[j] == xIndex && inArray2[j] == yIndex) return true;
			}
		}
		else if (inArray2[i] > yIndex) return false;
	}
	return false;
}