#include <fstream>
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <locale>
#include <ctype.h>

using namespace std;

void ponctuation(string &letra, map<string, string> symbols, ifstream &in_file, int count, int &count2) {

	if (letra == ":") {
		string copy = letra;
		letra = in_file.peek();
		if (letra == "-") {
			copy += in_file.get();
			cout << "(COLON_DASH,\"" << copy << "\"," << count << ")" << endl;
			count2++;
		}
		else {
			cout << "(" << symbols[copy] << ",\"" << copy << "\"," << count << ")" << endl;
			count2++;
		}
	}
	else {
		cout << "(" << symbols[letra] << ",\"" << letra << "\"," << count << ")" << endl;
		count2++;
	}
}
void read_filep(ifstream &in_file, int &count, int &count2, map<string, string>symbols, string &letra) {
	ponctuation(letra, symbols, in_file, count, count2);
}
void string_state2(string &letra, string &palavra, ifstream &in_file, string &temp, int &count) {
	if (letra == "'") {
		temp = letra;
		letra = in_file.peek();
		if (letra == "'") {
			letra = in_file.get();
			temp += letra;
			palavra += temp;
			temp = "";
		}
		else {
			palavra += temp;
		}
	}
	else {
		if (letra == "\n") {
			count++;
		}
		palavra += letra;
	}
}
void string_state(string &letra, ifstream &in_file, int &count, int &count2) {

	string palavra = letra;
	string temp;
	int startstr = count;
	while (temp != "'" && in_file.peek() != EOF) {
		letra = in_file.get();
		string_state2(letra, palavra, in_file, temp, count);
	}

	if (in_file.peek() != EOF) {
		cout << "(STRING,\"" << palavra << "\"," << startstr << ")" << endl;
	}
	else {
		if (temp == "'") {
			cout << "(STRING,\"" << palavra << "\"," << startstr << ")" << endl;
		}
		else {
			cout << "(UNDEFINED,\"" << palavra << "\"," << startstr << ")" << endl;
		}
	}
	count2++;
	
}
void comment2(string &letra, ifstream &in_file, string &palavra, int &count, int &startcom) {
	string temp;
	while (temp != "|" && in_file.peek() != EOF) {
		letra = in_file.get();
		palavra += letra;
		letra = in_file.peek();
		if (letra == "\n") {
			count++;
		}
		temp = letra;
	}
	temp = "";
	if (letra == "|") {
		while (temp != "#" && in_file.peek() != EOF) {
			letra = in_file.get();
			palavra += letra;
			letra = in_file.peek();
			if (letra == "\n") {
				count++;
			}
			temp = letra;
		}
	}

}
void comment3(string &letra, ifstream &in_file, string &palavra) {
	while (letra != "\n") {
		letra = in_file.get();
		palavra += letra;
		letra = in_file.peek();
	}
}
void comment(string &letra, ifstream &in_file, int &count, int &count2) {

	string palavra = letra;
	int startcom = count;
	letra = in_file.peek();
	if (letra == "|") {
		comment2(letra, in_file, palavra, count, startcom);
		if (!in_file.good()) {
			if (letra == "#") {
				letra = in_file.get();
				palavra += letra;
				cout << "(COMMENT,\"" << palavra << "\"," << startcom << ")" << endl;
			}
			else {
				cout << "(UNDEFINED,\"" << palavra << "\"," << startcom << ")" << endl;
			}
		}
		else {
			letra = in_file.get();
			palavra += letra;
			cout << "(COMMENT,\"" << palavra << "\"," << startcom << ")" << endl;
		}
	}
	else {
		comment3(letra, in_file, palavra);
		cout << "(COMMENT,\"" << palavra << "\"," << count << ")" << endl;
	}
	count2++;
}
void read_file2(string &letra, ifstream &in_file, int &count, int &count2, map <string, string> keys) {
	string palavra = letra;
	letra = in_file.peek();
	string temp = letra;
	while (isalpha(temp[0])) {
		letra = in_file.get();
		palavra += letra;
		letra = in_file.peek();
		temp = letra;
	}
	if (keys.count(palavra) > 0) {

		cout << "(" << keys[palavra] << ",\"" << palavra << "\"," << count << ")" << endl;
		count2++;
	}
	else {
		while (isalpha(temp[0]) || isdigit(temp[0])) {
			letra = in_file.get();
			palavra += letra;
			letra = in_file.peek();
			temp = letra;
		} 
		cout << "(ID,\"" << palavra << "\"," << count << ")" << endl;
		count2++;
	}
}
//---------------------------------------------------------------------------------------

void read_file(ifstream &in_file, int &count, int &count2) {

	map<string, string> symbols = {
		{ ".", "PERIOD" },
		{ ",", "COMMA" },
		{ "?", "Q_MARK" },
		{ "(", "LEFT_PAREN" },
		{ ")", "RIGHT_PAREN" },
		{ ":", "COLON" },
		{ ":-", "COLON_DASH" },
		{ "*", "MULTIPLY" },
		{ "+", "ADD" },
	};

	map<string, string> keys = {
		{ "Schemes", "SCHEMES" },
		{ "Facts", "FACTS" },
		{ "Rules", "RULES" },
		{ "Queries", "QUERIES" },
	};

	string palavra = "";
	string letra;
	letra = in_file.get();

		if (symbols.count(letra) > 0) {
			read_filep(in_file, count, count2,symbols, letra);
		}
		else if (letra == "\n") {
			count++;
		}
		else if (letra == "\r") {
			
		}
		else if (letra == "'") {
			string_state(letra, in_file, count, count2);
		}
		else if (letra == "#") {
			comment(letra, in_file, count, count2);
		}
		else if (isalpha(letra[0])) {
			read_file2(letra, in_file, count, count2, keys);
		}
		else {
			if (!isspace(letra[0])) {
				cout << "(UNDEFINED,\"" << letra << "\"," << count << ")" << endl;
				count2++;
			}
		}
}
//---------------------------------------------------------------------------------------

int main(int argc, char* argv[]) {

	//Enter file name

	int count2 = 0;
	ifstream in_file;
	int count = 1;
	in_file.open(argv[1]);
	if (in_file.fail())
	{
		cout << "\nCouldn't load file\n\n" << endl;
		system("pause");
		return 0;
	}
	else {
		while (in_file.peek() != EOF)
		{
			read_file(in_file, count, count2);
		}
		cout << "(EOF,\"""\"," << count << ")" << endl;
		count2++;
		cout << "Total Tokens = " << count2 << endl;
		in_file.close();
	}
	return 0;
}

//---------------------------------------------------------------------------------------
