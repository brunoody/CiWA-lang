//
// CiWA-lang.cpp: Main file
//

#include "./CiWA-lang.h"
using namespace std;

/*		ENUMERATION FUNC      */
int iota = 0;
int IOTA(bool reset = false) {
	if (reset) {
		iota = 0;
	}
	else {
		iota++;
	}
	return iota;
}
/***************************************/

/*		CONSTANTS		*/

/*	TYPE OPERATIONS	 */
const short __OP_INT8PUSH__ =			IOTA(true);
const short __OP_INT16PUSH__ =			IOTA();
const short __OP_INT32PUSH__ =			IOTA();
const short __OP_INT64PUSH__=			IOTA();
const short __OP_INT128PUSH__=			IOTA();
const short __OP_FLOAT16PUSH__ =		IOTA();
const short __OP_FLOAT32PUSH__ =		IOTA();
const short __OP_FLOAT64PUSH__ =		IOTA();
const short __OP_FLOAT128PUSH__ =		IOTA();
const short __OP_BOOLEANPUSH__ =		IOTA();
const short __OP_VARPUSH__ =			IOTA();
const short __OP_VARPOP__ =				IOTA();
const short __OP_CONSTDEF__ =			IOTA();
const short __OP_VARSETVAL__ =			IOTA();

/*	 COMMANDS	*/
const short __OP_IFSTART__ =			IOTA();
const short __OP_ELSE__ =				IOTA();
const short __OP_ELSIF_ =				IOTA();
const short __OP_END__ =				IOTA();

/*	 OPERATORS  */
const short __OP_EQUALS__ =				IOTA();
const short __OP_NOTEQUALS__ =			IOTA();
const short __OP_GREATER__ =			IOTA();
const short __OP_GREATEROREQUAL__ =		IOTA();
const short __OP_LESS__ =				IOTA();
const short __OP_LESSOREQUAL__ =		IOTA();

/***************************************/

/*	 TYPE DEFINITIONS	*/

typedef struct operation {
	int	  OP_TYPE = -1;
	char* OP_LABEL = 0; // the address label or typename for the WebAssembly code
	char* OP_VALUE = 0; // can be any size of byte[], basically
};

typedef union t_int8 {
	short i;
	char c;
};

typedef union t_int16 {
	short i;
	char c[2];
};

typedef union t_int32 {
	int i;
	char c[4];
};

typedef union t_int64 {
	long i;
	char c[8];
};

typedef union t_int128 {
	long long i;
	char c[16];
};

typedef struct float_parsed {
	string intPart = "";
	string floatPart = "";
};

typedef union t_float16 {
	float f;
	char c[2];
};

typedef union t_float32{
	float f;
	char c[4];
};

typedef union t_float64 {
	double f;
	char c[8];
};

typedef union t_float128 {
	long double f;
	char c[16];
};

/***************************************/

/*			UTILLS			*/

/*		 GENERAL UTILS		*/
unsigned long itIndex = 0;
string GetToken(string str, char delim, bool resetItIndex = false) {
	if (resetItIndex) {
		itIndex = 0;
	}

	if (str == "") {
		return "";
	}

	int delimIndex = -1;
	for (int i = itIndex; i <= str.size(); i++) {
		if (str[i] == delim || str[i] == '\0') {
			delimIndex = i;
			break;
		}
	}

	if (delimIndex == -1) {
		return "";
	}

	string ret = str.substr(itIndex, (delimIndex - itIndex));
	itIndex += (delimIndex - itIndex) + 1;
	return ret;
}

char* StrToCharPointer(string str, bool addNullChar = true) {
	char* ret = 0;
	if (addNullChar) {
		ret = new char[str.size() + 1];
		copy(str.begin(), str.end(), ret);
		ret[str.size()] = '\0'; // 'end char'
	}
	else {
		ret = new char[str.size()];
		copy(str.begin(), str.end(), ret);
	}
	return ret;
}

short floatPoints = 0;
float_parsed ParseFloat(string str) {
	short pointIndex = 0;
	for (int i = 0; i <= str.size(); i++) {
		if (str[i] == '.') {
			pointIndex = i;
			break;
		}
	}
	floatPoints = str.size() - pointIndex - 1;
	float_parsed parse;
	parse.intPart = str.substr(0, pointIndex);
	parse.floatPart = str.substr(pointIndex + 1, str.size());
	return parse;
}

long double StrToLongDouble(string str, short shiftBits = 0) {
	long long longLongPart = stod(ParseFloat(str).intPart);
	long long floatPartAsLongLong = stod(ParseFloat(str).floatPart);
	if (shiftBits > 0) {
		longLongPart = longLongPart >> shiftBits & 0xFF;
		floatPartAsLongLong = floatPartAsLongLong >> shiftBits & 0xFF;
	}
	float divisor = 10.0;
	for (int i = 1; i < floatPoints; i++) {
		divisor = divisor * 10.0;
	}
	const long double doublePart = floatPartAsLongLong / divisor;
	return longLongPart + doublePart;
}

string GetVarchar(string line, string token) {
	char toCheck = '\0';
	short start, countChars = -1;

	if (token[0] == '"') {
		toCheck = '"';
	}
	else if (token[0] == '\'') {
		toCheck = '\'';
	}

	if (toCheck != '\0') {
		start = line.find(token);
		countChars = start + 1;
		char temp = '\0';
		do {
			temp = line[countChars];
			countChars++;
			if (temp == toCheck) {
				break;
			}
		} while (temp != toCheck);
		itIndex += countChars-start-2;
		return line.substr(start+1, countChars-start-2);
	}

	return "";
}

/*		 OPERATION UTILS		*/
operation OpPush(string line, string word, long lineNumber, short OP_TYPE, short memSize) {
	operation op;
	op.OP_TYPE = OP_TYPE;

	word = GetToken(line, ' '); // reads the variable's name
	if (word == "") {
		cout << "--- syntax error in line (" << lineNumber << "): '" << line << "'; <-- expected variable's name definition.\n";
		exit(1);
	}
	op.OP_LABEL = StrToCharPointer(word);

	word = GetToken(line, ' '); // reads the variable's value
	if (word == "") {
		cout << "--- syntax error in line (" << lineNumber << "): '" << line << "'; <-- expected variable's value definition.\n";
		exit(1);
	}

	if (word[0] == '"' || word[0] == '\'') { // generic 'var', strings and character values
		word = GetVarchar(line, word);
		op.OP_VALUE = StrToCharPointer(word);
	}
	else {
		if (OP_TYPE == __OP_BOOLEANPUSH__) {
			op.OP_VALUE = new char[1];
			op.OP_VALUE[0] = word == "true" || stod(word) > 0 ? 1 : 0;
		}
		else if (OP_TYPE == __OP_VARPUSH__) { // generic 'var', but not char nor string
			op.OP_VALUE = StrToCharPointer(word);
		}
		else { // else: it''s a number value

			// TODO: 128bit support isn't quite good as proposed initually, needs refinement
			op.OP_VALUE = (char*)malloc(memSize);

			// INTEGER
			if (OP_TYPE == __OP_INT8PUSH__) {
				t_int8 int8;
				int8.i = (short)stod(word);
				*op.OP_VALUE = int8.c;
			}
			else if (OP_TYPE == __OP_INT16PUSH__) {
				t_int16 int16;
				int16.i = (short)stod(word);
				op.OP_VALUE = int16.c;
			}
			else if (OP_TYPE == __OP_INT32PUSH__) {
				t_int32 int32;
				int32.i = (int)stod(word);
				op.OP_VALUE = int32.c;
			}
			else if (OP_TYPE == __OP_INT64PUSH__) {
				t_int64 int64;
				int64.i = (long)stod(word);
				op.OP_VALUE = int64.c;
			}
			else if (OP_TYPE == __OP_INT128PUSH__) {
				t_int128 int128;
				int128.i = (long long)stod(word);
				op.OP_VALUE = int128.c;
			}
			// FLOAT
			else if (OP_TYPE == __OP_FLOAT16PUSH__) {
				t_float16 float16;
				float16.f = (float)StrToLongDouble(word);
				op.OP_VALUE = float16.c;
			}
			else if (OP_TYPE == __OP_FLOAT32PUSH__) {
				t_float32 float32;
				float32.f = (float)StrToLongDouble(word);
				op.OP_VALUE = float32.c;
			}
			else if (OP_TYPE == __OP_FLOAT64PUSH__) {
				t_float64 float64;
				float64.f = (double)StrToLongDouble(word);
				op.OP_VALUE = float64.c;
			}
			else if (OP_TYPE == __OP_FLOAT128PUSH__) {
				t_float128 float128;
				float128.f = StrToLongDouble(word);
				op.OP_VALUE = float128.c;
			}
		}
	}

	return op;
}

/***************************************/

/*		 MAIN PROGRAM		*/
int main()
{
	const regex r("((\\+|-)?[[:d:]]+)(\\.(([[:d:]]+)?))?");
	fstream file("../../../sample2-multiple_types.cw", ios::in); // ios::out | ios::trunc | ios::app

	if (file.is_open()) {
		string line = "";
		//string word = "";
		long countLines = 0;
		list<operation> _Stack_; // the stack of programm operations

		while (getline(file, line)) {
			countLines++;
			if (line.empty()) { // if it's an empty line, just jump to the next one
				continue;
			}
			replace(line.begin(), line.end(), '\t', *StrToCharPointer("    "));
			while (line[0] == '\0' || line[0] == ' ') {
				line = line.substr(1, line.size());
			}
			
			string word = GetToken(StrToCharPointer(line), ' ', true);
			while (word != "") {
				if (word[0] == ';' || word == "") { // if it's a comment or empty line, just jump to the next one
					break;
				}

				/*				---	TYPE PUSH ---				*/

				if (word == "int8") {
					operation op = OpPush(line, word, countLines, __OP_INT8PUSH__, 1);
					_Stack_.push_back(op);
				}

				else if (word == "int16") {
					operation op = OpPush(line, word, countLines, __OP_INT16PUSH__, 2);
					_Stack_.push_back(op);
				}

				else if (word == "int32" || word == "int") {
					operation op = OpPush(line, word, countLines, __OP_INT32PUSH__, 4);
					_Stack_.push_back(op);
				}

				else if (word == "int64") {
					operation op = OpPush(line, word, countLines, __OP_INT64PUSH__, 8);
					_Stack_.push_back(op);
				}

				else if (word == "int128") {
					operation op = OpPush(line, word, countLines, __OP_INT128PUSH__, 16);
					_Stack_.push_back(op);
				}

				else if (word == "float16") {
					operation op = OpPush(line, word, countLines, __OP_FLOAT16PUSH__, 2);
					_Stack_.push_back(op);
				}
				
				else if (word == "float32" || word == "float") {
					operation op = OpPush(line, word, countLines, __OP_FLOAT32PUSH__, 4);
					_Stack_.push_back(op);
				}

				else if (word == "float64") {
					operation op = OpPush(line, word, countLines, __OP_FLOAT64PUSH__, 8);
					_Stack_.push_back(op);
				}

				else if (word == "float128") {
					operation op = OpPush(line, word, countLines, __OP_FLOAT128PUSH__, 16);
					_Stack_.push_back(op);
				}

				else if (word == "bool") {
					operation op = OpPush(line, word, countLines, __OP_BOOLEANPUSH__, 1);
					_Stack_.push_back(op);
				}

				else if (word == "var") {
					operation op = OpPush(line, word, countLines, __OP_VARPUSH__, word.size());
					_Stack_.push_back(op);
				}

				/*****************************************************/

				/*				---	CONDITIONAL ---				*/

				else if (word == "if") {
					operation popop;
					popop.OP_TYPE = __OP_VARPOP__;
					word = GetToken(line, ' '); // reads the variable's name
					if (word == "") {
						cout << "--- syntax error in line (" << countLines + 1 << "): '" << line << "'; <-- expected variable name for conditional test definition.\n";
						exit(1);
					}
					popop.OP_LABEL = StrToCharPointer(word);
					popop.OP_VALUE = StrToCharPointer(word);
					_Stack_.push_back(popop);

					operation condop;
					word = GetToken(line, ' '); // reads the conditional operation
					if (word == "") {
						cout << "--- syntax error in line (" << countLines + 1 << "): '" << line << "'; <-- expected a conditional operation.\n";
						exit(1);
					}
					
					condop.OP_LABEL = new char[4];
					if (word == "==") {
						condop.OP_TYPE = __OP_EQUALS__;
						condop.OP_LABEL = StrToCharPointer("eq");
						condop.OP_VALUE = StrToCharPointer("eq");
					}
					else if (word == "!=") {
						condop.OP_TYPE = __OP_NOTEQUALS__;
						condop.OP_LABEL = StrToCharPointer("ne");
						condop.OP_VALUE = StrToCharPointer("ne");
					}
					else if (word == ">") {
						condop.OP_TYPE = __OP_GREATER__;
						condop.OP_LABEL = StrToCharPointer("gt");
						condop.OP_VALUE = StrToCharPointer("gt");
					}
					else if (word == ">=") {
						condop.OP_TYPE = __OP_GREATEROREQUAL__;
						condop.OP_LABEL = StrToCharPointer("ge");
						condop.OP_VALUE = StrToCharPointer("ge");
					}
					else if (word == "<") {
						condop.OP_TYPE = __OP_LESS__;
						condop.OP_LABEL = StrToCharPointer("lt");
						condop.OP_VALUE = StrToCharPointer("lt");
					}
					else if (word == "<=") {
						condop.OP_TYPE = __OP_LESSOREQUAL__;
						condop.OP_LABEL = StrToCharPointer("le");
						condop.OP_VALUE = StrToCharPointer("le");
					}
					else {
						cout << "--- syntax error in line (" << countLines + 1 << "): '" << line << "'; <-- invalid conditional operation.\n";
						exit(1);
					}

					operation poporconstop;
					word = GetToken(line, ' '); // reads the second term for conditional testing
					if (word == "") {
						cout << "--- syntax error in line (" << countLines + 1 << "): '" << line << "'; <-- expected variable name or a constant for conditional test definition.\n";
						exit(1);
					}
					// defines if it is a number or a variable's name
					if (regex_match(word, r)) {
						poporconstop.OP_TYPE = __OP_CONSTDEF__;
						poporconstop.OP_LABEL = StrToCharPointer("Const");
;						poporconstop.OP_VALUE = StrToCharPointer(word); // the const value is set
					}
					else {
						poporconstop.OP_TYPE = __OP_VARPOP__;
						poporconstop.OP_LABEL = StrToCharPointer(word); // tha variable's name is saved
					}
					_Stack_.push_back(poporconstop);

					_Stack_.push_back(condop); // only added after the addition of the 2 variables to be tested

					// lastly, opens the if block
					operation ifop;
					ifop.OP_TYPE = __OP_IFSTART__;
					ifop.OP_LABEL = StrToCharPointer("If");
					ifop.OP_VALUE = StrToCharPointer("If");
					_Stack_.push_back(ifop);
				}

				else if (word == "end") {
					operation op;
					op.OP_TYPE = __OP_END__;
					op.OP_LABEL = StrToCharPointer("End");
					op.OP_VALUE = StrToCharPointer("End");
					_Stack_.push_back(op);
				}

				else if (word == "else") {
					// firstly closes the block before
					operation op;
					op.OP_TYPE = __OP_END__;
					op.OP_LABEL = StrToCharPointer("End");
					op.OP_VALUE= StrToCharPointer("End");
					_Stack_.push_back(op);

					operation elseop;
					elseop.OP_TYPE = __OP_ELSE__;
					elseop.OP_LABEL = StrToCharPointer("Else");
					elseop.OP_VALUE = StrToCharPointer("Else");
					_Stack_.push_back(elseop);
				}

				else if (word == "elsif") {
					// firstly closes the block before
					operation op;
					op.OP_TYPE = __OP_END__;
					op.OP_LABEL = StrToCharPointer("End");
					op.OP_VALUE = StrToCharPointer("End");
					_Stack_.push_back(op);

					// then creates a new If-like operations sequence
					operation popop;
					popop.OP_TYPE = __OP_VARPOP__;
					word = GetToken(line, ' '); // reads the variable's name
					if (word == "") {
						cout << "--- syntax error in line (" << countLines + 1 << "): '" << line << "'; <-- expected variable name for conditional test definition.\n";
						exit(1);
					}
					popop.OP_LABEL = StrToCharPointer(word);
					popop.OP_VALUE = StrToCharPointer(word);
					_Stack_.push_back(popop);

					operation condop;
					word = GetToken(line, ' '); // reads the conditional operation
					if (word == "") {
						cout << "--- syntax error in line (" << countLines + 1 << "): '" << line << "'; <-- expected a conditional operation.\n";
						exit(1);
					}

					condop.OP_LABEL = new char[4];
					if (word == "==") {
						condop.OP_TYPE = __OP_EQUALS__;
						condop.OP_LABEL = StrToCharPointer("eq");
						condop.OP_VALUE = StrToCharPointer("eq");
					}
					else if (word == "!=") {
						condop.OP_TYPE = __OP_NOTEQUALS__;
						condop.OP_LABEL = StrToCharPointer("ne");
						condop.OP_VALUE = StrToCharPointer("ne");
					}
					else if (word == ">") {
						condop.OP_TYPE = __OP_GREATER__;
						condop.OP_LABEL = StrToCharPointer("gt");
						condop.OP_VALUE = StrToCharPointer("gt");
					}
					else if (word == ">=") {
						condop.OP_TYPE = __OP_GREATEROREQUAL__;
						condop.OP_LABEL = StrToCharPointer("ge");
						condop.OP_VALUE = StrToCharPointer("ge");
					}
					else if (word == "<") {
						condop.OP_TYPE = __OP_LESS__;
						condop.OP_LABEL = StrToCharPointer("lt");
						condop.OP_VALUE = StrToCharPointer("lt");
					}
					else if (word == "<=") {
						condop.OP_TYPE = __OP_LESSOREQUAL__;
						condop.OP_LABEL = StrToCharPointer("le");
						condop.OP_VALUE = StrToCharPointer("le");
					}
					else {
						cout << "--- syntax error in line (" << countLines + 1 << "): '" << line << "'; <-- invalid conditional operation.\n";
						exit(1);
					}

					operation poporconstop;
					word = GetToken(line, ' '); // reads the second term for conditional testing
					if (word == "") {
						cout << "--- syntax error in line (" << countLines + 1 << "): '" << line << "'; <-- expected variable name or a constant for conditional test definition.\n";
						exit(1);
					}
					// defines if it is a number or a variable's name
					if (regex_match(word, r)) {
						poporconstop.OP_TYPE = __OP_CONSTDEF__;
						poporconstop.OP_LABEL = StrToCharPointer("Const");
						poporconstop.OP_VALUE = StrToCharPointer(word); // the const value is set
					}
					else {
						poporconstop.OP_TYPE = __OP_VARPOP__;
						poporconstop.OP_LABEL = StrToCharPointer(word); // tha variable's name is saved
					}
					_Stack_.push_back(poporconstop);

					_Stack_.push_back(condop); // only added after the addition of the 2 variables to be tested

					// lastly, opens the if block
					operation ifop;
					ifop.OP_TYPE = __OP_IFSTART__;
					ifop.OP_LABEL = StrToCharPointer("If");
					ifop.OP_VALUE = StrToCharPointer("If");
					_Stack_.push_back(ifop);
				}

				/*****************************************************/

				/*
					[X] tries to find the name in the programm operations
					[X] if it doesn't: exits program with code 1 and saying that used variable without declaring it first
					[X] otherwise goes ahead and updates the variable's value - needs to be a new operation
				*/

				/*			---	TYPE ATTRIBUTION ---			*/
				else {
					operation varDefinition;
					auto op = _Stack_.begin();
					for (int i = 0; i < _Stack_.size(); i++) {
						if (op->OP_LABEL != 0) {
							if (word == op->OP_LABEL) {
								varDefinition = *op;
								break;
							}
						}
						advance(op, 1);
					}
					if (varDefinition.OP_LABEL == 0) {
						cout << "--- compile error in line (" << countLines + 1 << "): '" << line << "'; <-- variable name '" << word << "' not declared.\n";
						exit(1);
					}

					operation varsetop;
					varsetop.OP_TYPE = __OP_VARSETVAL__;
					string label = varDefinition.OP_LABEL;
					varsetop.OP_LABEL = StrToCharPointer(label);

					word = GetToken(line, ' '); // reads the value to reset
					if (word == "") {
						cout << "--- syntax error in line (" << countLines + 1 << "): '" << line << "'; <-- expected value to be set on variable '" << varsetop.OP_LABEL << "'.\n";
						exit(1);
					}
					string val = varDefinition.OP_VALUE;
					varsetop.OP_VALUE = (char*)malloc(val.size());
					varsetop.OP_VALUE = StrToCharPointer(val);
				}
				/*****************************************************/

				// automatically jumps to the next line when encounters a comment
				bool hasComment = false;
				for (int i = itIndex; i <= line.size(); i++) {
					if (line[i] == ';') {
						hasComment = true;
						break;
					}
				}
				
				if (!hasComment && itIndex == sizeof(line)) {
					word = GetToken(line, ' ', true);
				}
				else {
					word = GetToken(line, ' ');
				}
			}
		}

		cout << "Stack: \n\n";
		int count = 0;
		_Stack_.begin();
		while(_Stack_.size()) {
			operation op = _Stack_.front();
			cout << "   #" << count++ << "   OP_TYPE = " << op.OP_TYPE << "   OP_LABEL = " << op.OP_LABEL;
			cout << "   OP_VALUE = " << op.OP_VALUE << "\n\n";
			_Stack_.pop_front();
		}
		
		file.close();
	}
	else {
		cout << "file could not be opened...\n\n";
		exit(1);
	}

	cout << "file compiled successfully!\n\n";

	return 0;
}
