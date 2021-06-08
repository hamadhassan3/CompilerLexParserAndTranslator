#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <exception>
using namespace std;

/*
The Token structure holds the token and lexeme values
*/
struct Token {
	string token;
	string lex;
	string type;
};


int currIndex = 0;	//Current index to check in string
int currState = -1;	//Current state in state machine
vector<Token> allTokens;	//The extracted tokens
string currTok = ""; //Keeps track of the token currently being checked
int currLine = 0;	//Keeps track of line number
string errLex = "";


const vector<string> DATA_TYPES({ "Integer", "char" });
const vector<string> KEYWORDS({ "if", "elif", "else", "while",
	"In", "func", "print", "println", "ret" });


//The names of tokens
const string ID = "ID";
const string KEYWORD = "KEYWORD";
const string DATATYPE = "DATATYPE";
const string COLON = "':'";
const string PARENTHESIS = "PARENTHESIS";
const string BRACES = "BRACES";
const string SQBRACKET = "SQBRACKET";
const string COMMA = "','";
const string COMMENT = "COMMENT";
const string SEMICOLON = "';'";
const string ASSIGNOP = "ASSIGNOP";
const string STRING = "STRING";
const string LITERAL = "LITERAL";
const string AO = "AO";
const string RO = "RO";
const string IOP = "IOP";
const string NUMCONST = "NUMCONST";
const string SPACE = "SPACE";

//The values associated with states
const int START_STATE = 0;
const int ID_STATE = 10;
const int SPACE_STATE = 20;
const int COLON_STATE = 30;
const int PARENTHESIS_STATE = 40;
const int BRACES_STATE = 50;
const int SQBRACKET_STATE = 60;
const int COMMA_STATE = 70;
const int COMMENT_STATE = 80;
const int SEMICOLON_STATE = 90;
const int ASSIGNOP_STATE = 100;
const int STRING_STATE = 110;
const int LITERAL_STATE = 120;
const int RO_STATE = 130;
const int AO_STATE = 140;
const int IOP_STATE = 150;
const int NUMCONST_STATE = 160;

/*
This method checks in the keywords if the provided string is a keyword
*/
bool isKeyWord(string str) {

	for (int i = 0; i < KEYWORDS.size(); i++) {
		if (KEYWORDS[i] == str) {
			return true;
		}
	}
	return false;
}

/*
This method checks in the datatypes array if the provided string is a datatype
*/
bool isDataType(string str) {

	for (int i = 0; i < DATA_TYPES.size(); i++) {
		if (DATA_TYPES[i] == str) {
			return true;
		}
	}
	return false;
}
/*
This function checks if a valid ID token is present
Returns false if token is invalid.
*/
bool isId(const string FILE_DATA, bool& tokenEnded) {

	switch (currState) {

	case ID_STATE + 0:
		if (isalpha(FILE_DATA[currIndex])) {
			currState = ID_STATE + 1;
			return true;
		}
		else {
			return false;
		}
		break;
	case ID_STATE + 1:
		if (isalpha(FILE_DATA[currIndex]) || isdigit(FILE_DATA[currIndex])) {
			return true;
		}
		else {
			tokenEnded = true;
			return true;
		}
		break;
	}

	return false;

}


/*
This function checks if a valid Comment is present
Returns false if token is invalid.
*/
bool isComment(const string FILE_DATA, bool& tokenEnded, Token& token) {

	switch (currState) {

	case COMMENT_STATE + 0:
		if (FILE_DATA[currIndex] == '/') {
			currState = COMMENT_STATE + 1;
			return true;
		}
		else {
			return false;
		}
		break;
	case COMMENT_STATE + 1:
		if (FILE_DATA[currIndex] == '*') {
			currState = COMMENT_STATE + 2;
			return true;
		}
		else {
			return false;
		}
		break;
	case COMMENT_STATE + 2:
		if (FILE_DATA[currIndex] != '*') {
			token.lex += FILE_DATA[currIndex];
			if (FILE_DATA[currIndex] == '\n') {
				currLine++;
			}
			return true;
		}
		else {
			currState = COMMENT_STATE + 3;
			return true;
		}
		break;
	case COMMENT_STATE + 3:
		if (FILE_DATA[currIndex] == '/') {
			tokenEnded = true;
			return true;
		}
		else {
			currState = COMMENT_STATE + 2;
			return true;
		}
		break;


	}





	return false;

}

/*
This function checks if a valid String is present
Returns false if token is invalid.
*/
bool isString(const string FILE_DATA, bool& tokenEnded, Token& token) {

	switch (currState) {

	case STRING_STATE + 0:
		if (FILE_DATA[currIndex] == '"') {
			currState = STRING_STATE + 1;
			return true;
		}
		else {
			return false;
		}
		break;
	case STRING_STATE + 1:
		if (FILE_DATA[currIndex] == '"') {
			tokenEnded = true;
			return true;
		}
		else {
			token.lex += FILE_DATA[currIndex];
			return true;
		}
		break;
	}





	return false;

}


/*
This function checks if a valid Literal is provided
Returns false if token is invalid.
*/
bool isLiteral(const string FILE_DATA, bool& tokenEnded, Token& token) {

	switch (currState) {

	case LITERAL_STATE + 0:
		if (FILE_DATA[currIndex] == '\'') {
			currState = LITERAL_STATE + 1;
			return true;
		}
		else {
			return false;
		}
		break;
	case LITERAL_STATE + 1:
		if (FILE_DATA[currIndex] != '\'') {
			token.lex += FILE_DATA[currIndex];
			currState = LITERAL_STATE + 2;
			return true;
		}
		else {

			return false;
		}
		break;
	case LITERAL_STATE + 2:
		if (FILE_DATA[currIndex] == '\'') {
			tokenEnded = true;
			return true;
		}
		else {

			return false;
		}
	}





	return false;

}


/*
This function calls the state machines of tokens after going through
the initial states of the big DFA
If a valid token is identified, it adds it to the global vector of tokens
and returns true, otherwise it returns false without adding anything
*/
bool nextToken(const string FILE_DATA) {



	//Resetting the state machine
	currState = START_STATE;
	bool tokenEnded = false; //Indicates if the current token has ended
	bool valid = true; //Indicates if an error is found

	Token newToken;	//The token to be inserted
	newToken.token = "";
	newToken.lex = "";

	//This loop keeps on running until token is completed or error is found
	while (currIndex < FILE_DATA.size() && !tokenEnded && valid) {

		switch (currState) {

		case START_STATE:
			//Checking which state machine should be called
			if (FILE_DATA[currIndex] >= 0 && isalpha(FILE_DATA[currIndex])) {
				currState = ID_STATE;
				currTok = ID;
			}
			else if (FILE_DATA[currIndex] == ' ' || FILE_DATA[currIndex] == '\n' ||
				FILE_DATA[currIndex] == '\t') {
				currState = SPACE_STATE;
				currTok = SPACE;
			}
			else if (FILE_DATA[currIndex] == ':') {
				currState = COLON_STATE;
				currTok = COLON;
			}
			else if (FILE_DATA[currIndex] == '(' || FILE_DATA[currIndex] == ')') {
				currState = PARENTHESIS_STATE;
				currTok = PARENTHESIS;
			}
			else if (FILE_DATA[currIndex] == '{' || FILE_DATA[currIndex] == '}') {
				currState = BRACES_STATE;
				currTok = BRACES;
			}
			else if (FILE_DATA[currIndex] == '[' || FILE_DATA[currIndex] == ']') {
				currState = SQBRACKET_STATE;
				currTok = SQBRACKET;
			}
			else if (FILE_DATA[currIndex] == ',') {
				currState = COMMA_STATE;
				currTok = COMMA;
			}
			else if (FILE_DATA[currIndex] == '+' || FILE_DATA[currIndex] == '-' ||
				FILE_DATA[currIndex] == '*' || FILE_DATA[currIndex] == '/') {
				currState = AO_STATE;
				currTok = AO;
			}
			else if (FILE_DATA[currIndex] == '<' || FILE_DATA[currIndex] == '=' ||
				FILE_DATA[currIndex] == '>') {
				currState = RO_STATE;
				currTok = RO;
			}
			else if (FILE_DATA[currIndex] == ';') {
				currState = SEMICOLON_STATE;
				currTok = SEMICOLON;
			}
			else if (FILE_DATA[currIndex] == '\'') {
				currState = LITERAL_STATE;
				currTok = LITERAL;
			}
			else if (FILE_DATA[currIndex] == '"') {
				currState = STRING_STATE;
				currTok = STRING;
			}
			else if (isdigit(FILE_DATA[currIndex])) {
				currState = NUMCONST_STATE;
				currTok = NUMCONST;
			}
			else {
				valid = false;
			}
			break;
		case ID_STATE + 0:
		case ID_STATE + 1:

			valid = isId(FILE_DATA, tokenEnded);

			if (valid) {

				if (!tokenEnded) {
					newToken.lex += FILE_DATA[currIndex];

					currIndex++;
				}
				else {
					if (isKeyWord(newToken.lex)) {
						newToken.token = newToken.lex;
						newToken.lex = "^";
					}
					else if (isDataType(newToken.lex)) {
						newToken.token = newToken.lex;
						newToken.lex = "^";
					}
					else {
						newToken.token = ID;
					}

					allTokens.push_back(newToken);
				}
			}

			break;
		case SPACE_STATE:
			if (FILE_DATA[currIndex] == ' ' || FILE_DATA[currIndex] == '\n' || FILE_DATA[currIndex] == '\t') {
				if (FILE_DATA[currIndex] == '\n') {
					currLine++;
				}
				currIndex++;
			}
			else {
				tokenEnded = true;
			}
			break;
		case COLON_STATE:

			if (FILE_DATA[currIndex] == ':') {
				currIndex++;
				if (FILE_DATA[currIndex] == '=') {
					currIndex++;
					newToken.token = "':='";
					newToken.lex = "^";
					allTokens.push_back(newToken);
					tokenEnded = true;
				}
				else {
					newToken.token = COLON;
					newToken.lex = "^";
					allTokens.push_back(newToken);
					tokenEnded = true;
				}
			}
			else {
				valid = false;
			}
			break;
		case PARENTHESIS_STATE:
			if (FILE_DATA[currIndex] == '(') {
				currIndex++;
				newToken.token = "'('";
				newToken.lex = "^";
				allTokens.push_back(newToken);
				tokenEnded = true;
			}
			else if (FILE_DATA[currIndex] == ')') {
				currIndex++;
				newToken.token = "')'";
				newToken.lex = "^";
				allTokens.push_back(newToken);
				tokenEnded = true;
			}
			else {
				valid = false;
			}
			break;
		case BRACES_STATE:
			if (FILE_DATA[currIndex] == '{') {
				currIndex++;
				newToken.token = "'{'";
				newToken.lex = "^";
				allTokens.push_back(newToken);
				tokenEnded = true;
			}
			else if (FILE_DATA[currIndex] == '}') {
				currIndex++;
				newToken.token = "'}'";
				newToken.lex = "^";
				allTokens.push_back(newToken);
				tokenEnded = true;
			}
			else {
				valid = false;
			}
			break;
		case SQBRACKET_STATE:
			if (FILE_DATA[currIndex] == '[') {
				currIndex++;
				newToken.token = "'['";
				newToken.lex = "^";
				allTokens.push_back(newToken);
				tokenEnded = true;
			}
			else if (FILE_DATA[currIndex] == ']') {
				currIndex++;
				newToken.token = "']'";
				newToken.lex = "^";
				allTokens.push_back(newToken);
				tokenEnded = true;
			}
			else {
				valid = false;
			}
			break;
		case COMMA_STATE:
			if (FILE_DATA[currIndex] == ',') {
				currIndex++;
				newToken.token = COMMA;
				newToken.lex = "^";
				allTokens.push_back(newToken);
				tokenEnded = true;
			}
			else {
				valid = false;
			}
			break;
		case COMMENT_STATE + 0:
		case COMMENT_STATE + 1:
		case COMMENT_STATE + 2:
		case COMMENT_STATE + 3:
		case COMMENT_STATE + 4:
			valid = isComment(FILE_DATA, tokenEnded, newToken);



			if (valid) {
				currIndex++;
				if (tokenEnded) {



					newToken.token = COMMENT;
					newToken.lex = "/*" + newToken.lex;
					newToken.lex = newToken.lex + "*/";
					allTokens.push_back(newToken);
				}
			}
			break;
		case SEMICOLON_STATE:
			if (FILE_DATA[currIndex] == ';') {
				currIndex++;
				newToken.token = SEMICOLON;
				newToken.lex = "^";
				allTokens.push_back(newToken);
				tokenEnded = true;
			}
			else {
				valid = false;
			}
			break;
		case LITERAL_STATE + 0:
		case LITERAL_STATE + 1:
		case LITERAL_STATE + 2:
			valid = isLiteral(FILE_DATA, tokenEnded, newToken);



			if (valid) {
				currIndex++;
				if (tokenEnded) {

					newToken.token = LITERAL;
					newToken.lex = "'" + newToken.lex;
					newToken.lex = newToken.lex + "'";
					allTokens.push_back(newToken);
				}
			}

			break;



		case STRING_STATE + 0:
		case STRING_STATE + 1:

			valid = isString(FILE_DATA, tokenEnded, newToken);



			if (valid) {
				currIndex++;
				if (tokenEnded) {

					newToken.token = STRING;
					newToken.lex = "\"" + newToken.lex;
					newToken.lex = newToken.lex + "\"";
					allTokens.push_back(newToken);
				}
			}

			break;
		case RO_STATE:
			if (FILE_DATA[currIndex] == '>') {

				currIndex++;
				currState = RO_STATE + 1;
			}
			else if (FILE_DATA[currIndex] == '<') {
				currIndex++;
				currState = RO_STATE + 2;
			}
			else if (FILE_DATA[currIndex] == '=') {
				currIndex++;
				currState = RO_STATE + 4;
			}
			break;
		case RO_STATE + 1:
			if (FILE_DATA[currIndex] == '>') {
				currState = IOP_STATE + 1;
			}
			else if (FILE_DATA[currIndex] == '=') {
				currIndex++;
				newToken.token = "'>='";
				newToken.lex = ">=";
				allTokens.push_back(newToken);
				tokenEnded = true;
				valid = true;
			}
			else {
				newToken.token = RO;
				newToken.token = "'>'";
				newToken.lex = "^";
				allTokens.push_back(newToken);
				tokenEnded = true;
				valid = true;
			}
			break;
		case RO_STATE + 2:
			if (FILE_DATA[currIndex] == '=') {
				currIndex++;
				newToken.token = "'<='";
				newToken.lex = "^";
				allTokens.push_back(newToken);
				tokenEnded = true;
				valid = true;
			}
			else {
				newToken.token = RO;
				newToken.token = "'<'";
				newToken.lex = "^";
				allTokens.push_back(newToken);
				tokenEnded = true;
				valid = true;
			}
			break;
		case RO_STATE + 3:
			if (FILE_DATA[currIndex] == '=') {
				currIndex++;
				newToken.token = RO;
				newToken.token = "'/='";
				newToken.lex = "^";
				tokenEnded = true;
				allTokens.push_back(newToken);
				valid = true;
			}
			else {
				valid = false;
			}
			break;
		case RO_STATE + 4:
			if (FILE_DATA[currIndex] == '=') {
				currIndex++;
				newToken.token = RO;
				newToken.token = "'=='";
				newToken.lex = "^";
				tokenEnded = true;
				allTokens.push_back(newToken);
				valid = true;
			}
			else {
				valid = false;
			}
			break;

		case IOP_STATE:
			if (FILE_DATA[currIndex] == '>') {
				currIndex++;
				currState = IOP_STATE + 1;
				valid = true;
			}
			else {
				valid = false;
			}
			break;
		case IOP_STATE + 1:
			if (FILE_DATA[currIndex] == '>') {
				currIndex++;
				newToken.token = "'>>'";
				newToken.lex = "^";
				tokenEnded = true;
				allTokens.push_back(newToken);
				valid = true;
			}
			else {
				valid = false;
			}
			break;
		case AO_STATE:
			if (FILE_DATA[currIndex] == '+') {
				currIndex++;
				newToken.token = "'+'";
				newToken.lex = "^";
				allTokens.push_back(newToken);
				tokenEnded = true;
			}
			else if (FILE_DATA[currIndex] == '-') {
				currIndex++;
				newToken.token = "'-'";
				newToken.lex = "-";
				allTokens.push_back(newToken);
				tokenEnded = true;
			}
			else if (FILE_DATA[currIndex] == '*') {
				currIndex++;
				newToken.token = "'*'";
				newToken.lex = "^";
				allTokens.push_back(newToken);
				tokenEnded = true;
			}
			else if (FILE_DATA[currIndex] == '/') {
				if (currIndex + 1 < FILE_DATA.size() && FILE_DATA[currIndex + 1] == '*') {
					currState = COMMENT_STATE;
				}
				else if (currIndex + 1 < FILE_DATA.size() && FILE_DATA[currIndex + 1] == '=') {
					currIndex++;
					currState = RO_STATE + 3;
				}
				else {
					currIndex++;
					newToken.token = "'/'";
					newToken.lex = "^";
					allTokens.push_back(newToken);
					tokenEnded = true;
				}
			}
			break;
		case NUMCONST_STATE:
			if (FILE_DATA[currIndex] >= '0' && FILE_DATA[currIndex] <= '9') {
				newToken.lex += FILE_DATA[currIndex];
				currIndex++;
			}
			else {
				newToken.token = NUMCONST;

				allTokens.push_back(newToken);
				tokenEnded = true;

				if (currIndex + 1 < FILE_DATA.size() && isalpha(FILE_DATA[currIndex + 1])) {
					valid = false;
				}

			}

		}




	}


	//For cases when comment opens but does not end, etc
	if (valid && !tokenEnded) {
		valid = false;
	}

	if (!valid) {
		errLex = newToken.lex;
	}

	return valid;

}











/*
This method prints all tokens in global vector the the specified stream
*/
void printAllTokens(ostream& out) {

	for (int i = 0; i < allTokens.size(); i++) {
		out << "(" << allTokens[i].token;
		out << ", " << allTokens[i].lex << ")";
		out << endl;
	}

}



//******************************************** Parser Code ***************************************************

/*
This class is used to report errors in parsing
*/
class CodeError : public std::exception {

private:
	string reason;
	int tokenNumber;

public:

	CodeError(string reason, int lineNumber) {
		this->reason = reason;
		this->tokenNumber = lineNumber;
	}

	CodeError(const CodeError& e) {
		this->reason = e.reason;
		this->tokenNumber = e.tokenNumber;
	}

	string details() {
		stringstream ss;
		ss << "*** Error ***\nReason: " << reason;
		ss << "\n";
		ss << "Token Number: ";
		ss << tokenNumber;
		ss << "\n";
		return ss.str();
	}

	const char* what() const throw ()
	{
		return "There is an error in the code";
	}
};


ofstream parseTree("parsetree.txt");

int look; //For looking up ahead
vector<int> tokensFromWords;
vector<Token> allTokens2;
ofstream dTOut("parser-symboltable.txt");
int currIdx = 0;
int depth = 0;	//Depth of function calls


const int FUNC_LOOK = 'f';
const int DTYPE_LOOK = 500;
const int ID_LOOK = 'I';
const int NUM_LOOK = 'N';
const int RELOP_LOOK = 600;
const int WHILE_LOOK = 'w';
const int IF_LOOK = 'i';
const int ELSE_LOOK = 'e';
const int ELIF_LOOK = 700;
const int ASSIGNOP_LOOK = 800;
const int LIT_LOOK = 900;
const int STR_LOOK = 1000;
const int IN_LOOK = 1100;
const int INOP_LOOK = 1200;
const int COMMENT_LOOK = 1300;


string DATAT = "";



//Prototypes
void S();
void F();
int Statements();
void P();
void P2();
string E();
string R();
string Condition();
string C();
void While();
void If();
void Elif();
void Else();
void Assign();
void Dec();
void RR();
void Input();
void Input2();
void Comment();
void BPr();
void PrintLn();
void FuncCall();

const string TAC_FILE = "tac.txt";
const string TAC_SYMBOLTABLE_FILE = "translator-symboltable.txt";
ofstream tacSOut(TAC_SYMBOLTABLE_FILE);




int memAddress = 0;

int currVar = 1;

vector<string> allTacLines;

//Returns the token that is currently being checked in the translator/parser
Token getToken() {

	return allTokens[currIdx];

}


void emit(string val) {

	allTacLines.push_back(val);

}


//Emits vector of strings to output
void emit(vector<string> vals) {

	
	string line = "";

	for (int i = 0; i < vals.size(); i++) {


		if (i != 0) {
			line += " ";
		}


		line += vals[i];




	}

	allTacLines.push_back(line);
	
	



}


//Creates and returns a new variable name
string newVar() {

	stringstream ss;

	ss << "tmp";
	ss << currVar;
	

	currVar++;

	return ss.str();
	

}


//This method is used to backpatch the goto statements
void backpatch(int lineNumber, int value) {

	stringstream ss;


	ss << allTacLines[lineNumber - 1];
	ss << " ";
	ss << value;

	allTacLines[lineNumber - 1] = ss.str();

}


//This method print the tac file
void printTacFile() {

	ofstream tacOut(TAC_FILE);

	if (!tacOut.is_open()) {
		cout << "Error! The file " << TAC_FILE << " could not be opened." << endl;
		return;
	}

	for (int i = 0; i < allTacLines.size(); i++) {


		
		tacOut << i+1 << ") " << allTacLines[i] << endl;

	}

	tacOut.close();


}


//Prints dots so that output shows clear depth
void printDepthDots() {

	for (int i = 0; i < depth; i++) {
		parseTree << ": ";
	}

}

//Checks if tokens match and then currIdx++
void match(int tok) {
	if (look == tok) {

		printDepthDots();

		if (allTokens2[currIdx].lex == "^") {
			parseTree << allTokens2[currIdx].token << endl;
		}
		else {
			parseTree << allTokens2[currIdx].lex << endl;
		}

		currIdx++;


		if (currIdx < tokensFromWords.size()) {
			look = tokensFromWords[currIdx];
		}
		else {
			look = EOF;
		}

	}
	else {
		throw CodeError("Syntax Error", currIdx);
	}

}



//S -> F S | ^
void S() {

	printDepthDots();
	parseTree << "S()" << endl;
	depth++;

	if (look == FUNC_LOOK) {


		F();
		S();
	}
	else {
		Statements();
		;	//Do nothing
	}

	depth--;

}

//F->func dType : ID(P) : {Statements}
void F() {

	printDepthDots();
	parseTree << "F()" << endl;
	depth++;

	if (look == FUNC_LOOK) {

		match(FUNC_LOOK);
		match(DTYPE_LOOK);
		match(':');
		match(ID_LOOK);
		match('(');
		P();
		match(')');
		match('{');
		Statements();
		match('}');

	}
	else {
		throw CodeError("Syntax Error in function", currIdx);
	}

	depth--;

}


//P->dType P2 | ^
void P() {

	printDepthDots();
	parseTree << "P()" << endl;
	depth++;

	if (look == DTYPE_LOOK) {


		match(DTYPE_LOOK);
		match(':');
		match(ID_LOOK);

		P2();

	}
	else {
		;//Do nothing
	}

	depth--;

}

//P2 ->, dType : ID P2 | ^
void P2() {

	printDepthDots();
	parseTree << "P2()" << endl;
	depth++;

	if (look == ',') {

		match(',');
		match(DTYPE_LOOK);
		match(':');
		match(ID_LOOK);

		P2();

	}
	else {
		;//Do nothing
	}

	depth--;


}


//E -> ID R | num R
string E() {

	printDepthDots();
	parseTree << "E()" << endl;
	depth++;

	Token currToken = getToken();

	string toRet = "";

	if (look == ID_LOOK) {

		match(ID_LOOK);
		string rVal = R();

		string temp = newVar();


		emit(temp + " = " + currToken.lex + rVal);

		toRet += temp;


	}
	else if (look == NUM_LOOK) {

		match(NUM_LOOK);
		string rVal = R();

		string temp = newVar();


		emit(temp + " = " + currToken.lex + rVal);

		toRet += temp;

	}
	else {
		throw CodeError("Syntax Error in Expression", currIdx);
	}



	depth--;
	return toRet;
}

//R -> + ID R | - ID R | + num R | - num R
//R -> ^
string R() {

	printDepthDots();
	parseTree << "R()" << endl;
	depth++;

	string toRet = "";


	if (look == '+') {
		match('+');

		Token currToken = getToken();

		if (look == ID_LOOK) {
			match(ID_LOOK);

		}
		else if (look == NUM_LOOK) {
			match(NUM_LOOK);
		}
		else {
			throw CodeError("Error in Expression", currIdx);
		}


		string rVal = R();


		string temp = newVar();

		emit(temp + " = " + currToken.lex + rVal);

		toRet += " + " + temp;


	}
	else if (look == '-') {
		match('-');
		Token currToken = getToken();
		if (look == ID_LOOK) {
			match(ID_LOOK);
		}
		else if (look == NUM_LOOK) {
			match(NUM_LOOK);
		}
		else {
			throw CodeError("Error in Expression", currIdx);
		}
		string rVal = R();


		string temp = newVar();

		emit(temp + " = " + currToken.lex + rVal);

		toRet += " - " + temp;
	}
	else {
		//Do Nothing
	}

	depth--;

	return toRet;

}




//Condition -> && Condition | || Condition | (Condition) | ID C | E C
string Condition() {

	printDepthDots();
	parseTree << "Condition()" << endl;
	depth++;

	string toPrint = "";

	if (look == '(') {
		
		match('(');
		toPrint += "(";
		toPrint += Condition();
		match(')');
		toPrint += ")";
	}
	else if (look == '&') {
		toPrint += "&&";
		match('&');
		toPrint += Condition();
		
	}
	else if (look == '|') {
		match('|');
		toPrint += "||";
		toPrint += Condition();
	}
	else if (look == ID_LOOK) {
		Token tok = getToken();
		match(ID_LOOK);
		toPrint += tok.lex;
		toPrint += C();
	}
	else if (look == NUM_LOOK) {
		toPrint += E();
		toPrint += C();
	}
	else {
		throw new CodeError("Error in Condition", currIdx);
	}

	depth--;
	return toPrint;

}

//C -> Relop ID | Relop E
string C() {

	printDepthDots();
	parseTree << "C()" << endl;
	depth++;

	string toPrint = "";

	if (look == RELOP_LOOK) {
		Token tok = getToken();
		match(RELOP_LOOK);
		string op = "";

		for (int i = 0; i < tok.token.size(); i++) {
			if (tok.token[i] != '\'') {
				op += tok.token[i];
			}
		}
		
		toPrint += " " + op + " ";
		if (look == ID_LOOK) {
			Token tk = getToken();
			match(ID_LOOK);

			toPrint += tk.lex;
		}
		else {
			toPrint += E();
		}
	}
	else {
		throw CodeError("Error in Condition ", currIdx);
	}


	depth--;
	return toPrint;
}





//While -> while Condition: {Statements}
void While() {

	printDepthDots();
	parseTree << "While()" << endl;
	depth++;


	string toPrint = "";
	if (look == WHILE_LOOK) {
		match(WHILE_LOOK);

		toPrint += "if ";
		toPrint += Condition();
		toPrint += " goto";

		emit(toPrint);
		int currLine = allTacLines.size();
		backpatch(currLine, allTacLines.size() + 2);

		emit("goto");


		match(':');
		match('{');

		int currLine2 = allTacLines.size();


	
		Statements();
		
		emit("goto");
		backpatch(allTacLines.size(), currLine);

		backpatch(currLine2, allTacLines.size() + 1);



		match('}');
	}
	else {
		throw CodeError("Error in while", currIdx);
	}

	depth--;
}






//If -> If BE : { Statements } Elif Else
void If() {

	printDepthDots();
	parseTree << "If()" << endl;
	depth++;


	string toPrint = "";

	if (look == IF_LOOK) {
		match(IF_LOOK);

		toPrint += "if ";
		toPrint += Condition();
		toPrint += " goto";

		emit(toPrint);
		int currLine = allTacLines.size();
		backpatch(currLine, allTacLines.size() + 2);
		
		emit("goto");



		match(':');
		match('{');

		currLine = allTacLines.size();

		Statements();

		emit("goto");

		backpatch(currLine, allTacLines.size() + 1);

		


		match('}');




		currLine = allTacLines.size();

		Elif();
		
		backpatch(currLine, allTacLines.size() + 1);



	}
	else {
		throw CodeError("Error in if statement", currIdx);
	}
	depth--;


}

//elif -> elif BE : { multipleStatement } | ^
void Elif() {
	printDepthDots();
	parseTree << "Elif()" << endl;
	depth++;

	string toPrint = "";

	if (look == ELIF_LOOK) {
		match(ELIF_LOOK);
		toPrint += "elif ";
		toPrint += Condition();
		toPrint += " goto";

		emit(toPrint);
		int currLine = allTacLines.size();
		backpatch(currLine, allTacLines.size() + 2);

		emit("goto");


		match(':');
		match('{');
		currLine = allTacLines.size();

		Statements();

		emit("goto");

		backpatch(currLine, allTacLines.size() + 1);
		match('}');

		currLine = allTacLines.size();
		Elif();
		backpatch(currLine, allTacLines.size() + 1);
	}
	else {
		Else();
	}

	depth--;
}

//Else-> else { Statements } | ^
void Else() {

	printDepthDots();
	parseTree << "Else()" << endl;
	depth++;


	string toPrint = "";


	if (look == ELSE_LOOK) {
		match(ELSE_LOOK);



		match('{');
		
		Statements();

		match('}');

	}
	else {
		//Do nothing
	}




	depth--;

}


void DR() {

	printDepthDots();
	parseTree << "DR()" << endl;
	depth++;

	if (look == ',') {
		match(',');
		match(ID_LOOK);

		dTOut << DATAT << " " << allTokens2[currIdx - 1].lex << endl;
		tacSOut << DATAT << " " << allTokens2[currIdx - 1].lex << " " << memAddress << endl;
		memAddress += (DATAT == "char") ? 1 : (DATAT == "Integer") ? 4 : 0;
		DR();
	}
	else {
		//Do nothing
	}

	depth--;

}



void Dec() {
	printDepthDots();
	parseTree << "Dec()" << endl;
	depth++;

	if (look == DTYPE_LOOK) {
		match(DTYPE_LOOK);
		DATAT = allTokens2[currIdx - 1].token;
		match(':');
		match(ID_LOOK);
		dTOut << DATAT << " " << allTokens2[currIdx - 1].lex << endl;
		tacSOut << DATAT << " " << allTokens2[currIdx - 1].lex << " " << memAddress << endl;
		memAddress += (DATAT == "char") ? 1 : (DATAT == "Integer") ? 4 : 0;
		DR();
		match(';');
	}
	else {
		throw CodeError("Error in assignment", currIdx);
	}

	depth--;
}




//stmt -> ID := L ;
//L -> num | literal | ID | E
void Assign() {

	printDepthDots();
	parseTree << "Assign()" << endl;
	depth++;

	Token tok = getToken();

	if (look == ID_LOOK) {

		match(ID_LOOK);




		if (look == '(') {
			FuncCall();
		}
		else if (look == ASSIGNOP_LOOK) {
			if (look == ASSIGNOP_LOOK) {
				match(ASSIGNOP_LOOK);

				if (look == NUM_LOOK || look == ID_LOOK) {
					string eRet = E();

					emit(tok.lex + " = " + eRet);

				}
				else if (look == LIT_LOOK) {

					Token litTok = getToken();
					match(LIT_LOOK);

					emit(tok.lex + " = " + litTok.lex);
				}


			}
			else if (look == RELOP_LOOK) {
				C();
			}
			else {
				throw CodeError("Error in Assignment", currIdx);
			}
		}
		match(';');
	}
	else {
		throw CodeError("Error in Assignment", currIdx);
	}

	depth--;
}


//RR->E | ID | literal
void RR() {

	printDepthDots();
	parseTree << "RR()" << endl;
	depth++;

	if (look == ID_LOOK) {
		match(ID_LOOK);

	}
	else if (look == NUM_LOOK) {
		E();
	}
	else if (look == LIT_LOOK) {
		match(LIT_LOOK);
	}
	else {
		throw CodeError("Error in Assignment", currIdx);
	}


	depth--;
}



/*
Input -> In >> ID Input2;

*/

void Input() {

	printDepthDots();
	parseTree << "Input()" << endl;
	depth++;

	if (look == IN_LOOK) {

		match(IN_LOOK);
		match(INOP_LOOK);

		Token idTok = getToken();
		match(ID_LOOK);
		Input2();
		match(';');

		emit("in " + idTok.lex);

	}
	else {
		throw CodeError("Error in Input", currIdx);
	}

	depth--;
}


//Input2 -> ,ID Input2 | ^
void Input2() {

	printDepthDots();
	parseTree << "Input2()" << endl;
	depth++;

	if (look == IN_LOOK) {
		match(',');
		Token idTok = getToken();
		match(ID_LOOK);
		Input2();

		emit("in " + idTok.lex);

	}

	depth--;
}




/*
Println -> println(BPr);

*/

void PrintLn() {

	printDepthDots();
	parseTree << "PrintLn()" << endl;
	depth++;


	
	if (look == 'p') {
		match('p');
		match('(');
		BPr();
		match(')');
		match(';');
	}
	else {
		throw CodeError("Error In PrintLn", currIdx);
	}

	depth--;
}

/*
Print -> print(BPr);

*/

void Print() {

	printDepthDots();
	parseTree << "Print()" << endl;
	depth++;

	if (look == 123) {
		match(123);
		match('(');
		BPr();
		match(')');
		match(';');
	}
	else {
		throw CodeError("Error In Print", currIdx);
	}

	depth--;

}


//Function call
//ID(E|Lit|String);
void FuncCall() {

	if (look == '(') {


		match('(');
		if (look == NUM_LOOK || look == ID_LOOK) {
			E();
		}
		else if (look == LIT_LOOK) {
			match(LIT_LOOK);
		}
		else if (look == STR_LOOK) {
			match(STR_LOOK);
		}
		else {
			throw CodeError("Error in function call", currIdx);
		}
		match(')');

	}

}


//BPr -> (BPr) | String | ID
void BPr() {

	printDepthDots();
	parseTree << "BPr()" << endl;
	depth++;

	if (look == '(') {
		match('(');
		BPr();
		match(')');


	}
	else if (look == STR_LOOK) {
		Token tok = getToken();
		match(STR_LOOK);

		emit("out " + tok.lex);

	}
	else if (look == ID_LOOK) {

		Token tok = getToken();
		match(ID_LOOK);

		emit("out " + tok.lex);
	}

	depth--;

}


void Comment() {

	printDepthDots();
	parseTree << "Comment()" << endl;
	depth++;


	if (look == COMMENT_LOOK) {
		match(COMMENT_LOOK);
	}


	depth--;

}


//R -> ret E; | ret literal;| ret ID; | ret;
void Return() {

	printDepthDots();
	parseTree << "Return()" << endl;
	depth++;

	if (look == 'r') {

		match('r');
		if (look == NUM_LOOK) {
			E();
		}
		else if (look == ID_LOOK) {

			Token tok = getToken();
			match(ID_LOOK);



		}
		else if (look == LIT_LOOK) {
			Token tok = getToken();
			match(LIT_LOOK);

		}
		match(';');
	}


	depth--;


}



/*
Statements -> ^
Statements -> While Statements
Statements -> If Statements
Statements -> Statements R
Statements -> Comment Statements
Statements -> Condition Statements
Statements -> DecStmt Statements
Statements -> AssignStmt Statements
Statements -> Print Statements | Println Statements
Statements -> Input Statements
*/
int Statements() {

	printDepthDots();
	parseTree << "Statements()" << endl;
	depth++;


	int lineNumber = allTacLines.size();

	if (look == WHILE_LOOK) {
		While();
		Statements();
	}
	else if (look == IF_LOOK) {
		If();
		Statements();
	}
	else if (look == 'r') {
		Return();
		Statements();
	}
	else if (look == COMMENT_LOOK) {
		Comment();
		Statements();

	}
	else if (look == 'p') {
		PrintLn();
		Statements();
	}
	else if (look == 123) {
		Print();
		Statements();
	}
	else if (look == DTYPE_LOOK) {
		Dec();
		Statements();
	}
	else if (look == ID_LOOK) {
		Assign();
		Statements();
	}
	else if (look == IN_LOOK) {
		Input();
		Statements();
	}

	depth--;

	return lineNumber;

}














Token readToken(ifstream& fin) {

	string tok;
	Token token;




	getline(fin, tok);

	while (!fin.eof() && tok[tok.size() - 1] != ')') {
		string temp;
		getline(fin, temp);
		tok += temp;
	}



	//Removing brackets, commas and inverted commas

	string tokFiltered = "";

	for (int i = 0; i < tok.size(); i++) {



		if (tok[i] == ',' || tok[i] == '\'' || tok[i] == '(' || tok[i] == ')') {
			if (i != 0 && i != tok.size() - 1 && tok[i - 1] == '\'' && tok[i + 1] == '\'') {
				tokFiltered += tok[i];
			}
		}
		else {
			tokFiltered += tok[i];
		}

	}

	stringstream ss(tokFiltered);

	//Extracting token and lexeme

	ss >> token.token;
	ss >> token.lex;

	return token;

}



int main() {

	if (!tacSOut.is_open()) {
		cout << "Error! the file " << TAC_SYMBOLTABLE_FILE << " could not be opened/created." << endl;
		return -1;
	}

	if (!dTOut.is_open()) {
		cout << "Error! the file " << "parser_symboltable" << " could not be opened/created." << endl;
		return -1;
	}

	dTOut << "DataType Name" << endl;
	tacSOut << "DataType Name Address" << endl;

	char filenameIn[50]; //The name of input file
	char filenameOut[] = "words.txt";


	//Getting input file name from user
	cout << "Input the name of input file: ";
	cin >> filenameIn;


	ifstream fin(filenameIn);


	if (!fin.is_open()) {
		parseTree << "Error, the file " << filenameIn << " could not be opened." << endl;
		return -1;
	}


	bool valid = true; //Becomes false when an error is found in token
	int fileSize = 0;	//Size of file
	string fileData = ""; //The contents of the file


	//Extracting file content to a string
	while (!fin.eof()) {

		char currChar;
		currChar = fin.get();

		if (currChar != EOF) {
			fileData += currChar;
		}

	}
	fin.close();

	while (currIndex < fileData.size() && valid) {
		if (currLine == 2) {
			int a = 0;
		}
		valid = nextToken(fileData);
	}

	parseTree << endl;
	if (!valid) {

		parseTree << "*** There is an error in source code ***" << endl;
		parseTree << "Line Number: " << currLine + 1 << endl;
		parseTree << "Error in token: " << currTok << endl;
		parseTree << "The lexeme with err is: " << errLex << endl;


	}
	else {
		cout << "The souce code is correct!" << endl;


		//Printing all tokens

		ofstream fout(filenameOut);

		if (!fout.is_open()) {
			cout << "Error opening the file " << filenameOut << endl;
			return -1;
		}


		printAllTokens(fout);

		fout.close();
		cout << "The tokens are now saved to " << filenameOut << endl;
	}





	try {

		ifstream finW(filenameOut);

		if (!finW.is_open()) {
			parseTree << "Error opening " << filenameOut << endl;
			return -1;
		}

		int tokenNumber = 1;
		while (!finW.eof()) {

			Token token = readToken(finW);	//Reads a single token
			allTokens2.push_back(token);
			/*
			const vector<string> DATA_TYPES({"Integer", "char"});
const vector<string> KEYWORDS({"if", "elif", "else", "while",
	"In", "func", "print", "println"});
			*/
			//parseTree << tokenNumber++ << ": Token: " << token.token << " Lex: " << token.lex << endl;

			if (!token.token.empty()) {

				if (token.token == "Integer" || token.token == "char") {
					tokensFromWords.push_back(DTYPE_LOOK);
				}
				else if (token.token == "<" || token.token == ">" || token.token == "<=" ||
					token.token == ">=" || token.token == "==" || token.token == "/=") {
					tokensFromWords.push_back(RELOP_LOOK);
				}
				else if (token.token == "elif") {
					tokensFromWords.push_back(700);
				}
				else if (token.token == ":=") {
					tokensFromWords.push_back(800);
				}
				else if (token.token == "print") {
					tokensFromWords.push_back(123);
				}
				else if (token.token == "LITERAL") {
					tokensFromWords.push_back(LIT_LOOK);
				}
				else if (token.token == "STRING") {
					tokensFromWords.push_back(STR_LOOK);
				}
				else if (token.token == "In") {
					tokensFromWords.push_back(IN_LOOK);
				}
				else if (token.token == ">>") {
					tokensFromWords.push_back(INOP_LOOK);
				}
				else if (token.token == "COMMENT") {
					tokensFromWords.push_back(COMMENT_LOOK);
				}
				else {
					tokensFromWords.push_back(token.token[0]);	//We take only the first character of token
				}
			}
		}

		currIdx = 0;
		look = tokensFromWords[currIdx];
		S();


		cout << "Parser: The Code is Valid!" << endl;
		

		//Printing the result of translator
		printTacFile();

		cout << "Translator: The three address code is now saved to " << TAC_FILE << endl;

	}
	catch (CodeError e) {
		cout << e.what() << endl;
		cout << e.details() << endl;
	}

	parseTree.close();
	dTOut.close();
	tacSOut.close();

	return 0;
}