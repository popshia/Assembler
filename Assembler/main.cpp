//  System Program Homework part 1
//  Lexical Analysis ( x86 )
//  10627130 資訊三甲 林冠良
//  12/06 23:59 DEADLINE

#include <cctype>
#include <cstdio>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <cstring>
#include <string>
#include <string.h>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <bitset>

using namespace std;

//static int Command;
static ifstream Input;
static ofstream Output;

#define OPCODE 1
#define PSEUDO 2
#define REGISTER 3
#define DELIMETER 4
#define SYMBOL 5
#define INTEGER 6
#define STRING 7

typedef struct TokenData {
	int tokenType = 0;
	int tableNum = 0;
	string token = "\0";
	string machineCode = "\0";
} TokenData;

class Assembler {
public:
	vector<TokenData> opCodeTable;                // 1 ok
	vector<TokenData> pseudoInstructionsTable;    // 2 ok
	vector<TokenData> registerOrAccumulatorTable; // 3 ok
	vector<TokenData> delimetersTable;            // 4 ok
	vector<TokenData> symbolHashTable;            // 5 ok
	vector<TokenData> intHashTable;               // 6 ok
	vector<TokenData> stringHashTable;            // 7 ok
	vector<TokenData> commentTable;               // 8 ok
	vector<TokenData> currentLineTokens;
	vector<string> currentMachineCode;
	int lineNum = 0; // setw( 4 ) << setfill( '0' ) << hex << uppercase

	void Run() { GetToken(); } // start function
	
	void Go() { binaryToHexdacimal(); }

	void inputTable() {
		int tableNum = 1;
		string inputTableName = "\0";
		for ( int i = 1; i <= 4; i++ ) {
			inputTableName = "Table" + to_string( i ) + ".table";
			Input.open( inputTableName );
			while ( !Input.eof() ) {
				TokenData singleToken;
				singleToken.tableNum = tableNum;
				Input >> singleToken.token;
				singleToken.tokenType = i;
				if ( i == 1 ) opCodeTable.push_back( singleToken );
				if ( i == 2 ) pseudoInstructionsTable.push_back( singleToken );
				if ( i == 3 ) {
					if ( tableNum == 1 ) singleToken.machineCode = "100";
					else if ( tableNum == 2 ) singleToken.machineCode = "000";
					else if ( tableNum == 3 ) singleToken.machineCode = "000";
					else if ( tableNum == 4 ) singleToken.machineCode = "111";
					else if ( tableNum == 5 ) singleToken.machineCode = "011";
					else if ( tableNum == 6 ) singleToken.machineCode = "011";
					else if ( tableNum == 7 ) singleToken.machineCode = "101";
					else if ( tableNum == 8 ) singleToken.machineCode = "001";
					else if ( tableNum == 9 ) singleToken.machineCode = "001";
					else if ( tableNum == 10 ) singleToken.machineCode = "110";
					else if ( tableNum == 11 ) singleToken.machineCode = "010";
					else if ( tableNum == 12 ) singleToken.machineCode = "010";
					else if ( tableNum == 13 ) singleToken.machineCode = "100";
					else if ( tableNum == 14 ) singleToken.machineCode = "101";
					else if ( tableNum == 15 ) singleToken.machineCode = "110";
					else if ( tableNum == 16 ) singleToken.machineCode = "111";
					else if ( tableNum == 17 ) singleToken.machineCode = "00";
					else if ( tableNum == 18 ) singleToken.machineCode = "01";
					else if ( tableNum == 19 ) singleToken.machineCode = "10";
					else if ( tableNum == 20 ) singleToken.machineCode = "11";
					registerOrAccumulatorTable.push_back( singleToken );
				} // registers
				if ( i == 4 ) delimetersTable.push_back( singleToken );
				// cout << i << " " << singleToken.tableNum << " " << singleToken.token << endl;
				tableNum++;
			} // input & push
			tableNum = 1;
			Input.close();
		} // read and save
	} // input tables
private:
	bool IsDelimeter( char ch, TokenData& delimeterToken ) {
		string check( 1, ch );
		for ( int i = 0; i < delimetersTable.size(); i++ ) {
			if ( check == delimetersTable[i].token ) {
				delimeterToken = delimetersTable[i];
				return true;
			} // find
		} // check the table
		return false;
	} // check if the char is delimeter

	void GetToken() {
		currentLineTokens.clear();
		string wholeLine = "\0";
		string singleToken;
		string comment;
		int commentIndex = 1;
		TokenData commentToken;
		TokenData delimeterToken;
		while ( getline( Input, wholeLine ) ) {
			cout << setw( 4 ) << setfill( '0' ) << hex << uppercase << lineNum << "\t" << wholeLine << endl;
			Output << wholeLine << endl;
			for ( int i = 0; i < wholeLine.length(); i++ ) {
				if ( ( wholeLine[i] == '\'' && currentLineTokens[currentLineTokens.size() - 1].token == "'" ) || ( IsDelimeter( wholeLine[i], delimeterToken ) && currentLineTokens[currentLineTokens.size() - 1].token != "'" ) || ( wholeLine[i] == ' ' ) || ( wholeLine[i] == '\n' ) || ( wholeLine[i] == '\t' ) || ( wholeLine[i] == ';' ) || ( wholeLine[i] == '\r' ) || ( wholeLine[i] == '\0' ) ) {
					if ( !singleToken.empty() ) {
						//cout << singleToken << endl;
						Identify( singleToken, wholeLine[i] );
						singleToken.clear();
					} // identify
					if ( IsDelimeter( wholeLine[i], delimeterToken ) ) {
						currentLineTokens.push_back( delimeterToken );
						if ( wholeLine[i] == ';' ) {
							for ( int k = 0; k < wholeLine.length(); k++ )
								comment.push_back( wholeLine[i] );
							commentToken.tokenType = 8;
							commentToken.tableNum = commentIndex;
							commentToken.token = comment;
							commentTable.push_back( commentToken );
							currentLineTokens.push_back( commentToken );
							commentIndex++;
							break;
						} // comment case
					} // delimeter
				} // non-character case
				else {
					singleToken.push_back( wholeLine[i] );
					if ( i == wholeLine.length() - 1 && !singleToken.empty() ) {
						singleToken.erase( remove( singleToken.begin(), singleToken.end(), '\r' ), singleToken.end() );
						singleToken.erase( remove( singleToken.begin(), singleToken.end(), '\n' ), singleToken.end() );
						Identify( singleToken, wholeLine[i] );
						singleToken.clear();
					} // end of file
				} // character case
			} // read single line
			SyntaxAnalyze();
			for ( int i = 0; i < currentLineTokens.size(); i++ ) Output << "(" << currentLineTokens[i].tokenType << "," << currentLineTokens[i].tableNum << ")";
			Output << endl;
			currentLineTokens.clear();
		} // while not EOF
	} // cut the tokens

	void Identify( string singleToken, char currentChar ) {
		string save = singleToken;
		int ASCII = 0;
		int indexPlus = 0;
		if ( currentChar == '\'' ) {
			stringHashTable.resize( 100 );
			indexPlus = 0;
			TokenData stringToken;
			for ( int i = 0; i < singleToken.size(); i++ )
				ASCII += (int)singleToken[i];
			while ( stringHashTable[ASCII % 100 + indexPlus].token != "\0" && stringHashTable[ASCII % 100 + indexPlus].token != singleToken ) {
				indexPlus += 1;
				if ( ( ASCII % 100 + indexPlus ) > 99 ) indexPlus = -( ASCII % 100 );
			} // hashing
			stringToken.tokenType = 7;
			stringToken.tableNum = ASCII % 100 + indexPlus;
			stringToken.token = singleToken;
			stringHashTable[ASCII % 100 + indexPlus] = stringToken;
			currentLineTokens.push_back( stringToken );
			return;
		} // string case
		else if ( singleToken[singleToken.size() - 1] == 'H' && singleToken != "AH" && singleToken != "BH" && singleToken != "CH" && singleToken != "DH" ) {
			intHashTable.resize( 100 );
			int i = 0;
			for ( i = 0; isxdigit( singleToken[i] ); i++ )
				ASCII += (int)singleToken[i];
			ASCII += 72;
			if ( i == singleToken.size() - 1 ) {
				TokenData intToken;
				while ( intHashTable[ASCII % 100 + indexPlus].token != "\0" && intHashTable[ASCII % 100 + indexPlus].token != singleToken ) {
					indexPlus += 1;
					if ( ( ASCII % 100 + indexPlus ) > 99 ) indexPlus = -( ASCII % 100 );
				} // hashing
				intToken.tokenType = 6;
				intToken.tableNum = ASCII % 100 + indexPlus;
				intToken.token = singleToken;
				intHashTable[ASCII % 100 + indexPlus] = intToken;
				currentLineTokens.push_back( intToken );
				return;
			} // push to current
		} // check if the token is an integer
		for ( int i = 0; i < singleToken.size(); i++ ) singleToken[i] = toupper( singleToken[i] );
		for ( int i = 0; i < opCodeTable.size(); i++ ) {
			if ( singleToken == opCodeTable[i].token ) {
				currentLineTokens.push_back( opCodeTable[i] );
				return;
			} // found
		} // opCode
		for ( int i = 0; i < pseudoInstructionsTable.size(); i++ ) {
			if ( singleToken == pseudoInstructionsTable[i].token ) {
				currentLineTokens.push_back( pseudoInstructionsTable[i] );
				return;
			} // found
		} // pseudoInstructions
		for ( int i = 0; i < registerOrAccumulatorTable.size(); i++ ) {
			if ( singleToken == registerOrAccumulatorTable[i].token ) {
				currentLineTokens.push_back( registerOrAccumulatorTable[i] );
				return;
			} // found
		} // registerOrAccumulator
		ASCII = 0;
		singleToken = save;
		symbolHashTable.resize( 100 );
		indexPlus = 0;
		TokenData symbolToken;
		for ( int i = 0; i < singleToken.size(); i++ )
			ASCII += (int)singleToken[i];
		while ( symbolHashTable[ASCII % 100 + indexPlus].token != "\0" && symbolHashTable[ASCII % 100 + indexPlus].token != singleToken ) {
			indexPlus += 1;
			if ( ( ASCII % 100 + indexPlus ) > 99 ) indexPlus = -( ASCII % 100 );
		} // hashing
		symbolToken.tokenType = 5;
		symbolToken.tableNum = ASCII % 100 + indexPlus;
		symbolToken.token = singleToken;
		symbolHashTable[ASCII % 100 + indexPlus] = symbolToken;
		currentLineTokens.push_back( symbolToken );
		return;
	} // Identify()

	void SyntaxAnalyze() {

	} // SyntaxAnalyze()
	
	void binaryToHexdacimal() {
		string binary = "1111000010100010"; // F0 A2
		string returnString = "\0";
		string oneNibble = "\0";
		for ( int i = 0; i <= binary.size()-8; i+=8 ) {
			oneNibble += binary[i];
			oneNibble += binary[i+1];
			oneNibble += binary[i+2];
			oneNibble += binary[i+3];
			oneNibble += binary[i+4];
			oneNibble += binary[i+5];
			oneNibble += binary[i+6];
			oneNibble += binary[i+7];
			bitset<8> output(oneNibble);
			cout << hex << uppercase << output.to_ulong() << " ";
			oneNibble = "\0";
		} // get four nibbles
		cout << endl;
	} // convert binary to hexdacimal

	string BYTE() {
		string returnString;
		for ( int i = 1; i < currentLineTokens.size(); i++ ) {
			for ( int k = 0; k < currentLineTokens[i].token.size(); k++ ) {
				returnString += currentLineTokens[i].token[k];
			} // collect all char in each string
		} // run all string after the "BYTE"
		return returnString; // already in hexadicimal
	} // byte case

	string AAD() { return "1101010100001010"; }
	string AAM() { return "1101010000001010"; }
	string AAA() { return "00110111"; }
	string AAS() { return "00111111"; }
	string CBW() { return "10011000"; }
	string CLC() { return "11111000"; }
	string CLD() { return "11111100"; }
	string CLI() { return "11111010"; }
	string CMC() { return "11110101"; }
	string CMPSB() { return "10100110"; }
	string CMPSW() { return "10100111"; }
	string CWD() { return "10011001"; }
	string DAA() { return "00100111"; }
	string DAS() { return "00101111"; }
	string FWAIT() { return "10011011"; }
	string HLT() { return "11110100"; }
	string INTO() { return "11001110"; }
	string IRET() { return "11001111"; }
	string LAHF() { return "10011111"; }
	string LOCK() { return "11110000"; }
	string LODSB() { return "10101100"; }
	string LODSW() { return "10101101"; }
	string MOVSB() { return "10100100"; }
	string MOVSW() { return "10100101"; }
	string NOP() { return "10010000"; }
	string POPF() { return "10011110"; }
	string PUSHF() { return "10011101"; }
	string REP() { return "11110011"; }
	string REPE() { return "11110011"; }
	string REPNE() { return "11110010"; }
	string REPNZ() { return "11110010"; }
	string REPZ() { return "11110011"; }
	string SAHF() { return "10011110"; }
	string SCASB() { return "10101110"; }
	string SCASW() { return "10101110"; }
	string STC() { return "11111001"; }
	string STD() { return "11111110"; }
	string STI() { return "11111011"; }
	string STOSB() { return "10101010"; }
	string STOSW() { return "10101011"; }
	string WAIT() { return "10011011"; }
	string XLAT() { return "11100111"; }

	string PUSH() {
		if ( currentLineTokens.size() == 2 ) return "01010reg";
		else return "11111111mod110rm";
	} // PUSH()

	string POP() {
		if ( currentLineTokens.size() == 2 ) return "01011reg";
		else return "10001111mod000rm";
	} // POP()

	string RET() { return "10100011"; }
	string RETF() { return ""; } // NULL

	string INT() {
		return "11001101" + currentLineTokens[1].token[0] + currentLineTokens[1].token[1];
		// ex. INT 21H
	} // INT()

	string IN() {
		if ( currentLineTokens[1].token == "AL" || currentLineTokens[1].token == "AX" ) return "1110010wport";
		else return "1110110w";
	} // IN()

	string OUT() {
		if ( currentLineTokens[1].token == "AL" || currentLineTokens[1].token == "AX" ) return "1110011wport";
		else return "1110110w";
	} // OUT()

	string JMP() { return ""; } // not in input

	string CALL() {
		return "11101000lohi"; // lo hi is the memory address of the label after "CALL"
	} // CALL()
	
	string JA() { return ""; } // not in input
	string JAE() { return ""; } // not in input
	string JB() { return ""; } // not in input
	string JBE() { return ""; } // not in input
	string JC() { return ""; } // not in input
	string JCXZ() { return ""; } // not in input
	string JE() { return ""; } // not in input
	string JG() { return ""; } // not in input
	string JGE() { return ""; } // not in input
	string JL() { return ""; } // not in input
	string JLE() { return ""; } // not in input
	string JNA() { return ""; } // not in input
	string JNAE() { return ""; } // not in input
	string JNB() { return ""; } // not in input
	string JNBE() { return ""; } // not in input
	string JNC() { return ""; } // not in input
	string JNE() { return ""; } // not in input
	string JNG() { return ""; } // not in input
	string JNGE() { return ""; } // not in input
	string JNL() { return ""; } // not in input
	string JNLE() { return ""; } // not in input
	string JNO() { return ""; } // not in input
	string JNP() { return ""; } // not in input
	string JNS() { return ""; } // not in input
	string JNZ() { return ""; } // not in input
	string JO() { return ""; } // not in input
	string JP() { return ""; } // not in input
	string JPE() { return ""; } // not in input
	string JPO() { return ""; } // not in input
	string JS() { return ""; } // not in input
	string JZ() { return ""; } // not in input
	string LOOP() { return ""; } // not in input
	string LOOPE() { return ""; } // not in input
	string LOOPNE() { return ""; } // not in input
	string LOOPNZ() { return ""; } // not in input
	string LOOPZ() { return ""; } // not in input
	string LDS() { return ""; } // not in input
	string LEA() { return ""; } // not in input
	string LES() { return ""; } // not in input
	string DEC() { return ""; } // not in input
	string INC() { return ""; } // not in input
	string ESC() { return ""; } // not in input
	string DIV() { return ""; } // not in input
	string IDIV() { return ""; } // not in input
	string IMUL() { return ""; } // not in input
	string MUL() { return ""; } // not in input
	string NEG() { return ""; } // not in input
	string NOT() { return ""; } // not in input
	string RCL() { return ""; } // not in input
	string RCR() { return ""; } // not in input
	string ROL() { return ""; } // not in input
	string ROR() { return ""; } // not in input
	string SAR() { return ""; } // not in input
	string SHL() { return ""; } // not in input
	string SHR() { return ""; } // not in input
	string XCHG() { return ""; } // not in input
	string TEST() { return ""; } // not in input

	string MOV() {
		if ( currentLineTokens[1].tokenType == REGISTER && currentLineTokens[2].tokenType == REGISTER ) {
			return "10001110110" + currentLineTokens[1].machineCode + currentLineTokens[2].machineCode;
		} // register to segment register ex. MOV DS,AX
		else { //if ( currentLineTokens[1].tokenType == REGISTER && currentLineTokens[2].tokenType == INTEGER ) {
			return "10110" + currentLineTokens[1].machineCode + currentLineTokens[2].token[0] + currentLineTokens[2].token[1];
		} // immediate to register ex. MOV AH,4CH
	} // MOV()

	string ADC() { return ""; } // not in input
	string ADD() { return ""; } // not in input
	string AND() { return ""; } // not in input
	string CMP() { return ""; } // not in input
	string OR() { return ""; } // not in input
	string SBB() { return ""; } // not in input
	string SUB() { return ""; } // not in input
	string XOR() { return ""; } // not in input

};

int main( int argc, const char* argv[] ) {
	int command = 0;
	bool continueOrNot = false;
	Assembler Start;
	//Start.Go();
	//return 0;
	Start.inputTable();
	string inputFile;
	do {
		cout << "*****************************************" << endl; // welcome message
		cout << "***** Lexical Analysis              *****" << endl;
		cout << "***** 0 : Quit                      *****" << endl;
		cout << "***** 1 : Cut Token                 *****" << endl;
		cout << "*****************************************" << endl;
		cout << endl << "Please enter your choice :" << endl;
		cin >> command;
		cout << endl;
		if ( command == 0 ) { // bye :(((
			cout << "Bye :(((" << endl;
			return 0;
		} // quit
		else if ( command > 1 || command < 0 ) { // wrong command
			cout << "Error command! please enter an acceptable command :" << endl << endl;
			continueOrNot = true;
		} // wrong command
		else if ( command == 1 ) { // cut token
			bool function1Confirm = false;
			do {
				cout << "Please enter the file you want to process ( without \".txt\" ) or [0] to quit:" << endl;
				cin >> inputFile;
				cout << endl;
				if ( inputFile == "0" ) {
					function1Confirm = true;
					continueOrNot = true;
				} // quit
				else {
					string inputFileName = inputFile + ".txt";
					Input.open( inputFileName );
					if ( Input.is_open() ) {
						cout << "Processing " << inputFileName << "..." << endl << endl;
						string outputFileName = inputFile + "_output.txt";
						Output.open( outputFileName );
						cout << "Output in " << outputFileName << "..." << endl << endl;
						Start.Run();
						cout << "Process complete :) Please check the output file..." << endl << endl;
						function1Confirm = true;
						continueOrNot = true;
					} // opened
					else cout << "*****  " << inputFileName << " does not exist!  *****" << endl; // no file
				} // open and sort
			} while ( !function1Confirm );
			inputFile = "0";
		} // mission 1: cut token
	} while ( continueOrNot ); // gogogo
	Input.close();
	Output.close();
} // main function
