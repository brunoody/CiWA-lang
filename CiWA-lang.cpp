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
const short __OP_IF__ =					IOTA();
const short __OP_THEN__ =				IOTA();
const short __OP_ELSE__ =				IOTA();
const short __OP_ELSIF__ =				IOTA();
const short __OP_WHILE__ =				IOTA();
const short __OP_FOR__ =				IOTA();
const short __OP_END__ =				IOTA();
const short __OP_PRINTLN__ =			IOTA();

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
	char* OP_LABEL = NULL; // the address label or typename for the WebAssembly code
	char* OP_VALUE = NULL; // can be any size of byte[], basically
	bool  OP_TEXTFLAG = false; // defines if it's a text variable
	bool  OP_BLOCK_CLOSED = false; // for block openers, to know when they were already closed
};

typedef struct float_parsed {
	string intPart = "";
	string floatPart = "";
	unsigned char floatPoints;
};

typedef union t_int8 {
	char i;
	char c[1];
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
	long long i;
	char c[8];
};

typedef struct s__float16__ {
	char p1[2];
	char p2[2];
};

typedef union t_float16 {
	float f;
	s__float16__ c;
};

typedef union t_float32{
	float f;
	char c[4];
};

typedef union t_float64 {
	long double f;
	char c[8];
};

/*	128bit INT	*/
typedef struct s__int128__ {
	long long i_hi;
	long long i_lo;
};
typedef union t_int128 {
	s__int128__ s_val;
	char val[16];
};
t_int128 CreateInt128() {
	t_int128 hi_lo;
	hi_lo.s_val.i_hi = hi_lo.s_val.i_lo = 0;
	return hi_lo;
}

/*	128bit FLOAT  */
typedef struct s__float128__ {
	long double f_hi;
	long double f_lo;
};
typedef union t_float128 {
	s__float128__ s_val;
	char val[16];
};
t_float128 CreateFloat128() {
	t_float128 hi_lo;
	hi_lo.s_val.f_hi = hi_lo.s_val.f_lo = 0;
	return hi_lo;
}
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

unsigned char* StrToUnsignedCharPointer(string str, bool addNullChar = false) {
	unsigned char* ret = 0;
	if (addNullChar) {
		ret = new unsigned char[str.size() + 1];
		copy(str.begin(), str.end(), ret);
		ret[str.size()] = '\0'; // 'end char'
	}
	else {
		ret = new unsigned char[str.size()];
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
	parse.floatPoints = floatPoints;
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
	long double ret = longLongPart + (long double)(floatPartAsLongLong / divisor);
	return ret;
}

t_int128 StrToInt128(string str) {
	t_int128 ret = CreateInt128();
	unsigned short* cVals = new unsigned short[str.size()];
	string c = "";
	for (short i = str.size()-1; i >= 0; i--) {
		cVals[i] = stod(str.substr(i, 1));
	}

	unsigned long long multi10 = 1;
	bool hasReachedMaxLo = false;
	unsigned long long diff = 0;
	for (short n = str.size()-1; n >= 0; n--) {
		if ( (ret.s_val.i_lo + cVals[n]*multi10) >= 0xFFFFFFFFFFFFFFFF ) {
			ret.s_val.i_lo = 0xFFFFFFFFFFFFFFFF;
			diff = 9 - cVals[n];
			if (diff < 0) {
				diff = -diff;
			}
			hasReachedMaxLo = true;
			multi10 = 1;
			continue;
		}

		if (!hasReachedMaxLo) {
			ret.s_val.i_lo += cVals[n]*multi10;
		}
		else {
			ret.s_val.i_hi += ((cVals[n]+diff) * multi10);
			diff = 0;
		}
		multi10 *= 10;
	}

	delete cVals;
	return ret;
}

t_float128 StrToFloat128(string str) {
	t_int128 iPart = StrToInt128(ParseFloat(str).intPart);
	t_int128 fPart = StrToInt128(ParseFloat(str).floatPart);
	double divisor = 10.0;
	for (int i = 1; i < ParseFloat(str).floatPoints; i++) {
		divisor = divisor * 10.0;
	}
	t_float64 f64;
	f64.f = (fPart.s_val.i_lo / divisor);
	// TODO : must test if the value has overflown the 64bit size to start inserting into the low part
	// needs 4bit shift-right because of quadruple precision IEEE
	// TODO : review data precision and calculations theory for quadruple-precision float
	t_float128 ret = CreateFloat128();
	ret.s_val.f_lo = iPart.s_val.i_lo + f64.f;

	return ret;
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
			if (countChars == line.size()) {
				return "";
			}
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
operation OpPush(string line, string word, long lineNumber, short OP_TYPE) {
	// TODO : after returning the new OP and adding to the Stack, it screws the variable values
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
		if (word == "") {
			cout << "--- syntax error in line (" << lineNumber << "): '" << line << "'; <-- text variable unclosed.\n";
			exit(1);
		}
		op.OP_VALUE = StrToCharPointer(word);
		op.OP_TEXTFLAG = true;
	}
	else {
		if (OP_TYPE == __OP_BOOLEANPUSH__) {
			op.OP_VALUE = new char[1];
			op.OP_VALUE[0] = word == "true" || stod(word) > 0 ? 1 : 0;
		}
		else if (OP_TYPE == __OP_VARPUSH__) { // generic 'var', but not char nor string
			op.OP_VALUE = StrToCharPointer(word);
		}
		else { // otherwise, it's a number value
			// INTEGER
			if (OP_TYPE == __OP_INT8PUSH__) {
				t_int8 int8;
				int8.i = (char)stod(word);
				op.OP_VALUE = int8.c;
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
				int64.i = (long long)stod(word);
				op.OP_VALUE = int64.c;
			}
			else if (OP_TYPE == __OP_INT128PUSH__) {
				t_int128 int128 = StrToInt128(word);
				op.OP_VALUE = int128.val;
			}
			// FLOAT
			else if (OP_TYPE == __OP_FLOAT16PUSH__) {
				t_float16 float16;
				float16.f = (float)StrToLongDouble(word);
				op.OP_VALUE = float16.c.p2;
			}
			else if (OP_TYPE == __OP_FLOAT32PUSH__) {
				t_float32 float32;
				float32.f = (float)StrToLongDouble(word);
				op.OP_VALUE = float32.c;
			}
			else if (OP_TYPE == __OP_FLOAT64PUSH__) {
				t_float64 float64;
				float64.f = (long double)StrToLongDouble(word);
				op.OP_VALUE = float64.c;
			}
			else if (OP_TYPE == __OP_FLOAT128PUSH__) {
				t_float128 float128 = StrToFloat128(word);
				op.OP_VALUE =float128.val;
			}
			// UNREACHABLE
			else {
				cout << "--- compilation error in line (" << lineNumber << "): '" << line << "'; <-- type mismatch on variable '" << op.OP_LABEL << "'.\n";
				exit(1);
			}
		}
	}

	return op;
}

void IfPush(list<operation> &_Stack_, string line, string word, long countLines, regex r) {
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
		condop.OP_LABEL = StrToCharPointer("eq_s");
		condop.OP_VALUE = StrToCharPointer("eq_s");
	}
	else if (word == "!=") {
		condop.OP_TYPE = __OP_NOTEQUALS__;
		condop.OP_LABEL = StrToCharPointer("ne_s");
		condop.OP_VALUE = StrToCharPointer("ne_s");
	}
	else if (word == ">") {
		condop.OP_TYPE = __OP_GREATER__;
		condop.OP_LABEL = StrToCharPointer("gt_s");
		condop.OP_VALUE = StrToCharPointer("gt_s");
	}
	else if (word == ">=") {
		condop.OP_TYPE = __OP_GREATEROREQUAL__;
		condop.OP_LABEL = StrToCharPointer("ge_s");
		condop.OP_VALUE = StrToCharPointer("ge_s");
	}
	else if (word == "<") {
		condop.OP_TYPE = __OP_LESS__;
		condop.OP_LABEL = StrToCharPointer("lt_s");
		condop.OP_VALUE = StrToCharPointer("lt_s");
	}
	else if (word == "<=") {
		condop.OP_TYPE = __OP_LESSOREQUAL__;
		condop.OP_LABEL = StrToCharPointer("le_s");
		condop.OP_VALUE = StrToCharPointer("le_s");
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

	// opens the if block
	operation ifop;
	ifop.OP_TYPE = __OP_IF__;
	ifop.OP_LABEL = StrToCharPointer("If");
	ifop.OP_VALUE = StrToCharPointer("If");
	_Stack_.push_back(ifop);

	// opens the then block
	operation thenop;
	thenop.OP_TYPE = __OP_THEN__;
	thenop.OP_LABEL = StrToCharPointer("Then");
	thenop.OP_VALUE = StrToCharPointer("Then");
	_Stack_.push_back(thenop);
}

string DiscoverBlockToClose(list<operation>& _Stack_) {
	string closingBlock = "";

	auto op = _Stack_.begin();
	advance(op, _Stack_.size() - 1);
	for (int i = _Stack_.size() - 1; i >= 0; i--) {
		if (!op->OP_BLOCK_CLOSED &&
			(op->OP_TYPE == __OP_IF__ || op->OP_TYPE == __OP_WHILE__ || op->OP_TYPE == __OP_FOR__)
		) {
			closingBlock = op->OP_LABEL;
			op->OP_BLOCK_CLOSED = true;
			break;
		}
		op = _Stack_.begin();
		advance(op, i);
	}

	return closingBlock;
}

/***************************************/

/*		   TEST MAIN		*/
int Test () {

	cout << " Size of Base Struct = " << sizeof(s__int128__) << "\n";
	cout << " Size of Type i128 = " << sizeof(t_int128) << "\n";

	s__int128__ s_int; s_int.i_hi = s_int.i_lo = 0;
	t_int128 t_int = CreateInt128();

	cout << " Init value of Struct =>> hi = " << s_int.i_hi << " : lo = " << s_int.i_lo << "\n";
	cout << " Init value of Type i128 = " << t_int.val << "\n";

	cout << "\n\n";

	cout << " Size of Base Struct = " << sizeof(s__float128__) << "\n";
	cout << " Size of Type f128 = " << sizeof(t_float128) << "\n";

	s__float128__ s_float; s_float.f_hi = s_float.f_lo = 0.0;
	t_float128 t_float = CreateFloat128();

	cout << " Init value of Struct =>> hi = " << s_float.f_hi << " : lo = " << s_float.f_lo << "\n";
	cout << " Init value of Type f128 = " << t_float.val << "\n";
	
	return 0;
}

/*		 FILE PARSER		*/
list<operation> ParseFileToWasmStack (bool printStack = true)
{
	const regex r("((\\+|-)?[[:d:]]+)(\\.(([[:d:]]+)?))?");
	fstream file("../../../sample2-multiple_types.cw", ios::in); // ios::out | ios::trunc | ios::app
	short countElsif = 0;
	list<operation> _Stack_; // the stack of programm operations

	if (file.is_open()) {
		string line = "";
		long countLines = 0;

		while (getline(file, line)) {
			countLines++;
			if (line.empty()) { // if it's an empty line, just jump to the next one
				continue;
			}
			replace(line.begin(), line.end(), '\t', *StrToCharPointer("    "));
			while (line[0] == '\0' || line[0] == ' ') {
				line = line.substr(1, line.size());
			}

			// PARSE the file
			string word = GetToken(StrToCharPointer(line), ' ', true);
			while (word != "") {
				if (word[0] == ';' || word == "") { // if it's a comment or empty line, just jump to the next one
					break;
				}

				/*				---	TYPE PUSH ---				*/

				if (word == "int8") {
					operation op = OpPush(line, word, countLines, __OP_INT8PUSH__);
					_Stack_.push_back(op);
				}

				else if (word == "int16") {
					operation op = OpPush(line, word, countLines, __OP_INT16PUSH__);
					_Stack_.push_back(op);
				}

				else if (word == "int32" || word == "int") {
					operation op = OpPush(line, word, countLines, __OP_INT32PUSH__);
					_Stack_.push_back(op);
				}

				else if (word == "int64") {
					operation op = OpPush(line, word, countLines, __OP_INT64PUSH__);
					_Stack_.push_back(op);
				}

				else if (word == "int128") {
					operation op = OpPush(line, word, countLines, __OP_INT128PUSH__);
					_Stack_.push_back(op);
				}

				else if (word == "float16") {
					operation op = OpPush(line, word, countLines, __OP_FLOAT16PUSH__);
					_Stack_.push_back(op);
				}

				else if (word == "float32" || word == "float") {
					operation op = OpPush(line, word, countLines, __OP_FLOAT32PUSH__);
					_Stack_.push_back(op);
				}

				else if (word == "float64") {
					operation op = OpPush(line, word, countLines, __OP_FLOAT64PUSH__);
					_Stack_.push_back(op);
				}

				else if (word == "float128") {
					operation op = OpPush(line, word, countLines, __OP_FLOAT128PUSH__);
					_Stack_.push_back(op);
				}

				else if (word == "bool") {
					operation op = OpPush(line, word, countLines, __OP_BOOLEANPUSH__);
					_Stack_.push_back(op);
				}

				else if (word == "var") {
					operation op = OpPush(line, word, countLines, __OP_VARPUSH__);
					_Stack_.push_back(op);
				}

				/*****************************************************/

				/*				---	KEYWORDS ---				*/

				else if (word == "if") {
					IfPush(_Stack_, line, word, countLines, r);
				}

				// need to iterate through the _Stack_ and discover which block isn't closed yet, then
				// TODO: needs to check whether a block isn't closed till the end of the code to throw a compiler error
				else if (word == "end") {
					string closingBlock = DiscoverBlockToClose(_Stack_);

					if (closingBlock == "If")
					{
						// for each 'elsif' before the 'end' token, the compiler must add another 2 end-closures -> '))'
						for (int i = 0; i < countElsif; i++) {
							operation endop;
							endop.OP_TYPE = __OP_END__;
							endop.OP_LABEL = StrToCharPointer("End");
							endop.OP_VALUE = StrToCharPointer("End");

							_Stack_.push_back(endop);
							_Stack_.push_back(endop);
						}
						countElsif = 0;

						// Each End-statement has to close one of either options:
						//		a. If + Then
						//		b. Else + its If
						// Conclusion: always has to close 2 times (for If-Elsif-Else-blocks) -> '))'
						operation endop;
						endop.OP_TYPE = __OP_END__;
						endop.OP_LABEL = StrToCharPointer("End");
						endop.OP_VALUE = StrToCharPointer("End");

						_Stack_.push_back(endop);
						_Stack_.push_back(endop);
					}
					else if (closingBlock == "While") {
						// TODO
					}
					else if (closingBlock == "For") {
						// TODO
					}
				}

				else if (word == "else") {
					// Else-statements don't close the If-statements before them, but they must close the Then-blocks
					operation op;
					op.OP_TYPE = __OP_END__;
					op.OP_LABEL = StrToCharPointer("End");
					op.OP_VALUE = StrToCharPointer("End");
					_Stack_.push_back(op);

					operation elseop;
					elseop.OP_TYPE = __OP_ELSE__;
					elseop.OP_LABEL = StrToCharPointer("Else");
					elseop.OP_VALUE = StrToCharPointer("Else");
					_Stack_.push_back(elseop);
				}

				else if (word == "elsif") {
					countElsif++; // add 1 to the counter

					// firstly closes the Then-block before
					operation op;
					op.OP_TYPE = __OP_END__;
					op.OP_LABEL = StrToCharPointer("End");
					op.OP_VALUE = StrToCharPointer("End");
					_Stack_.push_back(op);

					// then opens the Else-block inside the If-statement
					operation elseop;
					elseop.OP_TYPE = __OP_ELSE__;
					elseop.OP_LABEL = StrToCharPointer("Else");
					elseop.OP_VALUE = StrToCharPointer("Else");
					_Stack_.push_back(elseop);

					// then creates a new If-like operations sequence
					IfPush(_Stack_, line, word, countLines, r);
				}

				/*****************************************************/

				/*				---	OTHER ---				*/

				else if (word == "println") {
					operation op;
					op.OP_TYPE = __OP_PRINTLN__;
					op.OP_LABEL = StrToCharPointer("println");

					word = GetToken(line, ' ');
					if (word == "") {
						cout << "--- syntax error in line (" << countLines + 1 << "): '" << line << "'; <-- expected value to be printed.\n";
						exit(1);
					}

					if (word[0] == '"' || word[0] == '\'') { // generic 'var', strings and character values
						word = GetVarchar(line, word);
						if (word == "") {
							cout << "--- syntax error in line (" << countLines << "): '" << line << "'; <-- text variable unclosed.\n";
							exit(1);
						}
						op.OP_VALUE = StrToCharPointer(word);
						op.OP_TEXTFLAG = true;
					}
					else {
						op.OP_VALUE = StrToCharPointer(word);
					}

					_Stack_.push_back(op);
				}

				/*****************************************************/

				/*
					[X] tries to find the name in the programm operations
					[X] if it doesn't: exits program with code 1 and saying that used variable without declaring it first
					[X] otherwise goes ahead and updates the variable's value - needs to be a new operation
				*/

				/*			---	VAR REATTRIBUTION ---			*/
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

					// tests if it's either a generic var reattribution or another type
					if (varDefinition.OP_TYPE == __OP_VARPUSH__) { // generic 'var', any value or type accepted
						if (word[0] == '"' || word[0] == '\'') { // if it's a text value...
							word = GetVarchar(line, word);
							if (word == "") {
								cout << "--- syntax error in line (" << countLines << "): '" << line << "'; <-- text variable unclosed.\n";
								exit(1);
							}
							varsetop.OP_VALUE = StrToCharPointer(word);
							varsetop.OP_TEXTFLAG = true;		// forces the var to be a text one, if it wasn't
						}
						else if (word == "true" || word == "false" || word == "null") { // for either boolean values or nullables
							varsetop.OP_VALUE = new char[1];
							varsetop.OP_VALUE[0] = word == "true" ? 1 : 0;
							varsetop.OP_TEXTFLAG = false;		// forces the var to not be a text one, if it was
						}
						else { // treats as numeric value
							if (word.find(".") != string::npos) {
								// it's a Float value
								t_float128 float128 = StrToFloat128(word);
								strcpy(varsetop.OP_VALUE, float128.val);
							}
							else {
								// Integer
								t_int128 int128 = StrToInt128(word);
								strcpy(varsetop.OP_VALUE, int128.val);
							}
							varsetop.OP_TEXTFLAG = false;		// forces the var to not be a text one, if it was
						}
					}
					else { // other types have to be reattributed accordingly
						if (word[0] == '"' || word[0] == '\'') { // if it is a generic 'var' when reattributing to another type...
							cout << "--- compilation error in line (" << countLines << "): '" << line << "'; <-- type mismatch on reset variable '" << varDefinition.OP_LABEL << "'.\n";
							exit(1);
						}
						varsetop.OP_TEXTFLAG = false; // forces the variable to not be a text one

						if (varDefinition.OP_TYPE == __OP_BOOLEANPUSH__) { // forces any number to boolean
							varsetop.OP_VALUE = new char[1];
							varsetop.OP_VALUE[0] = word == "true" || stod(word) > 0 ? 1 : 0;
						}

						// NUMBER REATTRIBUTIONS
						else {

							// INTEGER
							if (varDefinition.OP_TYPE == __OP_INT8PUSH__) {
								t_int8 int8;
								int8.i = (char)stod(word);
								varsetop.OP_VALUE = int8.c;
							}
							else if (varDefinition.OP_TYPE == __OP_INT16PUSH__) {
								t_int16 int16;
								int16.i = (short)stod(word);
								varsetop.OP_VALUE = int16.c;
							}
							else if (varDefinition.OP_TYPE == __OP_INT32PUSH__) {
								t_int32 int32;
								int32.i = (int)stod(word);
								varsetop.OP_VALUE = int32.c;
							}
							else if (varDefinition.OP_TYPE == __OP_INT64PUSH__) {
								t_int64 int64;
								int64.i = (long long)stod(word);
								varsetop.OP_VALUE = int64.c;
							}
							else if (varDefinition.OP_TYPE == __OP_INT128PUSH__) {
								t_int128 int128 = StrToInt128(word);
								varsetop.OP_VALUE = int128.val;
							}
							// FLOAT
							else if (varDefinition.OP_TYPE == __OP_FLOAT16PUSH__) {
								t_float16 float16;
								float16.f = (float)StrToLongDouble(word);
								varsetop.OP_VALUE = float16.c.p2;
							}
							else if (varDefinition.OP_TYPE == __OP_FLOAT32PUSH__) {
								t_float32 float32;
								float32.f = (float)StrToLongDouble(word);
								varsetop.OP_VALUE = float32.c;
							}
							else if (varDefinition.OP_TYPE == __OP_FLOAT64PUSH__) {
								t_float64 float64;
								float64.f = (long double)StrToLongDouble(word);
								varsetop.OP_VALUE = float64.c;
							}
							else if (varDefinition.OP_TYPE == __OP_FLOAT128PUSH__) {
								t_float128 float128 = StrToFloat128(word);
								varsetop.OP_VALUE = float128.val;
							}
							// UNREACHABLE
							else {
								cout << "--- compilation error in line (" << countLines << "): '" << line << "'; <-- type mismatch on reset variable '" << varDefinition.OP_LABEL << "'.\n";
								exit(1);
							}
						}
					}

					_Stack_.push_back(varsetop);
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

		if (printStack) {
			cout << "Stack: \n\n";
			auto op = _Stack_.begin();
			for (int i = 0; i < _Stack_.size(); i++) {
				cout << "   #" << i << "   OP_TYPE = " << op->OP_TYPE << "   OP_LABEL = " << op->OP_LABEL;
				cout << "   OP_VALUE = " << op->OP_VALUE << "\n\n";
				advance(op, 1);
			}
		}
		
		file.close();
	}
	else {
		cout << "file could not be opened...\n\n";
		exit(1);
	}

	cout << "file compiled successfully!\n\n";

	return _Stack_;
}

/*		 MAIN PROGRAM		*/
int main() {

	//Test();

	ParseFileToWasmStack();

	return 0;
}