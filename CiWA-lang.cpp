//
// CiWA-lang.cpp: Main file
//

#include "./CiWA-lang.h"
using namespace std;

#pragma region IOTA (ENUM-LIKE)

int _iota = 0;
int IOTA(bool reset = false) {
	if (reset) {
		_iota = 0;
	}
	else {
		_iota++;
	}
	return _iota;
}

#pragma endregion

#pragma region GLOBALS

typedef struct operation {
	int	  OP_TYPE = -1;
	char* OP_LABEL = NULL; // the address label or typename for the WebAssembly code
	char* OP_VALUE = NULL; // can be any size of byte[], basically
	char* OP_ORIGINAL_VALUE = NULL; // for storing v-strings value before interpolation
	bool  OP_TEXTFLAG = false; // defines if it's a text variable
	bool  OP_BLOCK_CLOSED = false; // for block openers, to know when they were already closed
};

typedef struct float_parsed {
	string intPart = "";
	string floatPart = "";
	unsigned char floatPoints = -1;
};

#define __MAXOPS__ 4096
int _Ops_Count_ = 0;
operation _Stack_[__MAXOPS__]; // the stack of programm operations

#pragma endregion

#pragma region CONSTANTS

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
const short __OP_MEMORY__ = 			IOTA();
const short __OP_VARSETVAL__ =			IOTA();

/*	 COMMANDS	*/
const short __OP_IF__ =					IOTA();
const short __OP_THEN__ =				IOTA();
const short __OP_ELSE__ =				IOTA();
const short __OP_ELSIF__ =				IOTA();
const short __OP_WHILE__ =				IOTA();
const short __OP_FOR__ =				IOTA();
const short __OP_LOOP__ =				IOTA();
const short __OP_END__ =				IOTA();
const short __OP_PRINTLN__ =			IOTA();

/*	 OPERATORS  */
const short __OP_EQUALS__ =				IOTA();
const short __OP_NOTEQUALS__ =			IOTA();
const short __OP_GREATER__ =			IOTA();
const short __OP_GREATEROREQUAL__ =		IOTA();
const short __OP_LESS__ =				IOTA();
const short __OP_LESSOREQUAL__ =		IOTA();

#pragma endregion

#pragma region TYPE DEFINITIONS

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

#pragma endregion

#pragma region GENERAL UTILS

unsigned long itIndex = 0;
unsigned long GetStringIterator() {
	return itIndex;
}

void ResetStringIterator() {
	itIndex = 0;
}

void MoveStringIterator(long value) {
	itIndex += value;
}

string GetToken(string str, char delim) {
	if (str == "") {
		return "";
	}

	int delimIndex = -1;
	for (int i = GetStringIterator(); i <= str.size(); i++) {
		if (str[i] == delim || str[i] == '\0') {
			delimIndex = i;
			break;
		}
	}

	if (delimIndex == -1) {
		return "";
	}

	string ret = str.substr(GetStringIterator(), (delimIndex - GetStringIterator()));
	MoveStringIterator((delimIndex - GetStringIterator()) + 1);
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
float_parsed ParseStrToFloat(string str) {
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
	long long longLongPart = stod(ParseStrToFloat(str).intPart);
	long long floatPartAsLongLong = stod(ParseStrToFloat(str).floatPart);
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

t_int128* StrToInt128(string str) {
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
	return &ret;
}

t_float128* StrToFloat128(string str) {
	t_int64 iPart; iPart.i = stod(ParseStrToFloat(str).intPart);
	t_int64 fPart; fPart.i = stod(ParseStrToFloat(str).floatPart);
	double divisor = 10.0;
	for (int i = 1; i < ParseStrToFloat(str).floatPoints; i++) {
		divisor = divisor * 10.0;
	}
	t_float64 f64;
	f64.f = (fPart.i / divisor);
	// TODO : must test if the value has overflown the 64bit size to start inserting into the low part
	// needs 4bit shift-right because of quadruple precision IEEE
	// TODO : review data precision and calculations theory for quadruple-precision float
	t_float128 ret = CreateFloat128();
	ret.s_val.f_lo = iPart.i + f64.f;

	return &ret;
}

string GetVarchar(string line, string token, long lineNumber) {
	MoveStringIterator(-token.size());
	char toCheck = '\0';
	short start = line.find(token);
	short countChars = 0;

	if (start == string::npos) {
		return "";
	}

	if (token[0] == '"') {
		toCheck = '"';
	}
	else if (token[0] == '\'') {
		toCheck = '\'';
	}

	if (toCheck != '\0') {
		char temp = '\0';
		do {
			countChars++;
			if (line[start+countChars] == '\0' || (countChars+start) == line.size()) {
				cout << "--- syntax error in line (" << lineNumber << "): '" << line << "'; <-- text variable unclosed.\n";
				exit(1);
			}
			temp = line[start+countChars];
		} while (temp != toCheck);
		MoveStringIterator(countChars);

		string ret = "";
		if(countChars-1 -start == 0) {
			return ret;
		}

		ret = line.substr(start+1, countChars-1);

		// if it's a v-string, the 'v' character comes together
		unsigned long itPos = GetStringIterator();
		if (line[itPos] == 'v') {
			MoveStringIterator(1);
			ret.insert(0, "<v-string>");
			ret.append("</v-string>");
		}

		return ret;
	}

	return "";
}

bool IsVString(string* text) {
	size_t pos1 = text->find("<v-string>");	// 10 characters-count
	size_t pos2 = text->find("</v-string>"); // 11 characters-count
	if (pos1 != string::npos && pos2 != string::npos) {
		text->replace(pos1, 10, "");
		text->replace(pos2-10, 11, "");

		return true;
	}
	return false;
}

std::tuple<long, long> VStringHasVariable(string text) {
	size_t pos1 = text.find("{");
	size_t pos2 = text.find("}");
	if (pos1 != string::npos && pos2 != string::npos) {
		return std::tuple<long, long>{pos1, pos2+1-pos1};
	}
	return std::tuple<long, long> {-1, -1};
}

void Trim(string* text) {
	size_t emptyPos = text->find(" ");
	while (emptyPos != string::npos) {
		text->replace(emptyPos, 1, "");
		emptyPos = text->find(" ");
	}
}

void TrimLeft(string* text, bool removeEOString = false) {
	replace(text->begin(), text->end(), '\t', *StrToCharPointer(" "));
	while ((*text)[0] == ' ' || ((*text)[0] == '\0' && removeEOString)) {
		*text = text->substr(1, text->size());
	}
}

#pragma endregion

#pragma region OPERATION UTILS

operation* FindOperationByLabel(string label, bool lastVal) {
	operation* ret = NULL;
	for (int i = 0; i < _Ops_Count_; i++) {
		operation* op = &_Stack_[i];
		if (op->OP_LABEL != NULL && label == op->OP_LABEL) {
			if (!lastVal) {
				return op;
			}
			ret = op;
		}
	}
	return ret;
}

void InterpolateVString(string line, long lineNumber, string* text) {
	auto variable = VStringHasVariable(*text);
	while (std::get<0>(variable) != -1) {
		// the text will be replaced from pos1 with size 'len' to the actual value of the variable, if found on the _Stack_
		long pos1 = std::get<0>(variable);
		long len = std::get<1>(variable);
		string label = text->substr(pos1 + 1, len - 2);
		Trim(&label);

		operation* varlastvalop = FindOperationByLabel(label, true);
		if (varlastvalop == NULL) {
			cout << "--- compile error in line (" << lineNumber + 1 << "): '" << line << "'; <-- variable name '" << label << "' not declared.\n";
			exit(1);
		}

		text->replace(pos1, len, varlastvalop->OP_VALUE);

		variable = VStringHasVariable(*text);
	}
}

void OpPush(string line, string word, long lineNumber, short OP_TYPE, short size) {
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
		word = GetVarchar(line, word, lineNumber);

		if (IsVString(&word)) {
			// while it has variables inside, then needs to find the variable in the _Stack_ and replace with its value
			op.OP_ORIGINAL_VALUE = StrToCharPointer(word);
			InterpolateVString(line, lineNumber, &word);
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
			if (word == "null") {
				op.OP_VALUE = new char[1];
				op.OP_VALUE[0] = 0;
			}
			else {
				op.OP_VALUE = StrToCharPointer(word);
			}
		}
		else { // otherwise, it's a number value
			if (word == "null") {
				word = "0";
			}

			op.OP_VALUE = new char[size]; // ATTENTION!!! byte[] must be created here not to lose data information afterwards during the compilation
			// INTEGER
			if (OP_TYPE == __OP_INT8PUSH__) {
				t_int8 int8;
				int8.i = (char)stod(word);
				op.OP_VALUE[0] = *int8.c;
			}
			else if (OP_TYPE == __OP_INT16PUSH__) {
				t_int16 int16;
				int16.i = (short)stod(word);
				*op.OP_VALUE = *int16.c;
			}
			else if (OP_TYPE == __OP_INT32PUSH__) {
				t_int32 int32;
				int32.i = (int)stod(word);
				*op.OP_VALUE = *int32.c;
			}
			else if (OP_TYPE == __OP_INT64PUSH__) {
				t_int64 int64;
				int64.i = (long long)stod(word);
				*op.OP_VALUE = *int64.c;
			}
			else if (OP_TYPE == __OP_INT128PUSH__) {
				t_int128* int128 = StrToInt128(word);
				*op.OP_VALUE = *int128->val;
			}
			// FLOAT
			else if (OP_TYPE == __OP_FLOAT16PUSH__) {
				t_float16 float16;
				float16.f = (float)StrToLongDouble(word);
				*op.OP_VALUE = *float16.c.p2;
			}
			else if (OP_TYPE == __OP_FLOAT32PUSH__) {
				t_float32 float32;
				float32.f = (float)StrToLongDouble(word);
				*op.OP_VALUE = *float32.c;
			}
			else if (OP_TYPE == __OP_FLOAT64PUSH__) {
				t_float64 float64;
				float64.f = (long double)StrToLongDouble(word);
				*op.OP_VALUE = *float64.c;
			}
			else if (OP_TYPE == __OP_FLOAT128PUSH__) {
				t_float128* float128 = StrToFloat128(word);
				*op.OP_VALUE = *float128->val;
			}
			// UNREACHABLE
			else {
				cout << "--- compilation error in line (" << lineNumber << "): '" << line << "'; <-- type mismatch on variable '" << op.OP_LABEL << "'.\n";
				exit(1);
			}
		}
	}

	_Stack_[_Ops_Count_] = op;
	_Ops_Count_++;
}

void IfPush(string line, string word, long countLines) {
	operation* popop = new operation;
	popop->OP_TYPE = __OP_VARPOP__;
	word = GetToken(line, ' '); // reads the variable's name
	if (word == "") {
		cout << "--- syntax error in line (" << countLines + 1 << "): '" << line << "'; <-- expected variable name for conditional test definition.\n";
		exit(1);
	}
	popop->OP_LABEL = StrToCharPointer(word);
	popop->OP_VALUE = StrToCharPointer(word);
	_Stack_[_Ops_Count_] = *popop;
	_Ops_Count_++;

	operation* condop = new operation;
	word = GetToken(line, ' '); // reads the conditional operation
	if (word == "") {
		cout << "--- syntax error in line (" << countLines + 1 << "): '" << line << "'; <-- expected a conditional operation.\n";
		exit(1);
	}

	condop->OP_LABEL = new char[4];
	if (word == "==") {
		condop->OP_TYPE = __OP_EQUALS__;
		condop->OP_LABEL = StrToCharPointer("eq_s");
		condop->OP_VALUE = StrToCharPointer("eq_s");
	}
	else if (word == "!=") {
		condop->OP_TYPE = __OP_NOTEQUALS__;
		condop->OP_LABEL = StrToCharPointer("ne_s");
		condop->OP_VALUE = StrToCharPointer("ne_s");
	}
	else if (word == ">") {
		condop->OP_TYPE = __OP_GREATER__;
		condop->OP_LABEL = StrToCharPointer("gt_s");
		condop->OP_VALUE = StrToCharPointer("gt_s");
	}
	else if (word == ">=") {
		condop->OP_TYPE = __OP_GREATEROREQUAL__;
		condop->OP_LABEL = StrToCharPointer("ge_s");
		condop->OP_VALUE = StrToCharPointer("ge_s");
	}
	else if (word == "<") {
		condop->OP_TYPE = __OP_LESS__;
		condop->OP_LABEL = StrToCharPointer("lt_s");
		condop->OP_VALUE = StrToCharPointer("lt_s");
	}
	else if (word == "<=") {
		condop->OP_TYPE = __OP_LESSOREQUAL__;
		condop->OP_LABEL = StrToCharPointer("le_s");
		condop->OP_VALUE = StrToCharPointer("le_s");
	}
	else {
		cout << "--- syntax error in line (" << countLines + 1 << "): '" << line << "'; <-- invalid conditional operation.\n";
		exit(1);
	}

	operation* poporconstop = new operation;
	word = GetToken(line, ' '); // reads the second term for conditional testing
	if (word == "") {
		cout << "--- syntax error in line (" << countLines + 1 << "): '" << line << "'; <-- expected variable name or a constant for conditional test definition.\n";
		exit(1);
	}
	// defines if the word token is a text value, a number or a variable's name
	if(word[0] == '"' || word[0] == '\'') {
		string token = GetVarchar(line, word, countLines);
		if (IsVString(&token)) {
			cout << "--- syntax error in line (" << countLines + 1 << "): '" << line << "'; <-- expected variable name or a constant for conditional test definition.\n" <<
					"\t--- additional information: v-strings are not considered constants.\n";
			exit(1);
		}

		// TODO : string constant case - create a memory space at the start of the _Stack_ to be referenced later on with its const index
		poporconstop->OP_TYPE = __OP_MEMORY__;
		poporconstop->OP_LABEL = StrToCharPointer("Memory");
		poporconstop->OP_VALUE = StrToCharPointer(word); // the const value is set
		poporconstop->OP_TEXTFLAG = true;
	}
	else {
		bool isNumber = true;
		bool foundPoint = false;
		for(int i = 0; i < word.length(); i++) {
			char c = word[i];
			if(c == '.' && !foundPoint) {
				foundPoint = true;
			}
			else if (c == '.' && foundPoint) {
				isNumber = false;
				break;
			}

			if(c != '.' && std::isdigit(c) == 0){
				isNumber = false;
				break;
			}
		}

		if(isNumber) {
			poporconstop->OP_TYPE = __OP_CONSTDEF__;
			poporconstop->OP_LABEL = StrToCharPointer("Const");
			poporconstop->OP_VALUE = StrToCharPointer(word); // the const value is set
		}
		else{
			poporconstop->OP_TYPE = __OP_VARPOP__;
			poporconstop->OP_LABEL = StrToCharPointer(word); // the variable's name is saved
		}
	}
	_Stack_[_Ops_Count_] = *poporconstop;
	_Ops_Count_++;

	_Stack_[_Ops_Count_] = *condop; // only added after the addition of the 2 variables to be tested
	_Ops_Count_++;

	// opens the if block
	operation* ifop = new operation;
	ifop->OP_TYPE = __OP_IF__;
	ifop->OP_LABEL = StrToCharPointer("If");
	ifop->OP_VALUE = StrToCharPointer("If");
	_Stack_[_Ops_Count_] = *ifop;
	_Ops_Count_++;

	// opens the then block
	operation* thenop = new operation;
	thenop->OP_TYPE = __OP_THEN__;
	thenop->OP_LABEL = StrToCharPointer("Then");
	thenop->OP_VALUE = StrToCharPointer("Then");
	_Stack_[_Ops_Count_] = *thenop;
	_Ops_Count_++;
}

string DiscoverBlockToClose() {
	string closingBlock = "";
	operation* op = NULL;

	for (int i = _Ops_Count_ - 1; i >= 0; i--) {
		op = &_Stack_[i];
		if (!op->OP_BLOCK_CLOSED &&
			(op->OP_TYPE == __OP_IF__ || op->OP_TYPE == __OP_WHILE__ || op->OP_TYPE == __OP_FOR__)
		) {
			closingBlock = op->OP_LABEL;
			op->OP_BLOCK_CLOSED = true;
			break;
		}
	}

	return closingBlock;
}

#pragma endregion

#pragma region TEST MAIN FUNCTION

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

#pragma endregion

#pragma region PARSER

void ParseFileToWasmStack (bool printStack = false) {
	fstream file("../../../sample3.cw", ios::in); // ios::out | ios::trunc | ios::app
	short countElsif = 0;

	if (file.is_open()) {
		string line = "";
		long countLines = 0;

		while (getline(file, line)) {
			countLines++;
			if (line.empty()) { // if it's an empty line, just jump to the next one
				continue;
			}
			
			TrimLeft(&line, true);
			ResetStringIterator();

			// PARSE the file
			string word = GetToken(StrToCharPointer(line), ' ');
			while (word != "") {
				if (word[0] == ';' || word == "") { // if it's a comment or empty line, just jump to the next one
					break;
				}

				#pragma region TYPE PUSH

				if (word == "int8") {
					OpPush(line, word, countLines, __OP_INT8PUSH__, 1);
				}

				else if (word == "int16") {
					OpPush(line, word, countLines, __OP_INT16PUSH__, 2);
				}

				else if (word == "int32") {
					OpPush(line, word, countLines, __OP_INT32PUSH__, 4);
				}

				else if (word == "int64" || word == "int") {
					OpPush(line, word, countLines, __OP_INT64PUSH__, 8);
				}

				else if (word == "int128") {
					OpPush(line, word, countLines, __OP_INT128PUSH__, 16);
				}

				else if (word == "float16") {
					OpPush(line, word, countLines, __OP_FLOAT16PUSH__, 2);
				}

				else if (word == "float32") {
					OpPush(line, word, countLines, __OP_FLOAT32PUSH__, 4);
				}

				else if (word == "float64" || word == "float") {
					OpPush(line, word, countLines, __OP_FLOAT64PUSH__, 8);
				}

				else if (word == "float128") {
					OpPush(line, word, countLines, __OP_FLOAT128PUSH__, 16);
				}

				else if (word == "bool") {
					OpPush(line, word, countLines, __OP_BOOLEANPUSH__, 0);
				}

				else if (word == "var") {
					OpPush(line, word, countLines, __OP_VARPUSH__, 0);
				}

				#pragma endregion

				#pragma region KEYWORDS

				else if (word == "=>") {
					continue;
				} 

				else if (word == "if") {
					IfPush(line, word, countLines);
				}

				else if (word == "else") {
					// Else-statements don't close the If-statements before them, but they must close the Then-blocks
					operation* op = new operation;
					op->OP_TYPE = __OP_END__;
					op->OP_LABEL = StrToCharPointer("End");
					op->OP_VALUE = StrToCharPointer("End");
					_Stack_[_Ops_Count_] = *op;
					_Ops_Count_++;

					operation* elseop = new operation;
					elseop->OP_TYPE = __OP_ELSE__;
					elseop->OP_LABEL = StrToCharPointer("Else");
					elseop->OP_VALUE = StrToCharPointer("Else");
					_Stack_[_Ops_Count_] = *elseop;
					_Ops_Count_++;
				}

				else if (word == "elsif") {
					countElsif++; // add 1 to the counter

					// firstly closes the Then-block before
					operation* op = new operation;
					op->OP_TYPE = __OP_END__;
					op->OP_LABEL = StrToCharPointer("End");
					op->OP_VALUE = StrToCharPointer("End");
					_Stack_[_Ops_Count_] =  *op;
					_Ops_Count_++;

					// then opens the Else-block inside the If-statement
					operation* elseop = new operation;
					elseop->OP_TYPE = __OP_ELSE__;
					elseop->OP_LABEL = StrToCharPointer("Else");
					elseop->OP_VALUE = StrToCharPointer("Else");
					_Stack_[_Ops_Count_] = *elseop;
					_Ops_Count_++;

					// then creates a new If-like operations sequence
					IfPush(line, word, countLines);
				}

				else if (word == "while") {
					// TODO : add the boolean test right after in the WebAssembly code - the test may have a const push or a local.get
					// TODO : also add the 'br 0' statement at the end of the loop to return to the start
					operation* blockop = new operation;
					blockop->OP_TYPE = __OP_WHILE__;
					blockop->OP_LABEL = StrToCharPointer("While-block");
					blockop->OP_VALUE = StrToCharPointer("Block");
					_Stack_[_Ops_Count_] = *blockop;
					_Ops_Count_++;

					operation* loopop = new operation;
					loopop->OP_TYPE = __OP_LOOP__;
					loopop->OP_LABEL = StrToCharPointer("While-loop");
					loopop->OP_VALUE = StrToCharPointer("Loop");
					_Stack_[_Ops_Count_] = *loopop;
					_Ops_Count_++;
				}

				else if (word == "for") {
					// TODO : add the boolean test right after in the WebAssembly code - the test may have a const push or a local.get
					// TODO : in a For loop, it will be needed to add the variable increment/decrement at the end of the loop
					// TODO : also add the 'br 0' statement at the end of the loop to return to the start
					operation* blockop = new operation;
					blockop->OP_TYPE = __OP_FOR__;
					blockop->OP_LABEL = StrToCharPointer("For-block");
					blockop->OP_VALUE = StrToCharPointer("Block");
					_Stack_[_Ops_Count_] = *blockop;
					_Ops_Count_++;

					operation* loopop = new operation;
					loopop->OP_TYPE = __OP_LOOP__;
					loopop->OP_LABEL = StrToCharPointer("For-loop");
					loopop->OP_VALUE = StrToCharPointer("Loop");
					_Stack_[_Ops_Count_] = *loopop;
					_Ops_Count_++;
				}

				// need to iterate through the _Stack_ and discover which block isn't closed yet, then
				// TODO : needs to check whether a block isn't closed till the end of the code to throw a compiler error
				//		- maybe do this in a second step, iterating through the _Stack_
				//		- it could be done always here, but would have a very poor performance
				else if (word == "end") {
					string closingBlock = DiscoverBlockToClose();

					if (closingBlock == "If") {
						operation* endop = new operation;
						endop->OP_TYPE = __OP_END__;
						endop->OP_LABEL = StrToCharPointer("End");
						endop->OP_VALUE = StrToCharPointer("End");

						// for each 'elsif' before the 'end' token, the compiler must add another 2 end-closures -> '))'
						for (int i = 0; i < countElsif; i++) {
							_Stack_[_Ops_Count_] = *endop;
							_Ops_Count_++;
							_Stack_[_Ops_Count_] = *endop;
							_Ops_Count_++;
						}
						countElsif = 0;

						// Each End-statement has to close one of either options:
						//		a. If + Then
						//		b. Else + its If
						// Conclusion: always has to close 2 times (for If-Elsif-Else-blocks) -> '))'
						_Stack_[_Ops_Count_] = *endop;
						_Ops_Count_++;
						_Stack_[_Ops_Count_] = *endop;
						_Ops_Count_++;
					}
					else if (closingBlock == "While" || closingBlock == "For") {
						// While and For loops are made with (block (loop ...)) structure, though to close them is just needed to add 2 end-closures -> '))' to the _Stack_
						operation* endop = new operation;
						endop->OP_TYPE = __OP_END__;
						endop->OP_LABEL = StrToCharPointer("End");
						endop->OP_VALUE = StrToCharPointer("End");

						_Stack_[_Ops_Count_] = *endop;
						_Ops_Count_++;
						_Stack_[_Ops_Count_] = *endop;
						_Ops_Count_++;
					}
				}

				#pragma endregion

				#pragma region LANGUAGE PRE-BUILT FUNCTIONS

				else if (word == "println") {
					operation* op = new operation;
					op->OP_TYPE = __OP_PRINTLN__;
					op->OP_LABEL = StrToCharPointer("println");

					word = GetToken(line, ' ');
					if (word == "") {
						cout << "--- syntax error in line (" << countLines + 1 << "): '" << line << "'; <-- expected value to be printed.\n";
						exit(1);
					}

					if (word[0] == '"' || word[0] == '\'') { // generic 'var', strings and character values
						word = GetVarchar(line, word, countLines);
						if (IsVString(&word)) {
							op->OP_ORIGINAL_VALUE = StrToCharPointer(word);
							InterpolateVString(line, countLines+1, &word);
						}
						op->OP_VALUE = StrToCharPointer(word);
						op->OP_TEXTFLAG = true;
					}
					else {
						op->OP_VALUE = StrToCharPointer(word);
					}

					_Stack_[_Ops_Count_] = *op;
					_Ops_Count_++;
				}

				#pragma endregion

                #pragma region VAR TYPE REATTRIBUTION

				/*
					[X] tries to find the name in the programm operations
					[X] if it doesn't: exits program with code 1 and saying that used variable without declaring it first
					[X] otherwise goes ahead and updates the variable's value - needs to be a new operation
				*/
				else {
					operation* varDefinition = FindOperationByLabel(word, false);
					if (varDefinition == NULL) {
						cout << "--- compile error in line (" << countLines + 1 << "): '" << line << "'; <-- variable name '" << word << "' not declared.\n";
						exit(1);
					}

					operation* varsetop = new operation;
					varsetop->OP_TYPE = __OP_VARSETVAL__;
					string label = varDefinition->OP_LABEL;
					varsetop->OP_LABEL = StrToCharPointer(label);

					word = GetToken(line, ' '); // reads the value to reset
					if (word == "") {
						cout << "--- syntax error in line (" << countLines + 1 << "): '" << line << "'; <-- expected value to be set on variable '" << varsetop->OP_LABEL << "'.\n";
						exit(1);
					}

					// tests if it's either a generic var reattribution or another type
					if (varDefinition->OP_TYPE == __OP_VARPUSH__) { // generic 'var', any value or type accepted
						if (word[0] == '"' || word[0] == '\'') { // if it's a text value...
							word = GetVarchar(line, word, countLines);
							if (IsVString(&word)) {
								varsetop->OP_ORIGINAL_VALUE = StrToCharPointer(word);
								InterpolateVString(line, countLines+1, &word);
							}
							varsetop->OP_VALUE = StrToCharPointer(word);
							varsetop->OP_TEXTFLAG = true;		// forces the var to be a text one, if it wasn't
						}
						else if (word == "true" || word == "false" || word == "null") { // for either boolean values or nullables
							varsetop->OP_VALUE = new char[1];
							varsetop->OP_VALUE[0] = word == "true" ? 1 : 0;
							varsetop->OP_TEXTFLAG = false;		// forces the var to not be a text one, if it was
						}
						else { // treats as 128bit numeric value
							bool isNumber = true;
							for (int i = 0; i < word.length(); i++) {
								char c = word[i];
								if (c != '.' && std::isdigit(c) == 0) {
									isNumber = false;
									break;
								}
							}
							if (!isNumber) {
								cout << "--- syntax error in line (" << countLines << "): '" << line << "'; <-- number value expected.\n";
								exit(1);
							}

							varsetop->OP_VALUE = new char[16];
							size_t posI = word.find(".");
							if (posI != string::npos) {
								string sub = word.substr(posI, word.size() - posI);
								if (sub.find('.') != string::npos) {
									cout << "--- syntax error in line (" << countLines << "): '" << line << "'; <-- floating point numbers should have only 1 point ('.').\n";
									exit(1);
								}

								// it's a Float value
								t_float128* float128 = StrToFloat128(word);
								*varsetop->OP_VALUE = *float128->val;
							}
							else {
								// Integer
								t_int128* int128 = StrToInt128(word);
								*varsetop->OP_VALUE = *int128->val;
							}
							varsetop->OP_TEXTFLAG = false;		// forces the var to not be a text one, if it was
						}
					}
					else { // other types have to be reattributed accordingly
						if (word[0] == '"' || word[0] == '\'') { // if it is a generic 'var' when reattributing to another type...
							cout << "--- compilation error in line (" << countLines << "): '" << line << "'; <-- type mismatch on reset variable '" << varDefinition->OP_LABEL << "'.\n";
							exit(1);
						}
						varsetop->OP_TEXTFLAG = false; // forces the variable to not be a text one

						if (varDefinition->OP_TYPE == __OP_BOOLEANPUSH__) { // forces any number to boolean
							varsetop->OP_VALUE = new char[1];
							varsetop->OP_VALUE[0] = word == "true" || stod(word) > 0 ? 1 : 0;
						}

						// NUMBER REATTRIBUTIONS
						else {
							if (word == "null") {
								word = "0";
							}

							// INTEGER
							if (varDefinition->OP_TYPE == __OP_INT8PUSH__) {
								t_int8 int8;
								int8.i = (char)stod(word);
								varsetop->OP_VALUE = new char[1];
								varsetop->OP_VALUE[0] = *int8.c;
							}
							else if (varDefinition->OP_TYPE == __OP_INT16PUSH__) {
								t_int16 int16;
								int16.i = (short)stod(word);
								varsetop->OP_VALUE = new char[2];
								*varsetop->OP_VALUE = *int16.c;
							}
							else if (varDefinition->OP_TYPE == __OP_INT32PUSH__) {
								t_int32 int32;
								int32.i = (int)stod(word);
								varsetop->OP_VALUE = new char[4];
								*varsetop->OP_VALUE = *int32.c;
							}
							else if (varDefinition->OP_TYPE == __OP_INT64PUSH__) {
								t_int64 int64;
								int64.i = (long long)stod(word);
								varsetop->OP_VALUE = new char[8];
								*varsetop->OP_VALUE = *int64.c;
							}
							else if (varDefinition->OP_TYPE == __OP_INT128PUSH__) {
								t_int128* int128 = StrToInt128(word);
								varsetop->OP_VALUE = new char[16];
								*varsetop->OP_VALUE = *int128->val;
							}
							// FLOAT
							else if (varDefinition->OP_TYPE == __OP_FLOAT16PUSH__) {
								t_float16 float16;
								float16.f = (float)StrToLongDouble(word);
								varsetop->OP_VALUE = new char[2];
								*varsetop->OP_VALUE = *float16.c.p2;
							}
							else if (varDefinition->OP_TYPE == __OP_FLOAT32PUSH__) {
								t_float32 float32;
								float32.f = (float)StrToLongDouble(word);
								varsetop->OP_VALUE = new char[4];
								*varsetop->OP_VALUE = *float32.c;
							}
							else if (varDefinition->OP_TYPE == __OP_FLOAT64PUSH__) {
								t_float64 float64;
								float64.f = (long double)StrToLongDouble(word);
								varsetop->OP_VALUE = new char[8];
								*varsetop->OP_VALUE = *float64.c;
							}
							else if (varDefinition->OP_TYPE == __OP_FLOAT128PUSH__) {
								t_float128* float128 = StrToFloat128(word);
								varsetop->OP_VALUE = new char[16];
								*varsetop->OP_VALUE = *float128->val;
							}
							// UNREACHABLE
							else {
								cout << "--- compilation error in line (" << countLines << "): '" << line << "'; <-- type mismatch on reset variable '" << varDefinition->OP_LABEL << "'.\n";
								exit(1);
							}
						}
					}

					_Stack_[_Ops_Count_] = *varsetop;
					_Ops_Count_++;
				}
				
				#pragma endregion

				// automatically jumps to the next line when encounters a comment
				unsigned long itPos = GetStringIterator();
				if (itPos < line.size()) {
					string subline = line.substr(itPos, line.size()-itPos);
					TrimLeft(&subline);
					if (subline[0] == ';') {
						break;
					}
				}
				
				word = GetToken(line, ' ');
			}
		}

		if (printStack) {
			cout << "Stack: \n\n";
			for (int i = 0; i < _Ops_Count_; i++) {
				operation* op = &_Stack_[i];
				if (op->OP_VALUE == NULL) {
					break;
				}

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
}

#pragma endregion

#pragma MAIN

int main() {

	//Test();

	ParseFileToWasmStack(true);

	return 0;
}