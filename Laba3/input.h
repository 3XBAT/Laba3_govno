#pragma once

#include <iostream>
#include <fstream>
#include <string>//для getline
using namespace std;

class CodeInput 
{
private:
	void input_code(string* code)
	{
		fstream fs;
		fs.open("data.txt", fstream::in | fstream::out);
		if (!fs.is_open())
		{
			cout << "File is not opened!" << endl;
		}
		else
		{
			string current_line;
			while (!fs.eof())
			{
				getline(fs, current_line);
				*code += current_line;
			}

		}
		fs.close();
	}

public:
	
	void get_input_code(string* code) {
		input_code(code);
	}
	
};