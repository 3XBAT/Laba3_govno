//Написать программу, определяющую корректность применения в данном коде переменных логического типа.
#include <vector>
#include <list>
#include <iostream>
#include <fstream>
#include "input.h"
#include <regex>
#include <map>

using namespace std;


string sliceOfString(string& str, int index)//для отбрасывания проверенной части кода
{
	string new_str;
	bool flag = false;

	while (index != str.length())
	{
		if (str[index] != ' ' || flag) 
		{
			new_str.push_back(str[index]);
			flag = 1;
		}
		
		index++;
	}

	return(new_str);
}

string PartOfTheString(string& code)//для деления кода по разде
{
	regex regexSepar(R"([-;,(){}=+*\s])");
	smatch match;

	bool flag = 1;
	regex_search(code, match, regexSepar);
	
	int index = 0;
	string new_str;

	while (code[index] != match.str()[0])
	{
		new_str.push_back(code[index]);
		index++;
		flag = 0;
	}
	if(flag)
	{
		return(match.str());
	}
	return(new_str);
}

enum TokenType {
	LITERAL,
	BOOL,
	COMMA,
	LEFT_BRACE,
	RIGHT_BRACE,
	SEMICOLON,
	IDENTIFIER,
	EQUAL,
	UNDEFINED,
	KEYWORD,
	DATA_TYPE
};

enum Errors
{
	WRONG_LITERAL,
	WRONG_INDENTIFIER,//тип аргумента не подходит для инициализации BOOL
	WRONG_REDEF,//нельяз переопределить перменную
	EXP_TYPE,
	EXP_IDENTIFIER,
	EXP_SEMICOLON,
	EXP_RIGHT_BRACE,
	EXP_LEFT_BRACE,
	EXP_VALUE,
	EXP_EQUAL,
	NON_DEF_VAR,//необъявленная перменная
	NON_INIT_VAR,//неинициализированная переменаня
	WRONG_DATA_TYPE,
	WRONG_INIT,
	UNDEF,

};

struct Token
{
	TokenType type;
	string value;
};


struct StatsOfVar
{
	bool isBool;
	string name;
	bool isInit;
	char status;                       //открыта для объявления или нет если статус 1, то переменная не доступна для определения, если 0, -1, -2, -3 и тд, то доступна для переопределения так как мы находимся в другой области видимости
	string value;
};

void ThrowError(vector<Token>::iterator it, Errors error)
{
	auto temp_it = it;
	while (temp_it->type != SEMICOLON && temp_it->type != LEFT_BRACE)
	{
		temp_it--;
	}

	temp_it++;

	while (temp_it->type != SEMICOLON && temp_it->type != RIGHT_BRACE)
	{
		cout << temp_it->value << " ";
		temp_it++;
	}

	if (error == WRONG_LITERAL) cout << "   Error:" << " The literal type is not suitable " << endl;
	if (error == WRONG_INDENTIFIER) cout << "   Error:" << " Unsuitable identifier" << endl;
	if (error == WRONG_REDEF) cout << "   Error:" << " redefining a variable" << endl;
	if (error == EXP_TYPE) cout << "   Error:" << " Expected data type" << endl;
	if (error == EXP_IDENTIFIER) cout << "   Error:" << " Expected identifier" << endl;
	if (error == EXP_SEMICOLON) cout << "   Error:" << " Expected semilicon ;" << endl;
	if (error == EXP_RIGHT_BRACE) cout << "   Error:" << " Expected right brace }" << endl;
	if (error == EXP_LEFT_BRACE) cout << "   Error:" << " Expexted left brace {" << endl;
	if (error == EXP_EQUAL) cout << "   Error:" << " Expected equal =" << endl;
	if (error == NON_DEF_VAR) cout << "   Error:" << " The variable has not been declared" << endl;
	if (error == NON_INIT_VAR) cout << "   Error:" << " The variable has not been initialized" << endl;
	if (error == EXP_VALUE) cout << "   Error:" << " Expexted value" << endl;
	if (error == WRONG_INIT) cout << "   Error:" << " A narrowing transformation is required" << endl;
	if (error == WRONG_DATA_TYPE) cout << "   Error:" << " The data type is not defined" << endl;
	if (error == UNDEF) cout << "   Error:" << " Unknown identifier" << endl;
}


vector<Token>tokenize(string& code)
{
	Token token;
	token.value = "";
	

	vector<Token> tokens(1,token);
	vector<Token>::iterator it = tokens.begin();
	
	regex regexKeyWords("(alignas|alignof|and b|and_eq b|asm a|auto|bitand b|bitor b|break|case|catch|class|compl b|concept c|const|const_cast|consteval c|constexpr|constinit c|continue|co_await c|co_return c|co_yield c|decltype|default|delete|do|dynamic_cast|else|enum|explicit|export c|extern|for|friend|goto|if|inline|long|mutable|namespace|new|noexcept|not b|not_eq b|nullptr|operator|or b|or_eq b|private|protected|public|register reinterpret_cast|requires c|return|short|signed|sizeof|static|static_assert|static_cast|struct|switch|template|this|thread_local|throw|try|typedef|typeid|typename|union|unsigned|using declaration|using directive|virtual|volatile|wchar_t|while|xor b|xor_eq b)");
	regex regexDataType(R"(\bchar\b|\bfloat\b|\bdouble\b|\bint\b|\bvoid\b|\bstring\b)");
	regex regexSepar(R"([;,{}='"])");
	regex regexIndentifier(R"([a-zA-Z_][a-zA-z_0-9]*)");
	regex regexLiteral(R"((["'].*["'])|(\d*\.\d+)|(\b\d+\b)|(\btrue\b)|(\bfalse\b))");
	regex regexBool("(bool)");

	smatch match;
	string buffer;

	bool flag_on_many_initialized = 0;
	
	int index = 0;
	while (code.length() )
	{
		if (code[index] == ',')
		{
			
			token.type = COMMA;
			token.value = ',';
			tokens.push_back(token);
			it = tokens.end() - 1;
			code = sliceOfString(code, 1);
		}
		else if (code[index] == '=')
		{
			
			token.type = EQUAL;
			token.value = '=';
			tokens.push_back(token);
			it = tokens.end() - 1;
			code = sliceOfString(code, 1);
		}
		else if (code[index] == '{')
		{
			token.type = LEFT_BRACE;
			token.value = '{';
			tokens.push_back(token);
			it = tokens.end() - 1;
			code = sliceOfString(code, 1);
		}
		else if (code[index] == '}')
		{
			token.type = RIGHT_BRACE;
			token.value = '}';
			tokens.push_back(token);
			it = tokens.end() - 1;
			code = sliceOfString(code, 1);
		}
		else if (code[index] == ';')
		{
			flag_on_many_initialized = 0;
			token.type = SEMICOLON;
			token.value = ';';
			tokens.push_back(token);
			it = tokens.end() - 1;
			code = sliceOfString(code, 1);
		}
		else if (regex_search(buffer = PartOfTheString(code), match, regexBool))//
		{
			token.type = BOOL;
			token.value = match.str();
			tokens.push_back(token);
			it = tokens.end() - 1;
			code = sliceOfString(code, match.length());
		}
		else if(regex_search(buffer = PartOfTheString(code), match, regexKeyWords))
		{
			token.type = KEYWORD;
			token.value = match.str();
			tokens.push_back(token);
			it = tokens.end() - 1;
			code = sliceOfString(code, match.length());
		}
		else if ((regex_search(buffer = PartOfTheString(code), match, regexLiteral)) &&((it)->type == EQUAL || (it)->type == LEFT_BRACE))
		{
			
				token.type = LITERAL;
				token.value = match.str();
				tokens.push_back(token);
				it = tokens.end() - 1;
				code = sliceOfString(code, match.length());
			
		}
		else if (regex_search(buffer = PartOfTheString(code), match, regexDataType))
		{
			token.type = DATA_TYPE;
			token.value = match.str();
			tokens.push_back(token);
			it = tokens.end() - 1;
			code = sliceOfString(code, match.length());
		}
		
		else if ((regex_search(buffer = PartOfTheString(code), match, regexIndentifier)) && isalpha(buffer[0]))
		{
			if ((it)->type == KEYWORD || (it)->type == DATA_TYPE || (it)->type == EQUAL||(it)->type == BOOL||(it->type == COMMA && flag_on_many_initialized)||((it)->type ==COMMA && (((it-2)->type == DATA_TYPE)|| ((it - 2)->type == DATA_TYPE) || (it-4)->type == DATA_TYPE || ((it - 2)->type == BOOL) || ((it - 2)->type == BOOL) || (it - 4)->type == BOOL)))
			{
				if ((it-1)->type == COMMA && (((it - 2)->type == DATA_TYPE) || ((it - 4)->type == DATA_TYPE) || ((it - 5)->type == DATA_TYPE) || ((it - 6)->type == DATA_TYPE) || (((it - 2)->type == BOOL)) || ((it - 4)->type == BOOL)|| ((it - 5)->type == BOOL) || ((it - 6)->type == BOOL)))
				{
					flag_on_many_initialized = 1;
				}
				token.type = IDENTIFIER;
				token.value = match.str();
				tokens.push_back(token);
				it = tokens.end() - 1;
				code = sliceOfString(code, match.length());
			}
			else
			{
				token.type = UNDEFINED;
				token.value = match.str();
				tokens.push_back(token);
				it = tokens.end() - 1;
				code = sliceOfString(code, match.length());
			}
		}
		else 
		{
			token.type = UNDEFINED;
			token.value = buffer;
			tokens.push_back(token);
			it = tokens.end() - 1;
			if (code.length() > 1) 
			{ 
				code = sliceOfString(code, buffer.length()); 
			}
			else {
				break;
			}

		}

	}
	map<int, string> tokenNames
	{
		{0, "LITERAL"},
		{ 1, "BOOL" },
		{ 2, "COMMA" },
		{ 3, "LEEFT_BRACE" },
		{ 4, "RIGHT_BRACE" },
		{ 5, "SEMICOLON" },
		{ 6,"IDENTIFIER" },
		{ 7, "EQUAL" },
		{ 8, "UNDEFINED" },
		{ 9, "KEYWORD" },
		{ 10, "DATA_TYPE" }
	};



	it = tokens.begin() + 1;
	while (it != tokens.end())
	{
		cout << "Token:   " << tokenNames[it->type] << "  " << it->value << endl;
		it++;
	}

	return(tokens);

}


bool CheckVarForTable(list<StatsOfVar>& TableOfVar, vector<Token>::iterator it)
{
	if (TableOfVar.size() == 0) return false;
	
	auto temp_iter = TableOfVar.end();
	temp_iter--;
	
	if (TableOfVar.size() != 1) {
		while (temp_iter != TableOfVar.begin())
		{
			if (temp_iter->name == it->value && temp_iter->status == 1)
			{
				return(true);
			}
			temp_iter--;
		}
	}
	else
	{
		if (temp_iter->name == it->value && temp_iter->status == 1)
		{
			return(true);
		}
		else
		{
			return false;
		}
	}
	return(false);
}

list<StatsOfVar>::iterator SearchVarForTable(list<StatsOfVar>& TableOfVar, vector<Token>::iterator it)//SearchVarInTable
{
	auto temp_iter = TableOfVar.end();
	temp_iter--;

	if (TableOfVar.size() != 1)
	{
		while (temp_iter != TableOfVar.begin())
		{
			if (temp_iter->name == it->value)
			{
				return(temp_iter);
			}
			temp_iter--;
		}
	}
	else
	{
		if (temp_iter->name == it->value)
		{
			return(temp_iter);
		}
	}

}



//bool proverkaForIdent(list<StatsOfVar>& TableOfVar, vector<Token>::iterator it)//проверяет возможность переопредления перменной
//{
//	//у нас есть идент и нам нужно проверить есть ли он в текущей области видимости
//
//	auto temp_iter = SearchVarForTable(TableOfVar, it);
//		
//	if (it->value == temp_iter->name)
//	{
//		if (temp_iter->status = 0)
//		{
//			return(true);
//		}
//		else
//		{
//			return(false);
//		}
//	}
//	
//
//
//}

bool CheckLiteralForCorrect(vector<Token>::iterator& it, string literal)
{
	if ((it - 1)->type == EQUAL)
	{
		regex regexLiteral(R"((['"].*['"])|(\d*\.\d+)|(\b\d+\b)|(\btrue\b)|(\bfalse\b))");
		smatch match;

		if (regex_search(it->value, match, regexLiteral))
		{
			return true;
		}
		else
		{
			it++;
			return false;
		}
	}
	else if (((it - 1)->type == LEFT_BRACE && (it - 2)->type == IDENTIFIER) || (it - 1)->type == LEFT_BRACE && (it - 2)->type == EQUAL)
	{
		regex regexLiteral(R"((\b[0,1]\b)|(\btrue\b)|(\bfalse\b))");
		smatch match;


		if (regex_search(it->value, match, regexLiteral))
		{
			return true;
		}
		else
		{
			it++;
			return false;
		}
	}

}//строго для bool-а


void CheckIdentifier(vector<Token>::iterator& it, list<StatsOfVar>::iterator& iter, list<StatsOfVar>& TableOfVar, vector<Token>& Tokens)//проверяет корректность множественного определения перменных одного типа
{// bool a, tt = true, i{4} , g = {1};
	bool flag_on_bool = 0;
	vector<Token>::iterator temp_it = it;

	while (temp_it != Tokens.begin())
	{
		if (temp_it->type == BOOL || temp_it->type == DATA_TYPE)
		{
			if (temp_it->type == BOOL) {
				flag_on_bool = 1;break;
			}
			if (temp_it->type == DATA_TYPE) {
				flag_on_bool = 0;break;
			}
		}
		temp_it--;
	}

	temp_it = it;

	

	if (flag_on_bool)//////////////////////////////////////////////
	{
		if (it->type == IDENTIFIER)
		{
			bool flag_equality = 0;
			bool flag_redef = 1;
			if (TableOfVar.size() > 1) {//проверка на то больше ли одной перменной в нашей таблице
				while (iter != TableOfVar.begin())
				{
					if (it->value == iter->name)
					{
						flag_equality = 1;
						if (iter->status == 1)
						{
							flag_redef = 0;
						}
						break;
					}
					iter--;
				}
			}
			else
			{
				if (it->value == iter->name)
				{
					flag_equality = 1;
					if (iter->status == 1)
					{
						flag_redef = 0;
					}
				}
			}



			if (flag_redef)// начинаются проврки////////////////////////////////////////////////////////////////
			{
				it++;
				if (it->type == EQUAL)
				{
					it++;
					if (it->type == LITERAL)
					{
						if (CheckLiteralForCorrect(it, it->value))
						{
							it++;
							if (it->type == SEMICOLON)
							{
								StatsOfVar var;
								var.isBool = 1;
								var.isInit = 1;
								var.name = (it - 3)->value;
								var.status = 1;
								TableOfVar.push_back(var);
								it++;
								iter++;
								return;
							}
							else if (it->type == COMMA)
							{
								StatsOfVar var;
								var.isBool = 1;
								var.isInit = 1;
								var.name = (it - 3)->value;
								var.status = 1;
								TableOfVar.push_back(var);
								it++;
								iter++;
								CheckIdentifier(it, iter, TableOfVar, Tokens);
							}
							else
							{
								ThrowError(it, EXP_SEMICOLON);
								return;
							}
						}
						else
						{
							ThrowError(it, WRONG_LITERAL);
							return;
						}
					}
					else if (it->type == IDENTIFIER)
					{

						auto temp_iter = iter;
						bool flag_equality = 0;
						while (temp_iter != TableOfVar.begin())
						{
							if (iter->name == it->value)
							{
								flag_equality = 1;
							}
						}

						if (flag_equality)
						{
							it++;
							if (it->type == SEMICOLON)
							{
								StatsOfVar var;
								var.isBool = 1;
								var.isInit = 1;
								var.name = (it - 3)->value;
								var.status = 1;
								TableOfVar.push_back(var);
								it++;
								iter++;
							}
							else if (it->type == COMMA)
							{
								StatsOfVar var;
								var.isBool = 1;
								var.isInit = 1;
								var.name = (it - 3)->value;
								var.status = 1;
								TableOfVar.push_back(var);
								it++;
								iter++;
								CheckIdentifier(it, iter, TableOfVar, Tokens);
							}
							else
							{
								ThrowError(it, EXP_SEMICOLON);
								return;
							}
						}
						else
						{
							ThrowError(it, WRONG_INDENTIFIER);
							return;
						}
					}
					else if (it->type == LEFT_BRACE)/////////////////////////////
					{// bool a = {...
						it++;
						if (it->type == LITERAL)
						{
							if (CheckLiteralForCorrect(it, it->value))
							{
								it++;
								if (it->type == RIGHT_BRACE)
								{
									it++;
									if (it->type == SEMICOLON)
									{
										StatsOfVar var;
										var.isBool = 1;
										var.isInit = 1;
										var.name = (it - 5)->value;
										var.status = 1;
										TableOfVar.push_back(var);
										it++;
										iter++;
										return;
									}
								}
								else// bool a = {b_;
								{
									ThrowError(it, EXP_RIGHT_BRACE);
									return;
								}
							}
							else
							{
								ThrowError(it, WRONG_LITERAL);
								return;
							}
						}
						else if (it->type == IDENTIFIER)//обязательно должен быть bool
						{
							auto temp_iter= SearchVarForTable(TableOfVar, it);
							if (temp_iter->name == it->value)
							{
								if (temp_iter->isBool)
								{
									if (temp_iter->isInit)
									{

										it++;
										if (it->type == RIGHT_BRACE)
										{
											it++;
											if (it->type == SEMICOLON)
											{
												StatsOfVar var;
												var.isBool = 1;
												var.isInit = 1;
												var.name = (it - 4)->value;
												var.status = 1;
												it++;
												iter++;
												return;
											}
											else if (it->type == COMMA)
											{
												it++;
												CheckIdentifier(it, iter, TableOfVar, Tokens);
											}
										}
										else
										{
											ThrowError(it, EXP_RIGHT_BRACE);
											return;
										}
									}
									else
									{
										ThrowError(it, WRONG_INDENTIFIER);
										return;
										//indet не инициализирован
									}
								}
								else
								{
									ThrowError(it, WRONG_INDENTIFIER);
									return;
									//идентификатор 
								}
							}
							else
							{
								ThrowError(it, WRONG_INDENTIFIER);
								return;
							}
						}
						else
						{
							ThrowError(it, EXP_VALUE);
							return;
						}
					}
				}
				else if (it->type == LEFT_BRACE)
				{
					it++;
					if (it->type == LITERAL)
					{
						if (CheckLiteralForCorrect(it, it->value))
						{
							it++;
							if (it->type == RIGHT_BRACE)
							{
								it++;
								if (it->type == SEMICOLON)
								{
									StatsOfVar var;
									var.isBool = 1;
									var.isInit = 1;
									var.name = (it - 5)->value;
									var.status = 1;
									TableOfVar.push_back(var);
									it++;
									iter++;
								}
								else if (it->type == COMMA)
								{
									StatsOfVar var;
									var.isBool = 1;
									var.isInit = 1;
									var.name = (it - 5)->value;
									var.status = 1;
									TableOfVar.push_back(var);
									it++;
									iter++;
									CheckIdentifier(it, iter, TableOfVar, Tokens);
								}
								else
								{
									ThrowError(it, EXP_SEMICOLON);
									return;
								}
							}
							else// bool a = {b_;
							{
								ThrowError(it, EXP_RIGHT_BRACE);
								return;
							}
						}
						else
						{
							ThrowError(it, WRONG_LITERAL);
							return;
						}
					}
					else if (it->type == IDENTIFIER)//обязательно должен быть bool
					{
						auto temp_iter = SearchVarForTable(TableOfVar, it);
						if (temp_iter->name == it->value)
						{
							if (temp_iter->isBool)
							{
								if (temp_iter->isInit)
								{

									it++;
									if (it->type == RIGHT_BRACE)
									{
										it++;
										if (it->type == SEMICOLON)
										{
											StatsOfVar var;
											var.isBool = 1;
											var.isInit = 1;
											var.name = (it - 4)->value;
											var.status = 1;
											it++;
											iter++;
											return;
										}
										else if (it->type == COMMA)
										{
											it++;
											CheckIdentifier(it, iter, TableOfVar, Tokens);
										}
									}
									else
									{
										ThrowError(it, EXP_RIGHT_BRACE);
										return;
									}
								}
								else
								{
									ThrowError(it, WRONG_INDENTIFIER);
									return;
									//indet не инициализирован
								}
							}
							else
							{
								ThrowError(it, WRONG_INDENTIFIER);
								return;
								//идентификатор 
							}
						}
						else
						{
							ThrowError(it, WRONG_INDENTIFIER);
							return;
						}
					}
					else
					{
						ThrowError(it, EXP_VALUE);
						return;
					}
				}

			}
			else
			{
				ThrowError(it, WRONG_REDEF);
				return;
			}


		}
		else if (it->type == UNDEFINED)
		{
			auto temp_iter = SearchVarForTable(TableOfVar, it);
			if (temp_iter->name == it->value)
			{
				if (temp_iter->isBool)
				{
					if (temp_iter->isInit)
					{

						it++;
						if (it->type == RIGHT_BRACE)
						{
							it++;
							if (it->type == SEMICOLON)
							{
								StatsOfVar var;
								var.isBool = 1;
								var.isInit = 1;
								var.name = (it - 4)->value;
								var.status = 1;
								it++;
								iter++;
								return;
							}
							else if (it->type == COMMA)
							{
								it++;
								CheckIdentifier(it, iter, TableOfVar, Tokens);
							}
						}
						else if (it->type == EQUAL)
						{
							it++;
							if (it->type == LITERAL)
							{
								if (CheckLiteralForCorrect(it, it->value))
								{
									it++;
									if (it->type == SEMICOLON)
									{
										StatsOfVar var;
										var.isBool = 1;
										var.isInit = 1;
										var.name = (it - 3)->value;
										var.status = 1;
										TableOfVar.push_back(var);
										it++;
										iter++;
										return;
									}
									else if (it->type == COMMA)
									{
										StatsOfVar var;
										var.isBool = 1;
										var.isInit = 1;
										var.name = (it - 3)->value;
										var.status = 1;
										TableOfVar.push_back(var);
										it++;
										iter++;
										CheckIdentifier(it, iter, TableOfVar, Tokens);
									}
									else
									{
										ThrowError(it, EXP_SEMICOLON);
										return;
									}
								}
								else
								{
									ThrowError(it, WRONG_LITERAL);
									return;
								}
							}
							else if (it->type == IDENTIFIER)
							{

								auto temp_iter = iter;
								bool flag_equality = 0;
								while (temp_iter != TableOfVar.begin())
								{
									if (iter->name == it->value)
									{
										flag_equality = 1;
									}
								}

								if (flag_equality)
								{
									it++;
									if (it->type == SEMICOLON)
									{
										StatsOfVar var;
										var.isBool = 1;
										var.isInit = 1;
										var.name = (it - 3)->value;
										var.status = 1;
										TableOfVar.push_back(var);
										it++;
										iter++;
									}
									else if (it->type == COMMA)
									{
										StatsOfVar var;
										var.isBool = 1;
										var.isInit = 1;
										var.name = (it - 3)->value;
										var.status = 1;
										TableOfVar.push_back(var);
										it++;
										iter++;
										CheckIdentifier(it, iter, TableOfVar, Tokens);
									}
									else
									{
										ThrowError(it, EXP_SEMICOLON);
										return;
									}
								}
								else
								{
									ThrowError(it, WRONG_INDENTIFIER);
									return;
								}
							}
							else if (it->type == LEFT_BRACE)/////////////////////////////
							{// bool a = {...
								it++;
								if (it->type == LITERAL)
								{
									if (CheckLiteralForCorrect(it, it->value))
									{
										it++;
										if (it->type == RIGHT_BRACE)
										{
											it++;
											if (it->type == SEMICOLON)
											{
												StatsOfVar var;
												var.isBool = 1;
												var.isInit = 1;
												var.name = (it - 5)->value;
												var.status = 1;
												TableOfVar.push_back(var);
												it++;
												iter++;
												return;
											}
										}
										else// bool a = {b_;
										{
											ThrowError(it, EXP_RIGHT_BRACE);
											return;
										}
									}
									else
									{
										ThrowError(it, WRONG_LITERAL);
										return;
									}
								}
								else if (it->type == IDENTIFIER)//обязательно должен быть bool
								{
									auto temp_iter = SearchVarForTable(TableOfVar, it);
									if (temp_iter->name == it->value)
									{
										if (temp_iter->isBool)
										{
											if (temp_iter->isInit)
											{

												it++;
												if (it->type == RIGHT_BRACE)
												{
													it++;
													if (it->type == SEMICOLON)
													{
														StatsOfVar var;
														var.isBool = 1;
														var.isInit = 1;
														var.name = (it - 4)->value;
														var.status = 1;
														it++;
														iter++;
														return;
													}
													else if (it->type == COMMA)
													{
														it++;
														CheckIdentifier(it, iter, TableOfVar, Tokens);
													}
												}
												else
												{
													ThrowError(it, EXP_RIGHT_BRACE);
													return;
												}
											}
											else
											{
												ThrowError(it, WRONG_INDENTIFIER);
												return;
												//indet не инициализирован
											}
										}
										else
										{
											ThrowError(it, WRONG_INDENTIFIER);
											return;
											//идентификатор 
										}
									}
									else
									{
										ThrowError(it, WRONG_INDENTIFIER);
										return;
									}
								}
								else
								{
									ThrowError(it, EXP_VALUE);
									return;
								}
							}
						}
						else
						{
							ThrowError(it, EXP_RIGHT_BRACE);
							return;
						}
					}
					else
					{
						ThrowError(it, WRONG_INDENTIFIER);
						return;
						//indet не инициализирован
					}
				}
				else
				{
					ThrowError(it, WRONG_INDENTIFIER);
					return;
					//идентификатор говно
				}
			}
			else
			{
				ThrowError(it, WRONG_INDENTIFIER);
				return;
			}
		}
		else
		{
			ThrowError(it, EXP_IDENTIFIER);
			return;
		}
	}   



	
}

void CheckVariables(vector<Token>& Tokens, list<StatsOfVar>& TableOfVar, vector<Token>::iterator& it, list<StatsOfVar>::iterator& iter)//проверяет истинность переменных которыми мы инициализируем новую перменную
{
	it++;
	if (it->type == SEMICOLON)
	{
		StatsOfVar var;
		var.isBool = 1;
		var.isInit = 1;
		var.name = (it - 3)->value;
		var.status = 1;
		var.value = (it - 1)->value;
		TableOfVar.push_back(var);
		it++;
		iter = TableOfVar.end();
		iter--;
		return;
	}
	else if (it->type == LITERAL)///////dllfmldvldmvldmlvdldvm
	{
		it++;
		CheckVariables(Tokens, TableOfVar, it, iter);
	}
	else if (it->type == COMMA)
	{
		StatsOfVar var;
		var.isBool = 1;
		var.isInit = 1;
		var.name = (it - 3)->value;
		var.status = 1;
		var.value = (it - 1)->value;
		TableOfVar.push_back(var);
		it++;
		iter = TableOfVar.end();
		iter--;
	
		CheckIdentifier(it, iter, TableOfVar, Tokens);
	}
	else
	{
		ThrowError(it, EXP_SEMICOLON);
		return;
	}


}

void ChangeStatus(list<StatsOfVar>& pointer,  bool action)// 1 - новая область, 0 - выходо из области
{
	auto iter_start = pointer.begin();
	auto iter_end = pointer.end();
	iter_end--;
	


	if (action)
	{
		while (iter_start != pointer.end())
		{
			iter_start->status -= 1;
			iter_start++;
		}
	}
	else
	{
		while (iter_end != pointer.begin())
		{
			
				if (iter_end->status == 1)
				{
					pointer.erase(iter_end);
					iter_end = pointer.end();
					
				}

				if (iter_end->status <= 0)
				{
					iter_end->status += 1;
				}
		
			iter_end--;
		}

	}
}

void AnalayzeTheBraces(vector<Token>::iterator& it, list<StatsOfVar>::iterator& iter, list<StatsOfVar>& TableOfVar, vector<Token>& Tokens)
{//анализирует корректность кода в фиг. скобках при инициализации перменной
	it++;
	if (it->type == LITERAL)
	{
		if (CheckLiteralForCorrect(it, it->value))
		{
			it++;
			if (it->type == RIGHT_BRACE)
			{
				it++;
				if (it->type == SEMICOLON)
				{
					StatsOfVar var;
					var.isBool = 1;
					var.isInit = 1;
					var.value = (it - 2)->value;
					var.status = 1;

					auto temp_it = it;
					while (temp_it != Tokens.begin())
					{
						if (temp_it->type == BOOL)
						{
							var.name = (temp_it + 1)->value;
							break;
						}
						temp_it--;
					}
					TableOfVar.push_back(var);
					it++;
					iter = TableOfVar.end();
					iter--;
					return;
				}
				else
				{
					ThrowError(it, EXP_SEMICOLON);
					return;
				}
			}
			else
			{
				ThrowError(it, EXP_RIGHT_BRACE);
				return;
			}
		}
		else
		{
			ThrowError(it, WRONG_LITERAL);
			return;
		}
	}
	else if (it->type == IDENTIFIER || (it->type == UNDEFINED && CheckVarForTable(TableOfVar,it)))
	{//второе выражение проверят если у нас андеф, то все равно нужно проверить по таблице вдруг такая перменная у нас есть
		if (TableOfVar.size() == 0 || (SearchVarForTable(TableOfVar,it)->status == 1 && SearchVarForTable(TableOfVar,it)->isInit))
		{
			it++;
				if (it->type == RIGHT_BRACE)
				{
					it++;
						if (it->type == SEMICOLON)
						{
							StatsOfVar var;
								var.isBool = 1;
								var.isInit = 1;
								var.status = 1;
							var.value = it->value;
							auto temp_it = it;
							while (temp_it != Tokens.begin())
							{
								if (temp_it->type == BOOL)
								{
									var.name = (temp_it + 1)->value;
									break;
								}
								temp_it--;
							}
							TableOfVar.push_back(var);
							it++;
							iter = TableOfVar.end();
							iter--;
							return;
						}
						else
						{
							ThrowError(it, EXP_SEMICOLON);
							return;
						}
				}
				else
				{
					ThrowError(it, EXP_RIGHT_BRACE);
					return;
				}
		}
		else
		{
			ThrowError(it, WRONG_INDENTIFIER);
			return;
		}
	}
	else
	{
		ThrowError(it, UNDEF);
		return;
	}
}

void analyze(vector<Token>& Tokens)
{

	auto it = Tokens.begin();
	list<StatsOfVar> TableOfVar = {};
	list<StatsOfVar>::iterator iter = TableOfVar.begin();

	int brace_left = 0, brace_right = 0;
	it++;
	while (it != Tokens.end())
	{

		if (it->type == LEFT_BRACE)
		{
			brace_left++;
			it++;
			if (brace_left > 1)ChangeStatus(TableOfVar, 1);
		}
		if (it->type == RIGHT_BRACE)
		{
			brace_right++;
			it++;
			if (brace_left - brace_right == 0)break;
			if (brace_right > 1)ChangeStatus(TableOfVar, 0);
		}

		if (it->type == BOOL)//////////////////////////////////////////////////////////////////////////////////
		{
			it++;
			if (it->type == IDENTIFIER)
			{
				if (TableOfVar.size() == 0 || !CheckVarForTable(TableOfVar,it))//
				{

					it++;
					if (it->type == EQUAL)
					{
						it++;
						if (it->type == LEFT_BRACE)
						{
							AnalayzeTheBraces(it, iter, TableOfVar, Tokens);
						}
						else if (it->type == LITERAL)
						{
							if (CheckLiteralForCorrect(it, it->value))
							{
								CheckVariables(Tokens,TableOfVar,it,iter);//CheckVariebles раньше было
							}
						}
						else if (it->type == IDENTIFIER || it->type == UNDEFINED)
						{
							if (TableOfVar.size() == 0)
							{
								ThrowError(it, NON_DEF_VAR);
								return;
							}

							bool flag = 0;
							auto temp_iter = TableOfVar.end();
							temp_iter--;
							
							while (temp_iter != TableOfVar.begin()||TableOfVar.size()==1)
							{
								if (it->value == temp_iter->name && temp_iter->isInit == 1 && temp_iter->status == 1) 
								{
									flag = 1;
									break;
								}
								
								if (TableOfVar.size() == 1)
								{
									break;
								}
								temp_iter--;
							}


							if (flag)
							{
								it++;
								if (it->type == SEMICOLON)
								{
									StatsOfVar var;
									var.isBool = 1;
									var.isInit = 1;
									var.status = 1;
									var.value = it->value;
									var.name = (it - 3)->value;
									TableOfVar.push_back(var);
									it++;
									iter++;
								}
								else
								{
									ThrowError(it, EXP_SEMICOLON);
									return;
								}

							}
							else
							{
								ThrowError(it, NON_INIT_VAR);
								return;
							}
						}
						else
						{
							ThrowError(it, EXP_SEMICOLON);
							return;
						}
					}
					else if (it->type == LEFT_BRACE)
					{
						AnalayzeTheBraces(it, iter, TableOfVar, Tokens);
					}
					else if (it->type == SEMICOLON)
					{
						StatsOfVar var;
						var.isBool = 1;
						var.isInit = 0;
						var.status = 1;
						var.value = "0";
						var.name = (it - 1)->value;
						TableOfVar.push_back(var);
						it++;
						iter = TableOfVar.end();
						iter--;
					}
					else if (it->type == COMMA)
					{
						StatsOfVar var;
						var.isBool = 1;
						var.isInit = 0;
						var.status = 1;
						var.value = "0";
						var.name = (it - 1)->value;
						TableOfVar.push_back(var);
						it++;
						iter = TableOfVar.end();
						iter--;
						CheckIdentifier(it, iter, TableOfVar, Tokens);
						return;
					}
					else
					{
						ThrowError(it, EXP_EQUAL);
						ThrowError(it, EXP_SEMICOLON);
						break;
					}

				}
				else
				{
					ThrowError(it, WRONG_INDENTIFIER);
					break;
				}
			}
			else
			{
				ThrowError(it, EXP_IDENTIFIER);
				return;
			}

		}
		else if (it->type == UNDEFINED)
		{
			ThrowError(it, UNDEF);
			return;
		}
		else
		{
			ThrowError(it, WRONG_DATA_TYPE);
			return;
		}



	}
}




int main() {
	fstream fs;
	string code;

	
	CodeInput item;

	item.get_input_code(&code);

	vector<Token> tokens = tokenize(code);
	
	cout << endl;

	analyze(tokens);

} 