// CiWA-lang.cpp: define o ponto de entrada para o aplicativo.
//

#include "CiWA-lang.h"
#include <list>
#include <regex>

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
#define __OP_INT32PUSH__ IOTA(true)
#define __OP_INT64PUSH__ IOTA()
#define __OP_FLOAT32PUSH__ IOTA()
#define __OP_FLOAT64PUSH__ IOTA()
#define __OP_BOOLEANPUSH__ IOTA()
#define __OP_CHARACTERPUSH__ IOTA()
#define __OP_VARPUSH__ IOTA()
#define __OP_VARPOP__ IOTA()
#define __OP_CONSTDEF__ IOTA()

/*	 COMMANDS	*/
#define __OP_IFSTART__ IOTA()
#define __OP_END__ IOTA()

/*	 OPERATORS  */
#define __OP_EQUALS__ IOTA()
#define __OP_NOTEQUALS__ IOTA()
#define __OP_GREATER__ IOTA()
#define __OP_GREATEROREQUAL__ IOTA()
#define __OP_LESS__ IOTA()
#define __OP_LESSOREQUAL__ IOTA()

/***************************************/

struct operation {
	short OP_TYPE;
	char* OP_LABEL; // the address label or typename for the WebAssembly code
	char* OP_VALUE; // can be any size of byte[], basically
};

char* StrToCharPointer(string str) {
	char* ret = new char[str.size()+1];
	copy(str.begin(), str.end(), ret);
	ret[str.size()] = '\0'; // 'end char'
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
		list<operation> _Stack_; // initially 10000 operations, if more is needed, than it'll be expanded

		while (getline(file, line)) {
			if (line.empty()) { // if it's an empty line, just jump to the next one
				continue;
			}
			replace(line.begin(), line.end(), '\t', (char)0);
			while (line[0] == '\0') {
				line = line.substr(1, line.size());
			}
			
			string word = strtok(StrToCharPointer(line), " ");

			while (StrToCharPointer(word) != NULL) {
				if (word[0] == ';' || word == "") { // if it's a comment line, just jump to the next one
					break;
				}

				/*				---	TYPE PUSH ---				*/

				if (word == "int32" || word == "int") {
					operation op;
					op.OP_TYPE = __OP_INT32PUSH__;
					
					word = strtok(NULL, " "); // reads the variable's name
					if (StrToCharPointer(word) == NULL) {
						cout << "--- syntax error in line (" << countLines+1 << "): '" << line << "';\n expected variable's name definition.\n";
						exit(1);
					}
					op.OP_LABEL = StrToCharPointer(word);

					word = strtok(NULL, " "); // reads the variable's value
					if (StrToCharPointer(word) == NULL) {
						cout << "--- syntax error in line (" << countLines + 1 << "): '" << line << "';\n expected variable's value definition.\n";
						exit(1);
					}
					op.OP_VALUE = new char[4];
					*op.OP_VALUE = (int)(*StrToCharPointer(word));

					_Stack_.push_back(op);
					break;
				}

				if (word == "int64") {
					operation op;
					op.OP_TYPE = __OP_INT64PUSH__;

					word = strtok(NULL, " "); // reads the variable's name
					if (StrToCharPointer(word) == NULL) {
						cout << "--- syntax error in line (" << countLines + 1 << "): '" << line << "';\n expected variable's name definition.\n";
						exit(1);
					}
					op.OP_LABEL = StrToCharPointer(word);

					word = strtok(NULL, " "); // reads the variable's value
					if (StrToCharPointer(word) == NULL) {
						cout << "--- syntax error in line (" << countLines + 1 << "): '" << line << "';\n expected variable's value definition.\n";
						exit(1);
					}
					op.OP_VALUE = new char[8];
					*op.OP_VALUE = (int)(*StrToCharPointer(word));

					_Stack_.push_back(op);
					break;
				}
				
				if (word == "float32" || word == "float") {
					operation op;
					op.OP_TYPE = __OP_FLOAT32PUSH__;

					word = strtok(NULL, " "); // reads the variable's name
					if (StrToCharPointer(word) == NULL) {
						cout << "--- syntax error in line (" << countLines + 1 << "): '" << line << "';\n expected variable's name definition.\n";
						exit(1);
					}
					op.OP_LABEL = StrToCharPointer(word);

					word = strtok(NULL, " "); // reads the variable's value
					if (StrToCharPointer(word) == NULL) {
						cout << "--- syntax error in line (" << countLines + 1 << "): '" << line << "';\n expected variable's value definition.\n";
						exit(1);
					}
					op.OP_VALUE = new char[4];
					*op.OP_VALUE = (float)(*StrToCharPointer(word));

					_Stack_.push_back(op);
					break;
				}

				if (word == "float64") {
					operation op;
					op.OP_TYPE = __OP_FLOAT64PUSH__;

					word = strtok(NULL, " "); // reads the variable's name
					if (StrToCharPointer(word) == NULL) {
						cout << "--- syntax error in line (" << countLines + 1 << "): '" << line << "';\n expected variable's name definition.\n";
						exit(1);
					}
					op.OP_LABEL = StrToCharPointer(word);

					word = strtok(NULL, " "); // reads the variable's value
					if (StrToCharPointer(word) == NULL) {
						cout << "--- syntax error in line (" << countLines + 1 << "): '" << line << "';\n expected variable's value definition.\n";
						exit(1);
					}
					op.OP_VALUE = new char[8];
					*op.OP_VALUE = (double)(*StrToCharPointer(word));

					_Stack_.push_back(op);
					break;
				}

				if (word == "bool") {
					operation op;
					op.OP_TYPE = __OP_BOOLEANPUSH__;

					word = strtok(NULL, " "); // reads the variable's name
					if (StrToCharPointer(word) == NULL) {
						cout << "--- syntax error in line (" << countLines + 1 << "): '" << line << "';\n expected variable's name definition.\n";
						exit(1);
					}
					op.OP_LABEL = StrToCharPointer(word);

					word = strtok(NULL, " "); // reads the variable's value
					if (StrToCharPointer(word) == NULL) {
						cout << "--- syntax error in line (" << countLines + 1 << "): '" << line << "';\n expected variable's value definition.\n";
						exit(1);
					}
					op.OP_VALUE = new char[1];
					*op.OP_VALUE = (bool)(*StrToCharPointer(word));

					_Stack_.push_back(op);
					break;
				}

				if (word == "char") {
					operation op;
					op.OP_TYPE = __OP_CHARACTERPUSH__;

					word = strtok(NULL, " "); // reads the variable's name
					if (StrToCharPointer(word) == NULL) {
						cout << "--- syntax error in line (" << countLines + 1 << "): '" << line << "';\n expected variable's name definition.\n";
						exit(1);
					}
					op.OP_LABEL = StrToCharPointer(word);

					word = strtok(NULL, " "); // reads the variable's value
					if (StrToCharPointer(word) == NULL) {
						cout << "--- syntax error in line (" << countLines + 1 << "): '" << line << "';\n expected variable's value definition.\n";
						exit(1);
					}
					op.OP_VALUE = new char[1];
					*op.OP_VALUE = (*StrToCharPointer(word));

					_Stack_.push_back(op);
					break;
				}

				if (word == "var") {
					operation op;
					op.OP_TYPE = __OP_VARPUSH__;

					word = strtok(NULL, " "); // reads the variable's name
					if (StrToCharPointer(word) == NULL) {
						cout << "--- syntax error in line (" << countLines + 1 << "): '" << line << "';\n expected variable's name definition.\n";
						exit(1);
					}
					op.OP_LABEL = StrToCharPointer(word);

					word = strtok(NULL, " "); // reads the variable's value
					if (StrToCharPointer(word) == NULL) {
						cout << "--- syntax error in line (" << countLines + 1 << "): '" << line << "';\n expected variable's value definition.\n";
						exit(1);
					}
					op.OP_VALUE = new char[sizeof(*StrToCharPointer(word))];
					*op.OP_VALUE = *StrToCharPointer(word);

					_Stack_.push_back(op);
					break;
				}

				/*****************************************************/

				/*				---	CONDITIONAL ---				*/

				if (word == "if") {
					operation popop;
					popop.OP_TYPE = __OP_VARPOP__;
					word = strtok(NULL, " "); // reads the variable's name
					if (StrToCharPointer(word) == NULL) {
						cout << "--- syntax error in line (" << countLines + 1 << "): '" << line << "';\n expected variable name for conditional test definition.\n";
						exit(1);
					}
					popop.OP_LABEL = StrToCharPointer(word);
					_Stack_.push_back(popop);

					operation condop;
					word = strtok(NULL, " "); // reads the conditional operation
					if (StrToCharPointer(word) == NULL) {
						cout << "--- syntax error in line (" << countLines + 1 << "): '" << line << "';\n expected a conditional operation.\n";
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
						cout << "--- syntax error in line (" << countLines + 1 << "): '" << line << "';\n invalid conditional operation.\n";
						exit(1);
					}

					operation poporconstop;
					word = strtok(NULL, " "); // reads the second term for conditional testing
					if (StrToCharPointer(word) == NULL) {
						cout << "--- syntax error in line (" << countLines + 1 << "): '" << line << "';\n expected variable name or a constant for conditional test definition.\n";
						exit(1);
					}
					// defines if it is a number or a variable's name
					if (regex_match(word, r)) {
						poporconstop.OP_TYPE = __OP_CONSTDEF__;
						poporconstop.OP_VALUE = StrToCharPointer(word);
					}
					else {
						poporconstop.OP_TYPE = __OP_VARPOP__;
						poporconstop.OP_LABEL = StrToCharPointer(word);
					}
					_Stack_.push_back(poporconstop);

					_Stack_.push_back(condop); // only added after the addition of the 2 variables to be tested

					// lastly, opens the if block
					operation ifop;
					ifop.OP_TYPE = __OP_IFSTART__;
					ifop.OP_LABEL = StrToCharPointer("If");
					_Stack_.push_back(ifop);

					break;
				}

				if (word == "end") {
					operation op;
					op.OP_TYPE = __OP_END__;
					op.OP_LABEL = StrToCharPointer("End");
					_Stack_.push_back(op);
					break;
				}

				/*****************************************************/

				/* 
					TODO : Type attribution after already having declared it 
					- tries to find the name in the programm operations
					- if it doesn't: exits program with code 1 and saying that used variable without declaring it first
				*/

				/*			---	TYPE ATTRIBUTION ---			*/

				operation varDefinition;
				for(int i=0; i<_Stack_.size(); i++){
					operation op = _Stack_.front();
					if (word == op.OP_LABEL) {
						varDefinition = op;
					}
				}

				if (varDefinition.OP_LABEL != "") {
					cout << "--- compile error in line (" << countLines + 1 << "): '" << line << "';\n variable name not declared.\n";
					exit(1);
				}

				/*****************************************************/

			}

			countLines++;
		}
		
		file.close();
	}
	else {
		cout << "file could not be opened..." << "\n\n";
		exit(1);
	}

	return 0;
}
