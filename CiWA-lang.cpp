// CiWA-lang.cpp: define o ponto de entrada para o aplicativo.
//

#include "./CiWA-lang.h"

using namespace std;

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

/*		CONSTANTS		*/

/*	TYPE OPERATIONS	 */
const short __OP_INT8PUSH__ =			IOTA(true);
const short __OP_INT16PUSH__ =			IOTA();
const short __OP_INT32PUSH__ =			IOTA();
const short __OP_INT64PUSH__=			IOTA();
const short __OP_INT128PUSH__=			IOTA();
const short __OP_FLOAT8PUSH__ =			IOTA();
const short __OP_FLOAT16PUSH__ =		IOTA();
const short __OP_FLOAT32PUSH__ =		IOTA();
const short __OP_FLOAT64PUSH__ =		IOTA();
const short __OP_FLOAT128PUSH__ =		IOTA();
const short __OP_BOOLEANPUSH__ =		IOTA();
const short __OP_CHARACTERPUSH__ =		IOTA();
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

struct operation {
	int	  OP_TYPE = -1;
	char* OP_LABEL = 0; // the address label or typename for the WebAssembly code
	char* OP_VALUE = 0; // can be any size of byte[], basically
};

char* StrToCharPointer(string str) {
	char* ret = new char[str.size()+1];
	copy(str.begin(), str.end(), ret);
	ret[str.size()] = '\0'; // 'end char'
	return ret;
}

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

int main()
{
	const regex r("((\\+|-)?[[:d:]]+)(\\.(([[:d:]]+)?))?");
	fstream file("../../../sample1.cw", ios::in); // ios::out | ios::trunc | ios::app

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
			replace(line.begin(), line.end(), '\t', '\0');
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
					operation op;
					op.OP_TYPE = __OP_INT8PUSH__;

					word = GetToken(line, ' '); // reads the variable's name
					if (word == "") {
						cout << "--- syntax error in line (" << countLines + 1 << "): '" << line << "'; <-- expected variable's name definition.\n";
						exit(1);
					}
					op.OP_LABEL = StrToCharPointer(word);

					word = GetToken(line, ' '); // reads the variable's value
					if (word == "") {
						cout << "--- syntax error in line (" << countLines + 1 << "): '" << line << "'; <-- expected variable's value definition.\n";
						exit(1);
					}
					op.OP_VALUE = new char[1];
					*op.OP_VALUE = *StrToCharPointer(word);

					_Stack_.push_back(op);
				}

				else if (word == "int16") {
					operation op;
					op.OP_TYPE = __OP_INT16PUSH__;

					word = GetToken(line, ' '); // reads the variable's name
					if (word == "") {
						cout << "--- syntax error in line (" << countLines + 1 << "): '" << line << "'; <-- expected variable's name definition.\n";
						exit(1);
					}
					op.OP_LABEL = StrToCharPointer(word);

					word = GetToken(line, ' '); // reads the variable's value
					if (word == "") {
						cout << "--- syntax error in line (" << countLines + 1 << "): '" << line << "'; <-- expected variable's value definition.\n";
						exit(1);
					}
					op.OP_VALUE = new char[2];
					*op.OP_VALUE = (short)(*StrToCharPointer(word));

					_Stack_.push_back(op);
				}

				else if (word == "int32" || word == "int") {
					operation op;
					op.OP_TYPE = __OP_INT32PUSH__;
					
					word = GetToken(line, ' '); // reads the variable's name
					if (word == "") {
						cout << "--- syntax error in line (" << countLines+1 << "): '" << line << "'; <-- expected variable's name definition.\n";
						exit(1);
					}
					op.OP_LABEL = StrToCharPointer(word);

					word = GetToken(line, ' '); // reads the variable's value
					if (word == "") {
						cout << "--- syntax error in line (" << countLines + 1 << "): '" << line << "'; <-- expected variable's value definition.\n";
						exit(1);
					}
					op.OP_VALUE = new char[4];
					*op.OP_VALUE = (int)(*StrToCharPointer(word));

					_Stack_.push_back(op);
				}

				else if (word == "int64") {
					operation op;
					op.OP_TYPE = __OP_INT64PUSH__;

					word = GetToken(line, ' '); // reads the variable's name
					if (word == "") {
						cout << "--- syntax error in line (" << countLines + 1 << "): '" << line << "'; <-- expected variable's name definition.\n";
						exit(1);
					}
					op.OP_LABEL = StrToCharPointer(word);

					word = GetToken(line, ' '); // reads the variable's value
					if (word == "") {
						cout << "--- syntax error in line (" << countLines + 1 << "): '" << line << "'; <-- expected variable's value definition.\n";
						exit(1);
					}
					op.OP_VALUE = new char[8];
					*op.OP_VALUE = (long long)(*StrToCharPointer(word));

					_Stack_.push_back(op);
				}

				else if (word == "int128") {
					operation op;
					op.OP_TYPE = __OP_INT128PUSH__;

					word = GetToken(line, ' '); // reads the variable's name
					if (word == "") {
						cout << "--- syntax error in line (" << countLines + 1 << "): '" << line << "'; <-- expected variable's name definition.\n";
						exit(1);
					}
					op.OP_LABEL = StrToCharPointer(word);

					word = GetToken(line, ' '); // reads the variable's value
					if (word == "") {
						cout << "--- syntax error in line (" << countLines + 1 << "): '" << line << "'; <-- expected variable's value definition.\n";
						exit(1);
					}
					op.OP_VALUE = new char[16];
					*op.OP_VALUE = (long long)(*StrToCharPointer(word));

					_Stack_.push_back(op);
				}

				else if (word == "float8") {
					operation op;
					op.OP_TYPE = __OP_FLOAT8PUSH__;

					word = GetToken(line, ' '); // reads the variable's name
					if (word == "") {
						cout << "--- syntax error in line (" << countLines + 1 << "): '" << line << "'; <-- expected variable's name definition.\n";
						exit(1);
					}
					op.OP_LABEL = StrToCharPointer(word);

					word = GetToken(line, ' '); // reads the variable's value
					if (word == "") {
						cout << "--- syntax error in line (" << countLines + 1 << "): '" << line << "'; <-- expected variable's value definition.\n";
						exit(1);
					}
					op.OP_VALUE = new char[1];
					*op.OP_VALUE = (float)(*StrToCharPointer(word));

					_Stack_.push_back(op);
				}

				else if (word == "float16") {
					operation op;
					op.OP_TYPE = __OP_FLOAT16PUSH__;

					word = GetToken(line, ' '); // reads the variable's name
					if (word == "") {
						cout << "--- syntax error in line (" << countLines + 1 << "): '" << line << "'; <-- expected variable's name definition.\n";
						exit(1);
					}
					op.OP_LABEL = StrToCharPointer(word);

					word = GetToken(line, ' '); // reads the variable's value
					if (word == "") {
						cout << "--- syntax error in line (" << countLines + 1 << "): '" << line << "'; <-- expected variable's value definition.\n";
						exit(1);
					}
					op.OP_VALUE = new char[2];
					*op.OP_VALUE = (float)(*StrToCharPointer(word));

					_Stack_.push_back(op);
				}
				
				else if (word == "float32" || word == "float") {
					operation op;
					op.OP_TYPE = __OP_FLOAT32PUSH__;

					word = GetToken(line, ' '); // reads the variable's name
					if (word == "") {
						cout << "--- syntax error in line (" << countLines + 1 << "): '" << line << "'; <-- expected variable's name definition.\n";
						exit(1);
					}
					op.OP_LABEL = StrToCharPointer(word);

					word = GetToken(line, ' '); // reads the variable's value
					if (word == "") {
						cout << "--- syntax error in line (" << countLines + 1 << "): '" << line << "'; <-- expected variable's value definition.\n";
						exit(1);
					}
					op.OP_VALUE = new char[4];
					*op.OP_VALUE = (float)(*StrToCharPointer(word));

					_Stack_.push_back(op);
				}

				else if (word == "float64") {
					operation op;
					op.OP_TYPE = __OP_FLOAT64PUSH__;

					word = GetToken(line, ' '); // reads the variable's name
					if (word == "") {
						cout << "--- syntax error in line (" << countLines + 1 << "): '" << line << "'; <-- expected variable's name definition.\n";
						exit(1);
					}
					op.OP_LABEL = StrToCharPointer(word);

					word = GetToken(line, ' '); // reads the variable's value
					if (word == "") {
						cout << "--- syntax error in line (" << countLines + 1 << "): '" << line << "'; <-- expected variable's value definition.\n";
						exit(1);
					}
					op.OP_VALUE = new char[8];
					*op.OP_VALUE = (long double)(*StrToCharPointer(word));

					_Stack_.push_back(op);
				}

				else if (word == "float128") {
					operation op;
					op.OP_TYPE = __OP_FLOAT128PUSH__;

					word = GetToken(line, ' '); // reads the variable's name
					if (word == "") {
						cout << "--- syntax error in line (" << countLines + 1 << "): '" << line << "'; <-- expected variable's name definition.\n";
						exit(1);
					}
					op.OP_LABEL = StrToCharPointer(word);

					word = GetToken(line, ' '); // reads the variable's value
					if (word == "") {
						cout << "--- syntax error in line (" << countLines + 1 << "): '" << line << "'; <-- expected variable's value definition.\n";
						exit(1);
					}
					op.OP_VALUE = new char[16];
					*op.OP_VALUE = (long double)(*StrToCharPointer(word));

					_Stack_.push_back(op);
				}

				else if (word == "bool") {
					operation op;
					op.OP_TYPE = __OP_BOOLEANPUSH__;

					word = GetToken(line, ' '); // reads the variable's name
					if (word == "") {
						cout << "--- syntax error in line (" << countLines + 1 << "): '" << line << "'; <-- expected variable's name definition.\n";
						exit(1);
					}
					op.OP_LABEL = StrToCharPointer(word);

					word = GetToken(line, ' '); // reads the variable's value
					if (word == "") {
						cout << "--- syntax error in line (" << countLines + 1 << "): '" << line << "'; <-- expected variable's value definition.\n";
						exit(1);
					}
					op.OP_VALUE = new char[1];
					*op.OP_VALUE = (bool)(*StrToCharPointer(word));

					_Stack_.push_back(op);
				}

				else if (word == "char") {
					// TODO : check for " and ' for char and string
					operation op;
					op.OP_TYPE = __OP_CHARACTERPUSH__;

					word = GetToken(line, ' '); // reads the variable's name
					if (word == "") {
						cout << "--- syntax error in line (" << countLines + 1 << "): '" << line << "'; <-- expected variable's name definition.\n";
						exit(1);
					}
					op.OP_LABEL = StrToCharPointer(word);

					word = GetToken(line, ' '); // reads the variable's value
					if (word == "") {
						cout << "--- syntax error in line (" << countLines + 1 << "): '" << line << "'; <-- expected variable's value definition.\n";
						exit(1);
					}
					op.OP_VALUE = new char[1];
					*op.OP_VALUE = *StrToCharPointer(word);

					_Stack_.push_back(op);
				}

				else if (word == "var") {
					// TODO : check for " and ' for char and string
					// since 'var' is a generic type, it can receive attribution with those
					operation op;
					op.OP_TYPE = __OP_VARPUSH__;

					word = GetToken(line, ' '); // reads the variable's name
					if (word == "") {
						cout << "--- syntax error in line (" << countLines + 1 << "): '" << line << "'; <-- expected variable's name definition.\n";
						exit(1);
					}
					op.OP_LABEL = StrToCharPointer(word);

					word = GetToken(line, ' '); // reads the variable's value
					if (word == "") {
						cout << "--- syntax error in line (" << countLines + 1 << "): '" << line << "'; <-- expected variable's value definition.\n";
						exit(1);
					}
					op.OP_VALUE = new char[sizeof(*StrToCharPointer(word))];
					*op.OP_VALUE = *StrToCharPointer(word);

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
					}
					else if (word == "!=") {
						condop.OP_TYPE = __OP_NOTEQUALS__;
						condop.OP_LABEL = StrToCharPointer("ne");
					}
					else if (word == ">") {
						condop.OP_TYPE = __OP_GREATER__;
						condop.OP_LABEL = StrToCharPointer("gt");
					}
					else if (word == ">=") {
						condop.OP_TYPE = __OP_GREATEROREQUAL__;
						condop.OP_LABEL = StrToCharPointer("ge");
					}
					else if (word == "<") {
						condop.OP_TYPE = __OP_LESS__;
						condop.OP_LABEL = StrToCharPointer("lt");
					}
					else if (word == "<=") {
						condop.OP_TYPE = __OP_LESSOREQUAL__;
						condop.OP_LABEL = StrToCharPointer("le");
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
					_Stack_.push_back(ifop);
				}

				else if (word == "end") {
					operation op;
					op.OP_TYPE = __OP_END__;
					op.OP_LABEL = StrToCharPointer("End");
					_Stack_.push_back(op);
				}

				else if (word == "else") {
					// firstly closes the block before
					operation op;
					op.OP_TYPE = __OP_END__;
					op.OP_LABEL = StrToCharPointer("End");
					_Stack_.push_back(op);

					operation elseop;
					elseop.OP_TYPE = __OP_ELSE__;
					elseop.OP_LABEL = StrToCharPointer("Else");
					_Stack_.push_back(elseop);
				}

				else if (word == "elsif") {
					// firstly closes the block before
					operation op;
					op.OP_TYPE = __OP_END__;
					op.OP_LABEL = StrToCharPointer("End");
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
					}
					else if (word == "!=") {
						condop.OP_TYPE = __OP_NOTEQUALS__;
						condop.OP_LABEL = StrToCharPointer("ne");
					}
					else if (word == ">") {
						condop.OP_TYPE = __OP_GREATER__;
						condop.OP_LABEL = StrToCharPointer("gt");
					}
					else if (word == ">=") {
						condop.OP_TYPE = __OP_GREATEROREQUAL__;
						condop.OP_LABEL = StrToCharPointer("ge");
					}
					else if (word == "<") {
						condop.OP_TYPE = __OP_LESS__;
						condop.OP_LABEL = StrToCharPointer("lt");
					}
					else if (word == "<=") {
						condop.OP_TYPE = __OP_LESSOREQUAL__;
						condop.OP_LABEL = StrToCharPointer("le");
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
					varsetop.OP_LABEL = varDefinition.OP_LABEL;

					word = GetToken(line, ' '); // reads the value to reset
					if (word == "") {
						cout << "--- syntax error in line (" << countLines + 1 << "): '" << line << "'; <-- expected value to be set on variable '" << varsetop.OP_LABEL << "'.\n";
						exit(1);
					}
					varsetop.OP_VALUE = new char[sizeof(*varDefinition.OP_VALUE)];
					*varsetop.OP_VALUE = *varDefinition.OP_VALUE;
				}
				/*****************************************************/

				if (itIndex == sizeof(line)) {
					word = GetToken(line, ' ', true);
				}
				else {
					word = GetToken(line, ' ');
				}
			}
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
