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

static string FileName = "\0";
static ifstream Input;
static ofstream Output;

#define Instrucion 1
#define Pseudo 2
#define Register 3
#define Delimiter 4
#define Symbol 5
#define Integer 6
#define String 7

typedef struct Token {
    string tokenName;
    int table = 0;
    int value = 0;
    string machineCode;
} Token;

typedef struct Type {
    string tokenName;
    int type = 0;
} Type;

typedef struct Label {
    string labelName;
    string address = "0000";
} Label;

class Assembler {
public:
    vector<Token> InstrucionTable;
    vector<Token> PseudoTable;
    vector<Token> RegisterTable;
    vector<Token> DelimiterTable;
    Token SymbolTable[100];
    Token IntegerTable[100];
    Token StringTable[100];

    
    vector<Token> Tokens;
    vector<Token> currentLineTokens;
    vector<Label> LabelTable;
    vector<Type>  TypeTable;
    vector<Token> Operands;
    
    int lineNum = 0;
    int linePlus = 0;
    
    string wholeLine = "/0";

    void ReadTable() {
        string tempTable;
        Token NullData;
        Token tempData ;
		int tableNum = 1;
        
        for( int i = 1 ; i <= 4 ; i++ ) {
            string fileName = "Table" + to_string( i ) + ".table" ;
            Input.open( fileName.c_str() );
			while( !Input.eof() ) {
				tempData.value = tableNum;
                Input >> tempData.tokenName ;
                tempData.table = i;
                if( i == 1 ) InstrucionTable.push_back( tempData ) ;
                if( i == 2 ) PseudoTable.push_back( tempData ) ;
                if( i == 3 ) {
                    if ( tempData.value == 1 ) tempData.machineCode = "100";
                    else if ( tempData.value == 2 ) tempData.machineCode = "000";
                    else if ( tempData.value == 3 ) tempData.machineCode = "000";
                    else if ( tempData.value == 4 ) tempData.machineCode = "111";
                    else if ( tempData.value == 5 ) tempData.machineCode = "011";
                    else if ( tempData.value == 6 ) tempData.machineCode = "011";
                    else if ( tempData.value == 7 ) tempData.machineCode = "101";
                    else if ( tempData.value == 8 ) tempData.machineCode = "001";
                    else if ( tempData.value == 9 ) tempData.machineCode = "001";
                    else if ( tempData.value == 10 ) tempData.machineCode = "110";
                    else if ( tempData.value == 11 ) tempData.machineCode = "010";
                    else if ( tempData.value == 12 ) tempData.machineCode = "010";
                    else if ( tempData.value == 13 ) tempData.machineCode = "100";
                    else if ( tempData.value == 14 ) tempData.machineCode = "101";
                    else if ( tempData.value == 15 ) tempData.machineCode = "110";
                    else if ( tempData.value == 16 ) tempData.machineCode = "111";
                    else if ( tempData.value == 17 ) tempData.machineCode = "00";
                    else if ( tempData.value == 18 ) tempData.machineCode = "01";
                    else if ( tempData.value == 19 ) tempData.machineCode = "10";
                    else if ( tempData.value == 20 ) tempData.machineCode = "11";
                    RegisterTable.push_back( tempData ) ;
                } // register
                if( i == 4 ) DelimiterTable.push_back( tempData ) ;
                //cout<<tempData.table<<","<<tempData.value<<"  "<<tempData.tokenName<< endl;
				tableNum++;
            } // done
			tableNum = 1;
            Input.close();
        } // for
        
    } // ReadTable()
    
    /*void ReadType() {
        string tempTable;
        Type tempData ;
        string fileName = "type.txt" ;
        input.open( fileName.c_str() );
        
        while( !input.eof() ) {
            input >> tempData.tokenName ;
            input >> tempData.type ;
            TypeTable.push_back(tempData);
        } //  done
        
        input.close();
        
        //for( int i = 0 ;  i < TypeTable.size() ; i++ ) cout<<TypeTable[i].value<<"  "<<TypeTable[i].tokenName<< endl;
         
     } // ReadType()*/

    bool WhichType( string token ) {
        string Upper, Lower = "\0";
        
        Upper = token.substr(0, token.size() ) ;
        Lower = token.substr(0, token.size() ) ;
        for( int i = 0 ; i < token.size() ; i++ ) Upper[i] = toupper( Upper[i] );
        for( int i = 0 ; i < token.size() ; i++ ) Lower[i] = tolower( Lower[i] );
        
        
        for( int i = 0 ;  i < InstrucionTable.size() ; i++ ) {
            if ( InstrucionTable[i].tokenName == token || InstrucionTable[i].tokenName == Upper || InstrucionTable[i].tokenName ==Lower  ) {
                Tokens.push_back( InstrucionTable[i] ) ;
                //output << "("<< InstrucionTable[i].table << ","<< InstrucionTable[i].value << ")";
                return true;
            } // same
        } // find instruction
            
        for( int i = 0 ;  i < PseudoTable.size() ; i++ ) {
            if ( PseudoTable[i].tokenName == token || PseudoTable[i].tokenName == Upper || PseudoTable[i].tokenName == Lower ) {
                Tokens.push_back( PseudoTable[i] );
                //output << "("<< PseudoTable[i].table << ","<< PseudoTable[i].value << ")";
                return true;
            } // same
        } // find pseudo
            
        for( int i = 0 ;  i < RegisterTable.size() ; i++ ) {
            if ( RegisterTable[i].tokenName == token || RegisterTable[i].tokenName == Upper || RegisterTable[i].tokenName == Lower  ) {
                Tokens.push_back( RegisterTable[i] );
                //output << "("<< RegisterTable[i].table << ","<< RegisterTable[i].value << ")";
                return true;
            } // same
        } // find register
            
        for( int i = 0 ;  i < DelimiterTable.size() ; i++ ) {
            if ( DelimiterTable[i].tokenName == token  ) {
                Tokens.push_back( DelimiterTable[i] );
                //output << "("<< DelimiterTable[i].table << ","<< DelimiterTable[i].value << ")";
                return true;
            } // same
        } // find delimiter
        
        return false;
        
    } // WhichType

    bool isDelimiter( string token ) {
        for( int i = 0 ;  i < DelimiterTable.size() ; i++ ) {
            if ( DelimiterTable[i].tokenName == token  ) return true;
        } // find delimiter
        return false;
    } // isInt

    bool isInt( string token ) {
        for( int i = 0; i < token.size()  ; i ++ ) if ( !isxdigit(token[i]) && token[i] != 'H' ) return false;
        return true;
    } // isInt

    void StringProcess( string token ) {
        int AsciiSum, hValue = 0;
        bool done  = false;
        Token tempToken;
        for( int i = 0 ; i < token.size() ; i++ ) AsciiSum += (int)token[i] ;
        hValue = AsciiSum % 100;
        
        for( int i = 0 ; i < 100 ; i ++ ) {
            if( StringTable[i].tokenName == token ) {
                tempToken.tokenName = StringTable[i].tokenName;
                tempToken.table = 7;
                tempToken.value = StringTable[i].value;
                Tokens.push_back(tempToken);
                //output << "("<< tempToken.table << ","<< tempToken.value << ")";
                done = true;
            } // if
        } // find same in table
        
        while( !done ) {
            if( StringTable[hValue].value != 0 ) {
                hValue++;
                if ( hValue == 100 ) hValue = hValue - 100;
            } // collision
            
            else {
                tempToken.tokenName = token;
                tempToken.table = 7;
                tempToken.value = hValue;
                StringTable[hValue] = tempToken;
                Tokens.push_back(tempToken);
                //output << "("<< tempToken.table << ","<< tempToken.value << ")";
                done = true;
            } // done
        } // put in table
        
    } // StringProcess

    void IntProcess( string token ) {
        int AsciiSum, hValue = 0;
        bool done  = false;
        Token tempToken;
        for( int i = 0 ; i < token.size() ; i++ ) AsciiSum += (int)token[i] ;
        hValue = AsciiSum % 100;
        
        for( int i = 0 ; i < 100 ; i ++ ) {
            if( IntegerTable[i].tokenName == token ) {
                tempToken.tokenName = IntegerTable[i].tokenName;
                tempToken.table = 6;
                tempToken.value = IntegerTable[i].value;
                Tokens.push_back(tempToken);
                //output << "("<< tempToken.table << ","<< tempToken.value << ")";
                done = true;
            } // if
        } // find same in table
        
        while( !done ) {
            if( IntegerTable[hValue].value != 0 ) {
                hValue++;
                if ( hValue == 100 ) hValue = hValue - 100;
            } // collision
            
            else {
                tempToken.tokenName = token;
                tempToken.table = 6;
                tempToken.value = hValue;
                IntegerTable[hValue] = tempToken;
                Tokens.push_back(tempToken);
                //output << "("<< tempToken.table << ","<< tempToken.value << ")";
                done = true;
            } // done
        } // put in table
        
    } // IntProcess

    void SymbolProcess( string token ) {
        int AsciiSum, hValue = 0;
        bool done  = false;
        Token tempToken;
        for( int i = 0 ; i < token.size() ; i++ ) AsciiSum += (int)token[i] ;
        hValue = AsciiSum % 100;
        
        for( int i = 0 ; i < 100 ; i ++ ) {
            if( SymbolTable[i].tokenName == token ) {
                tempToken.tokenName = SymbolTable[i].tokenName;
                tempToken.table = 5;
                tempToken.value = SymbolTable[i].value;
                Tokens.push_back(tempToken);
                //output << "("<< tempToken.table << ","<< tempToken.value << ")";
                done = true;
            } // if
        } // find same in table
        
        while( !done ) {
            if( SymbolTable[hValue].value != 0 ) {
                hValue++;
                if ( hValue == 100 ) hValue = hValue - 100;
            } // collision
            
            else {
                tempToken.tokenName = token;
                tempToken.table = 5;
                tempToken.value = hValue;
                SymbolTable[hValue] = tempToken;
                Tokens.push_back(tempToken);
                //output << "("<< tempToken.table << ","<< tempToken.value << ")";
                done = true;
            } // done
        } // put in table
        
        
    } // SymbolProcess

    void InputFileAndAnalyze() {
        vector<char> CutLetters;
        bool stringStill = false;
        Token SyntaxTemp;
        SyntaxTemp.table = -1;
        SyntaxTemp.tokenName = "endl";
        SyntaxTemp.value = -1;
        
        while( getline( Input, wholeLine ) ) {

            for( int i = 0 ; i < wholeLine.size() ; i ++ ) {
                
                string charToString(1, wholeLine[i]);
                
                if( (wholeLine[i] != ' ' &&wholeLine[i] != '\n' &&wholeLine[i] != '\r' &&wholeLine[i] != '\t' &&wholeLine[i] != '\0' ) || stringStill ){
                    if( wholeLine[i] == ';' ) {
                        WhichType( charToString );
                        break;
                    } // comment case
                    
                    if( isDelimiter( charToString ) && stringStill == false ) {
                        if( charToString == "'" ) stringStill = true; // string with delimiter inside process
                        
                        string CutToken( CutLetters.begin(), CutLetters.end() ) ;
                        if( CutToken != "\0" ) {
                            if( !WhichType( CutToken ) ) {
                                if ( charToString == "'" && Tokens[ Tokens.size() - 1 ].tokenName == "'" ) StringProcess( CutToken );
                                else if ( isInt( CutToken ) ) IntProcess( CutToken );
                                else SymbolProcess( CutToken );
                            } // symbol,int or string
                            
                            CutLetters.clear();
                        } // delimiter with token in CutLetters
                        

                        WhichType( charToString );
                    } // Delimiter Case
                    
                    else {
                        if( charToString == "'" ) {
                            stringStill = false;
                            string CutToken( CutLetters.begin(), CutLetters.end() ) ;
                            //cout << CutToken << endl;
                            StringProcess( CutToken );
                            WhichType( charToString );
                            CutLetters.clear();
                            charToString = '\0';
                            continue;
                        } // string with delimiter inside process*/
                        
                        if( int(wholeLine[i] >= 32 ) )
                            CutLetters.push_back( wholeLine[i] ) ;
                    } // else
                } // letter process
                
                else {
                    string CutToken( CutLetters.begin(), CutLetters.end() ) ;
                    if( CutToken != "\0" ) {
                        if( !WhichType( CutToken ) ) {
                            if ( charToString == "'" && Tokens[ Tokens.size() - 1 ].tokenName == "'" ) StringProcess( CutToken );
                            else if ( isInt( CutToken ) ) IntProcess( CutToken );
                            else SymbolProcess( CutToken );
                        } // symbol,int or string

                        CutLetters.clear();
                    } // delimiter with token in CutLetters
                    
                } // put char to command
            
            } // line analyze
            
            
            string CutToken( CutLetters.begin(), CutLetters.end() ) ;
               if( CutToken != "\0" ) {
                   if( !WhichType( CutToken ) ) {
                       if ( isInt( CutToken ) ) IntProcess( CutToken );
                       else SymbolProcess( CutToken );
                   } // int or string , last token would not be string
                   
                   CutLetters.clear();
               } // last token left in CutLetters
            
            //output << endl;
            
            
            for( int i = 0 ; i < Tokens.size() ; i ++ ) if( Tokens[i].tokenName.size() >= 1 ) currentLineTokens.push_back(Tokens[i]);
            //SyntaxAnalyze() ;
            
            SaveLabel();
            FetchOperands();
            SearchOperands();
            
            
            Operands.clear();
            currentLineTokens.clear();
            Tokens.clear();
        } // input done
        
        /*for( int i = 0 ; i < Tokens.size() ; i ++ ) {
            cout << Tokens[i].table << " ," << Tokens[i].value << " ,"<<  Tokens[i].tokenName<<endl;
        } // for*/
        //for( int i = 0 ; i < 100 ; i ++ ) cout << SymbolTable[i].value << " "<<SymbolTable[i].tokenName << endl;
        lineNum = 0;
        linePlus = 0;
    } // InputFile
    
    void Print( string machineCode ) {
        lineNum += linePlus;
        Output << setw( 4 ) << setfill( '0' ) << hex << uppercase << lineNum << " ";
        linePlus = 0;
        for ( int i = 0; i < machineCode.size(); i++ ) {
            Output << uppercase << machineCode[i];
            if ( i % 2 == 1 ) {
                linePlus++;
                Output << " ";
            } // if
        } // print
        
        if( currentLineTokens[0].tokenName == "MOV" && currentLineTokens[2].table == Symbol )
            Output << "R" ;
        
        Output << "\t" << "\t";
        Output << wholeLine ;
        Output << endl;
    } // Output()
    
    void SaveLabel() {
        Label temp;
        for( int i = 0 ; i < currentLineTokens.size() ; i++ ) {
            if( currentLineTokens[i].table == Symbol ) {
                temp.labelName = currentLineTokens[i].tokenName;
                if( lineNum / 10 == 0 )
                    temp.address = "000" + to_string(lineNum);
                else
                    temp.address = "00" + to_string(lineNum);
            } // if
            
            LabelTable.push_back(temp);
        }// for
    } //SaveLabel()
    
    string SearchLabel( string tokenName ) {
        for( int i = 0 ; i < LabelTable.size() ; i++ ) {
            if( tokenName == LabelTable[i].labelName )
               return LabelTable[i].address;
        } // for
        
        return "0000" ;
    } // SearchLabel()
    
    
    
    
    void SyntaxAnalyze() {
        for( int i = 0 ; i < currentLineTokens.size() ; i++ ) {
            if( currentLineTokens[i].tokenName == "," && currentLineTokens[i + 1].table == Delimiter ) {
                for( int j = 0 ; j < currentLineTokens.size() ; j++ ) cout << currentLineTokens[j].tokenName << endl;
                cout <<  "Syntax error 拉幹，逗號不要他媽接Delimiter" << endl;
            } // if
            //else if
        } // for
    } // syntaxAnalyze
    
    void FetchOperands() {
        vector<Token>::iterator it = currentLineTokens.begin();
        
        for( int i = 0 ; i < currentLineTokens.size() ; i++, it++ ) {
            if( currentLineTokens[i].tokenName == "'"  )
                currentLineTokens.erase(it) ;
        } // delete" ' "
        
        it = currentLineTokens.begin();
        for( int i = 0 ; i < currentLineTokens.size() ; i++, it++ ) {
            if( currentLineTokens[i].tokenName == ","  )
                currentLineTokens.erase(it) ;
        } // delete" , "
        
        it = currentLineTokens.begin();
        for( int i = 0 ; i < currentLineTokens.size() ; i++, it++ ) {
            if( currentLineTokens[i].tokenName == "]"  )
                currentLineTokens.erase(it) ;
        } // delete" ] "
        
        it = currentLineTokens.begin();
        for( int i = 0 ; i < currentLineTokens.size() ; i++, it++ ) {
            if( currentLineTokens[i].tokenName == "]"  )
                currentLineTokens.erase(it) ;
        } // delete" [ "
        
        if( currentLineTokens[0].table == Symbol )
            currentLineTokens.erase( currentLineTokens.begin() ) ; // first symbol delete
        
        it = currentLineTokens.begin();
        for( int i = 0 ; i < currentLineTokens.size() ; i++, it++ ) {
            if( currentLineTokens[i].tokenName == "WORD" || currentLineTokens[i].tokenName == "word" ) {
                if( currentLineTokens[i + 1].tokenName == "PTR" || currentLineTokens[i + 1].tokenName == "ptr" ) {
                    currentLineTokens.erase(it) ;
                    currentLineTokens.erase(it) ;
                } // if
            } // if
        } // delete word ptr
        
        it = currentLineTokens.begin();
        for( int i = 0 ; i < currentLineTokens.size() ; i++, it++ ) {
            if( currentLineTokens[i].tokenName == "BYTE" || currentLineTokens[i].tokenName == "byte" ) {
                if( currentLineTokens[i + 1].tokenName == "PTR" || currentLineTokens[i + 1].tokenName == "ptr" ) {
                    currentLineTokens.erase(it) ;
                    currentLineTokens.erase(it) ;
                } // if
            } // if
        } // delete byte ptr
        
        //for( int i = 0 ; i < currentLineTokens.size() ; i++ ) cout << currentLineTokens[i].tokenName << "    ";
        //cout << endl;
    } //FetchOperands
    
    void SearchOperands(){
        string machineCode = "\0" ;
        if( currentLineTokens[0].tokenName == "BYTE" ) machineCode = BYTE();
        else if( currentLineTokens[0].tokenName == "MOV" ) machineCode = MOV() ;
        else if( currentLineTokens[0].tokenName == "CALL" ) machineCode = CALL() ;
        else if( currentLineTokens[0].tokenName == "INT" ) machineCode = INT() ;
        else if( currentLineTokens[0].tokenName == "RET" ) machineCode = RET() ;
        else if( currentLineTokens[0].tokenName == "AAD" ) machineCode = AAD();
        else if( currentLineTokens[0].tokenName == "AAM" ) machineCode = AAM();
        else if( currentLineTokens[0].tokenName == "AAA" ) machineCode = AAA();
        else if( currentLineTokens[0].tokenName == "AAS" ) machineCode = AAS();
        else if( currentLineTokens[0].tokenName == "CBW" ) machineCode = CBW();
        else if( currentLineTokens[0].tokenName == "CLC" ) machineCode = CLC();
        else if( currentLineTokens[0].tokenName == "CLD" ) machineCode = CLD();
        else if( currentLineTokens[0].tokenName == "CLI" ) machineCode = CLI();
        else if( currentLineTokens[0].tokenName == "CMC" ) machineCode = CMC();
        else if( currentLineTokens[0].tokenName == "CMPSB" ) machineCode = CMPSB();
        else if( currentLineTokens[0].tokenName == "CMPSW" ) machineCode = CMPSW();
        else if( currentLineTokens[0].tokenName == "CWD" ) machineCode = CWD();
        else if( currentLineTokens[0].tokenName == "DAA" ) machineCode = DAA();
        else if( currentLineTokens[0].tokenName == "DAS" ) machineCode = DAS();
        else if( currentLineTokens[0].tokenName == "FWAIT" ) machineCode = FWAIT();
        else if( currentLineTokens[0].tokenName == "HLT" ) machineCode = HLT();
        else if( currentLineTokens[0].tokenName == "INTO" ) machineCode = INTO();
        else if( currentLineTokens[0].tokenName == "IRET" ) machineCode = IRET();
        else if( currentLineTokens[0].tokenName == "LAHF" ) machineCode = LAHF();
        else if( currentLineTokens[0].tokenName == "LOCK" ) machineCode = LOCK();
        else if( currentLineTokens[0].tokenName == "LODSB" ) machineCode = LODSW();
        else if( currentLineTokens[0].tokenName == "MOVSB" ) machineCode = MOVSB();
        else if( currentLineTokens[0].tokenName == "MOVSW" ) machineCode = MOVSW();
        else if( currentLineTokens[0].tokenName == "NOP" ) machineCode = NOP();
        else if( currentLineTokens[0].tokenName == "POPF" ) machineCode = POPF();
        else if( currentLineTokens[0].tokenName == "PUSHF" ) machineCode = PUSHF();
        else if( currentLineTokens[0].tokenName == "REP" ) machineCode = REP();
        else if( currentLineTokens[0].tokenName == "REPE" ) machineCode = REPE();
        else if( currentLineTokens[0].tokenName == "REPNE" ) machineCode = REPNE();
        else if( currentLineTokens[0].tokenName == "REPZ" ) machineCode = REPZ();
        else if( currentLineTokens[0].tokenName == "SAHF" ) machineCode = SAHF();
        else if( currentLineTokens[0].tokenName == "SCASB" ) machineCode = SCASB();
        else if( currentLineTokens[0].tokenName == "SCASW" ) machineCode = SCASW();
        else if( currentLineTokens[0].tokenName == "STC" ) machineCode = STC();
        else if( currentLineTokens[0].tokenName == "STD" ) machineCode = STI();
        else if( currentLineTokens[0].tokenName == "STOSB" ) machineCode = STOSB();
        else if( currentLineTokens[0].tokenName == "STOSW" ) machineCode = STOSW();
        else if( currentLineTokens[0].tokenName == "WAIT" ) machineCode = WAIT();
        else if( currentLineTokens[0].tokenName == "XLAT" ) machineCode = XLAT();
        Print( machineCode );
    };
    
    string binaryToHexdacimal(string binary ) {
        // string binary = "1111000010100010"; // F0 A2
        //cout << binary << endl;
        string oneNibble = "\0";
        stringstream convert;
        string returnString = "\0";
        for ( int i = 0; i <= binary.size()-4; i+=4 ) {
            oneNibble += binary[i];
            oneNibble += binary[i+1];
            oneNibble += binary[i+2];
            oneNibble += binary[i+3];
            bitset<4> output(oneNibble);
            convert << hex << uppercase << output.to_ulong();
            returnString += convert.str();
            convert.str("");
            oneNibble = "\0";
        } // get four nibbles
        return returnString;
    } // convert binary to hexdacimal
    
    string StringToHex( string inputString ) {
        string hexString = "\0" ;
        int decimalNum = 0, size = 0;
        stringstream my_ss;
        //cout << inputString << ",   ";
        for( int i = 0 ; i < sizeof( inputString ) ; i++ ) {
            if( int(inputString[i]) >= 32) size++;
        } // cut tokens not clear
    
        for( int i = 0 ; i < size ; i++ ) {
            decimalNum = int(inputString[i]);
            if( decimalNum >= 32 ) {
                my_ss << hex << decimalNum;
                hexString =  my_ss.str();
            } // if
        } // for
        
        //cout << hexString << endl;
        return hexString;
    } // StringToHex()
    
    string BYTE() {
        string returnString, onlyString, intString = "\0";

        for ( int i = 1; i < currentLineTokens.size(); i++ ) {
            if( currentLineTokens[i].table == Integer ){
                currentLineTokens[i].tokenName.pop_back() ;
                returnString += currentLineTokens[i].tokenName;
                
                //cout << intString << endl;
            } // integer case
            
            else{
                for ( int k = 0; k < currentLineTokens[i].tokenName.size(); k++ ) {
                    onlyString += currentLineTokens[i].tokenName[k];
                } // collect all char in each string
                
                returnString += StringToHex( onlyString );
                //cout << onlyString << endl;
            } // string case
            
        } // run all string after the "BYTE"
            
        //cout << returnString<< endl;
        return returnString; // already in hexadicimal
    } // byte case

    string AAD() { return binaryToHexdacimal("1101010100001010"); }
    string AAM() { return binaryToHexdacimal("1101010000001010"); }
    string AAA() { return binaryToHexdacimal("00110111"); }
    string AAS() { return binaryToHexdacimal("00111111"); }
    string CBW() { return binaryToHexdacimal("10011000"); }
    string CLC() { return binaryToHexdacimal("11111000"); }
    string CLD() { return binaryToHexdacimal("11111100"); }
    string CLI() { return binaryToHexdacimal("11111010"); }
    string CMC() { return binaryToHexdacimal("11110101"); }
    string CMPSB() { return binaryToHexdacimal("10100110"); }
    string CMPSW() { return binaryToHexdacimal("10100111"); }
    string CWD() { return binaryToHexdacimal("10011001"); }
    string DAA() { return binaryToHexdacimal("00100111"); }
    string DAS() { return binaryToHexdacimal("00101111"); }
    string FWAIT() { return binaryToHexdacimal("10011011"); }
    string HLT() { return binaryToHexdacimal("11110100"); }
    string INTO() { return binaryToHexdacimal("11001110"); }
    string IRET() { return binaryToHexdacimal("11001111"); }
    string LAHF() { return binaryToHexdacimal("10011111"); }
    string LOCK() { return binaryToHexdacimal("11110000"); }
    string LODSB() { return binaryToHexdacimal("10101100"); }
    string LODSW() { return binaryToHexdacimal("10101101"); }
    string MOVSB() { return binaryToHexdacimal("10100100"); }
    string MOVSW() { return binaryToHexdacimal("10100101"); }
    string NOP() { return binaryToHexdacimal("10010000"); }
    string POPF() { return binaryToHexdacimal("10011110"); }
    string PUSHF() { return binaryToHexdacimal("10011101"); }
    string REP() { return binaryToHexdacimal("11110011"); }
    string REPE() { return binaryToHexdacimal("11110011"); }
    string REPNE() { return binaryToHexdacimal("11110010"); }
    string REPNZ() { return binaryToHexdacimal("11110010"); }
    string REPZ() { return binaryToHexdacimal("11110011"); }
    string SAHF() { return binaryToHexdacimal("10011110"); }
    string SCASB() { return binaryToHexdacimal("10101110"); }
    string SCASW() { return binaryToHexdacimal("10101110"); }
    string STC() { return binaryToHexdacimal("11111001"); }
    string STD() { return binaryToHexdacimal("11111110"); }
    string STI() { return binaryToHexdacimal("11111011"); }
    string STOSB() { return binaryToHexdacimal("10101010"); }
    string STOSW() { return binaryToHexdacimal("10101011"); }
    string WAIT() { return binaryToHexdacimal("10011011"); }
    string XLAT() { return binaryToHexdacimal("11100111"); }

    string PUSH() {
        if ( currentLineTokens.size() == 2 ) return "01010reg";
        else return "11111111mod110rm";
    } // PUSH()

    string POP() {
        if ( currentLineTokens.size() == 2 ) return "01011reg";
        else return "10001111mod000rm";
    } // POP()

    string RET() { return binaryToHexdacimal("11000011"); }
    string RETF() { return ""; } // NULL

    string INT() {
        currentLineTokens[1].tokenName.pop_back();
        return binaryToHexdacimal("11001101") + currentLineTokens[1].tokenName;
        // ex. INT 21H
    } // INT()

    string IN() {
        if ( currentLineTokens[1].tokenName == "AL" || currentLineTokens[1].tokenName == "AX" ) return "1110010wport";
        else return "1110110w";
    } // IN()

    string OUT() {
        if ( currentLineTokens[1].tokenName == "AL" || currentLineTokens[1].tokenName == "AX" ) return "1110011wport";
        else return "1110110w";
    } // OUT()

    string JMP() { return ""; } // not in input

    string CALL() {
        return binaryToHexdacimal("11101000") + "0004"; // lo hi is the memory address of the label after "CALL"
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
        if ( currentLineTokens[1].table == Register && currentLineTokens[2].table == Register ) {
            //cout << binaryToHexdacimal("10001110110" + currentLineTokens[1].machineCode + currentLineTokens[2].machineCode) << endl;
            return binaryToHexdacimal("10001110110" + currentLineTokens[1].machineCode + currentLineTokens[2].machineCode);
        } // register to segment register ex. MOV DS,AX
        else if ( currentLineTokens[1].table == Register && currentLineTokens[2].table == Integer ) {
            //cout << binaryToHexdacimal("10110" + currentLineTokens[1].machineCode) + currentLineTokens[2].tokenName[0] + currentLineTokens[2].tokenName[1] << endl;
            return binaryToHexdacimal("10110" + currentLineTokens[1].machineCode) + currentLineTokens[2].tokenName[0] + currentLineTokens[2].tokenName[1];
        } // immediate to register ex. MOV AH,4CH
        else if ( currentLineTokens[1].tokenName == "AX" ) {
            //cout << binaryToHexdacimal("101000010000") << endl;
            return binaryToHexdacimal("10100001") + SearchLabel(currentLineTokens[2].tokenName);
        } // AX case
        else if ( currentLineTokens[1].table == Register && currentLineTokens[2].table == Symbol ) {
            //cout << binaryToHexdacimal("1000101100" + currentLineTokens[1].machineCode + "010") << endl;
            return binaryToHexdacimal("1000101100" + currentLineTokens[1].machineCode + "110") + SearchLabel(currentLineTokens[2].tokenName);
        } // DX case
        else return "0000" ;
    } // MOV()

    string ADC() { return ""; } // not in input
    string ADD() { return ""; } // not in input
    string AND() { return ""; } // not in input
    string CMP() { return ""; } // not in input
    string OR() { return ""; } // not in input
    string SBB() { return ""; } // not in input
    string SUB() { return ""; } // not in input
    string XOR() { return ""; } // not in input
}; // Assembler

int main( int argc, const char* argv[] ) {
	int command = 0;
	bool continueOrNot = false;
	Assembler Start;
	//Start.Go();
	//return 0;
	Start.ReadTable();
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
						Start.InputFileAndAnalyze();
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
