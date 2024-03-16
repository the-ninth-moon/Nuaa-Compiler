#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <vector>
#include <set>
#define TEST cout<<"---------------"<<words[nowNum].type<<"    "<<words[nowNum].name<<"   行："<<words[nowNum].column<<"     列："<<words[nowNum].row<<endl;
using namespace std;


// 使用字符分割
void Stringsplit(const string& str, const char split, vector<string>& res)
{
	if (str == "")        return;
	//在字符串末尾也加入分隔符，方便截取最后一段
	string strs = str + split;
	size_t pos = strs.find(split);
	size_t last = 0;
	// 若找不到内容则字符串搜索函数返回 npos
	while (pos != strs.npos)
	{

		string temp = strs.substr(last, pos - last);//从last到pos，长度为pos-last
		res.push_back(temp);
		last = pos + 1;
		pos = strs.find(split, pos + 1);
	}
}

int level = 0;//层差
int space = 3;//开辟的空间
bool generate = true;//如果出错了就不生成代码

class OneCode {
public:
	OneCode() {};
	OneCode(string f, int l, int a1) :func(f), L(l), a(a1) {};
	string func;
	int L;
	int a;//地址偏移量，对于跳转是绝对地址，对于常数来说记录数值
};
class Code {
public:
	OneCode code[300];
	int allcode;
	int nowCode;

	Code(int a = 0, int n = 0) :allcode(a), nowCode(n) {};
	void emit(string f, int l, int a) {
		if (!generate) return;
		code[nowCode].func = f;
		code[nowCode].L = l;
		code[nowCode].a = a;
		nowCode++;
		allcode++;
	}
	void write2File()
	{
		if (!generate) return;
		fstream f("code.txt", ios::out);
		for (int i = 0;i < allcode;i++)
		{
			f << code[i].func << " " << code[i].L << " " << code[i].a << endl;
			cout << code[i].func << " " << code[i].L << " " << code[i].a << endl;
		}
		f.close();
		cout << "代码生成完毕" << endl;
	}
	int len() {
		return nowCode;//nowcode指向下一个写入的地方
	}
};
Code code;

class TableItem {
public:
	TableItem(string n, string t, int l, int v, int a) :name(n), type(t), level(l), val(v), addr(a) {};
	string name;
	string type;
	int level; //层次
	int val;
	int addr;
};
class Table {
public:
	map<int, vector<TableItem>> table;//key为level，value其实用一个TableItem就可以了，因为同一时刻至多每层只定义了一个过程
	Table() {}
	void recordTable(string n, string t, int l = 0, int v = 0, int a = 0) {
		table[l].push_back(TableItem(n, t, l, v, a));
	}
	//查找name在符号表的位置
	vector<int> find(string n)
	{
		//先从自身level找起
		for (int i = level;i >= 0;i--) {
			for (int j = table[i].size() - 1;j >= 0;--j) {
				if (table[i][j].name == n) return { i,j };
			}
		}
		return { -1,-1 };
	}
	vector<int> find2(string n)
	{
		//查找过程，可以找子过程，所以要这么做
		for (int i = 0;;i++) {
			if (!table.count(i)) break;
			for (int j = table[i].size() - 1;j >= 0;--j) {
				if (table[i][j].name == n) return { i,j };
			}
		}
		return { -1,-1 };
	}
	int len() {
		return table.size();
	}
};
Table table;

//词法分析器传来的一个单词的结构
class OneWord
{
public:
	string type;
	string name;
	int column;
	int row;
	OneWord(string t, string n, int c, int r) :type(t), name(n), column(c), row(r) {};
	OneWord(string t, string n, string c, string r) :type(t), name(n), column(stoi(c)), row(stoi(r)) {};
};
class error {
public:
	string e;
	int column;
	int row;
	error(string i, int c, int r) :e(i), column(c), row(r) {};
};
class WordAnalyse
{
public:
	string strToken = "";
	char ch;
	int code;
	int value;
	int worldNum = 31;
	int constNum = 0;

	int line = 1;
	int column = 1;
	int lastcolumn = 1;

	map<string, int> world = {
		{"program",1},{"const",2},{"var",3},{"procedure",4},{"begin",5},
		{"end",6},{"if",7},{"then",8},{"else",9},{"while",10},
		{"do",11},{"call",12},{"read",13},{"write",14},{"odd",15},
		{":=",16},{";",17},{",",18},{"=",19},{"<>",20},
		{"<",21},{"<=",22},{">",23},{">=",24},{"+",25},
		{"-",26},{"*",27},{"/",28},{"(",29},{")",30},
		{".",31}
	};
	map<long, int> number = {};


	FILE* input;
	FILE* output;
	char output2[30];
	WordAnalyse(string in = "input.txt", string ou = "output.txt", string ou2 = "wordAnalyse.txt")
	{
		input = fopen(in.c_str(), "r");
		//output = fopen(ou.c_str(),"w");
		//output2 = fopen(ou2.c_str(),"w");
	}
	void close() {
		fclose(input);
	}
	void GetChar()
	{
		//cout<<"Getchar   ";
		//cout<<input.tellg();
		ch = fgetc(input);
		//cout<<line<<"   "<<column;
		//printf("   %c \n",ch);
		column++;
	}

	void GetBC()
	{
		if (ch == ' ' || ch == '	')
		{
			GetChar();
			GetBC();
		}
	}

	void Concat()
	{
		strToken += ch;
	}

	bool IsDigit()
	{
		return ch >= '0' && ch <= '9';
	}
	bool IsLetter()
	{
		return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
	}
	bool IsSymbol()
	{
		return ch == '=' || ch == '+' || ch == '-' || ch == '*' || ch == '/' ||
			ch == '>' || ch == '<' || ch == ':' || ch == ',' || ch == ';' || ch == '(' || ch == ')' || ch == '.';
	}
	int Reserve()
	{
		//cout<<"reserve   "+strToken<<endl;
		if (world[strToken]) {
			return world[strToken];
		}
		return 0;
	}
	void Retract()
	{
		//cout<<"retract   "<<endl;;
		if (feof(input)) return;
		if (ch == '\n') {
			line++;
			column = 0;
			return;
		}
		column--;
		//cout<<input.tellg()<<"  ";
		fseek(input, -1, SEEK_CUR);
		//cout<<input.tellg()<<endl;
	}
	void test()
	{
		if (input == NULL) return;
		GetChar();
		//cout<<input.tellg()<<"  ";
		fseek(input, -1, SEEK_CUR);
		//cout<<input.tellg()<<endl;
		GetChar();
		//cout<<input.tellg()<<"  ";
		fseek(input, -1, SEEK_CUR);
		//cout<<input.tellg()<<endl;
	}
	void InsertId() {
		++worldNum;
		world[strToken] = worldNum;

	}
	void InsertConst() {
		++constNum;
		//cout<<strToken<<endl;
		number[stoi(strToken)] = constNum;

	}

	void handle() {
		Retract();
		strToken.clear();
	}

	string oneWordAnalyse()
	{
		sprintf(output2, "");
		GetChar();GetBC();
		//cout<<"Analyse  ";
		//cout<<input.tellg()<<"   "<<ch<<endl;

		if (IsLetter()) {
			while (IsLetter() || IsDigit()) {
				Concat();
				GetChar();
			}
			code = Reserve();
			//标识符
			if (code == 0) {
				InsertId();
				//output<<"(id,"+strToken +','+ to_string(worldNum) +")"<<endl;
				//fprintf(output,"(id,%s,%d)\n",strToken.c_str(),worldNum);
				sprintf(output2, "nid %s %d %d", strToken.c_str(), line, column - 1);
				//cout<<"(id,"+strToken +','+ to_string(worldNum) +")"<<endl;

			}
			//保留字
			else {
				//output<<"("+ strToken+","+ to_string(world[strToken]) +")"<<endl;
				if (world[strToken] <= 31)
				{
					//fprintf(output,"(reserved word,%s,%d)\n",strToken.c_str(),world[strToken]);
					sprintf(output2, "word %s %d %d", strToken.c_str(), line, column - 1);
				}
				else
				{
					//fprintf(output,"(id,%s,%d)\n",strToken.c_str(),world[strToken]);
					sprintf(output2, "id %s %d %d", strToken.c_str(), line, column - 1);
				}

				//cout<<"("+ strToken+","+ to_string(world[strToken]) +")"<<endl;

			}
		}
		else if (IsDigit()) {
			while (IsDigit()) {
				Concat();
				GetChar();
			}
			if (ch == '\n') {
				InsertConst();
				//fprintf(output,"(integer,%s,%d)\n",strToken.c_str(),constNum);
				sprintf(output2, "integer %s %d %d", strToken.c_str(), line, column - 1);
				//cout<<"(integer,"+ strToken+","+to_string(constNum) +")"<<endl;
				handle();
				return output2;
			}
			//处理小数点
			Retract();
			GetChar();
			if (ch == '.')
			{
				Concat();
				GetChar();
				while (IsDigit()) {
					Concat();
					GetChar();
				}
				InsertConst();
				//fprintf(output,"(float,%s,%d)\n",strToken.c_str(),constNum);
				sprintf(output2, "float %s %d %d", strToken.c_str(), line, column - 1);
				//cout<<"(float,"+ strToken+","+to_string(constNum) +")"<<endl;
			}
			//形如111aaa
			else if (IsLetter())
			{
				sprintf(output2, "integer %s %d %d", strToken.c_str(), line, column - 1);
				string tmp;
				while (IsLetter() || IsDigit()) {
					if (IsLetter()) tmp += ch;
					Concat();
					GetChar();
				}
				if (world.count(tmp))
					sprintf(output2, "word %s %d %d", tmp.c_str(), line, column - 1);
				//fprintf(output,"Error: 非法标志符 %s 出现在 行:%d, 列:%d.\n",strToken.c_str(),line,column-1);
				printf("词法错误: 非法标志符 %s 出现在 行:%d, 列:%d.\n", strToken.c_str(), line, column - 1);
				generate = false;
			}
			else {
				InsertConst();
				//fprintf(output,"(integer,%s,%d)\n",strToken.c_str(),constNum);
				sprintf(output2, "integer %s %d %d", strToken.c_str(), line, column - 1);
				//cout<<"(integer,"+ strToken+","+to_string(constNum) +")"<<endl;
			}

		}
		else if (IsSymbol()) {
			//cout<<"else if IsSymbol "<<(char)ch<<endl;
			while (IsSymbol()) {
				Concat();
				GetChar();
			}
			if (strToken == "+") {
				//fprintf(output,"(aop,%s)\n",strToken.c_str());
				sprintf(output2, "aop %s %d %d", strToken.c_str(), line, column - 1);
			}
			else if (strToken == "-")
			{
				//fprintf(output,"(aop,%s)\n",strToken.c_str());
				sprintf(output2, "aop %s %d %d", strToken.c_str(), line, column - 1);
			}
			else if (strToken == "*") {
				//fprintf(output,"(mop,%s)\n",strToken.c_str());
				sprintf(output2, "mop %s %d %d", strToken.c_str(), line, column - 1);
			}
			else if (strToken == "/") {
				//fprintf(output,"(mop,%s)\n",strToken.c_str());
				sprintf(output2, "mop %s %d %d", strToken.c_str(), line, column - 1);
			}
			else if (strToken == "=") {
				//fprintf(output,"(lop,%s)\n",strToken.c_str());
				sprintf(output2, "lop %s %d %d", strToken.c_str(), line, column - 1);
			}
			else if (strToken == "<>") {
				//fprintf(output,"(lop,%s)\n",strToken.c_str());
				sprintf(output2, "lop %s %d %d", strToken.c_str(), line, column - 1);
			}
			else if (strToken == "<") {
				//fprintf(output,"(lop,%s)\n",strToken.c_str());
				sprintf(output2, "lop %s %d %d", strToken.c_str(), line, column - 1);
			}
			else if (strToken == "<=") {
				//fprintf(output,"(lop,%s)\n",strToken.c_str());
				sprintf(output2, "lop %s %d %d", strToken.c_str(), line, column - 1);
			}
			else if (strToken == ">") {
				//fprintf(output,"(lop,%s)\n",strToken.c_str());
				sprintf(output2, "lop %s %d %d", strToken.c_str(), line, column - 1);
			}
			else if (strToken == ">=") {
				//fprintf(output,"(lop,%s)\n",strToken.c_str());
				sprintf(output2, "lop %s %d %d", strToken.c_str(), line, column - 1);
			}
			else if (strToken == ":=") {
				//fprintf(output,"(lop,%s)\n",strToken.c_str());
				sprintf(output2, "lop %s %d %d", strToken.c_str(), line, column - 1);
			}
			else if (strToken == "(") {
				//fprintf(output,"(lpar,%s)\n",strToken.c_str());
				sprintf(output2, "lpar %s %d %d", strToken.c_str(), line, column - 1);
			}
			else if (strToken == ")") {
				//fprintf(output,"(rpar,%s)\n",strToken.c_str());
				sprintf(output2, "rpar %s %d %d", strToken.c_str(), line, column - 1);
			}
			else if (strToken == ",") {
				//fprintf(output,"(comma,%s)\n",strToken.c_str());
				sprintf(output2, "comma %s %d %d", strToken.c_str(), line, column - 1);
			}
			else if (strToken == ";") {
				//fprintf(output,"(semicolon,%s)\n",strToken.c_str());
				sprintf(output2, "semicolon %s %d %d", strToken.c_str(), line, column - 1);
			}
			else if (strToken == ".") {
				//fprintf(output,"(period,%s)\n",strToken.c_str());
				sprintf(output2, "period %s %d %d", strToken.c_str(), line, column - 1);
			}
			else if (strToken == ");") {
				//fprintf(output,"(rpar,%s)\n",strToken.c_str());
				sprintf(output2, "rpar ) %d %d semicolon ; %d %d", line, column - 2, line, column - 1);
				//fprintf(output,"(semicolon,%s)\n",strToken.c_str());
			}
			else if (strToken == "()") {
				//fprintf(output,"(lpar,%s)\n",strToken.c_str());
				sprintf(output2, "lpar ) %d %d rpar ) %d %d", line, column - 2, line, column - 1);
				//fprintf(output,"(rpar,%s)\n",strToken.c_str());
			}
			else if (strToken == "();") {
				//fprintf(output,"(lpar,%s)\n",strToken.c_str());
				sprintf(output2, "lpar ( %d %d rpar ) %d %d semicolon ; %d %d", line, column - 3, line, column - 2, line, column - 1);
				//fprintf(output,"(rpar,%s)\n",strToken.c_str());
			}
			else {
				if (strToken != "\n")
				{
					if (strToken != ");")
					{
						printf("词法错误: 疑似运算符 '%s' 出现在 行:%d, 列:%d，请检查是否缺失符号\n", strToken.c_str(), line, column - 1);
						generate = false;
						//fprintf(output,"Error: 疑似运算符 '%s' 出现在 行:%d, 列:%d，请检查是否缺失符号\n",strToken.c_str(),line,column-1);						
					}
				}
			}
		}
		else {
			if (strToken != "\n" && strToken != "")
			{
				printf("词法错误: 疑似运算符 '%s' 出现在 行:%d, 列:%d，请检查是否缺失符号\n", strToken.c_str(), line, column - 1);
				generate = false;
				//fprintf(output,"Error: 疑似运算符 '%s' 出现在 行:%d, 列:%d，请检查是否缺失符号\n",strToken.c_str(),line,column-1);				
			}
			GetChar();
		}
		handle();
		return output2;
	}
	string analyse()
	{
		if (!input) {
			sprintf(output2, "ProEnd # %d %d", line, column - 1);
			return output2;
		}
		if (!feof(input))
		{
			return oneWordAnalyse();
			//cout<<input.tellg()<<(char)input.get()<<endl;
		}
		sprintf(output2, "ProEnd # %d %d", line, column - 1);
		return output2;
	}
};

class GrammarAnalyse
{
public:
	int nowNum = 0;
	int allNum = 0;
	bool wrong = false;//语法错误
	bool wrong2 = false;//语义错误
	vector<OneWord> words;//存储单词
	vector<error> errors;
	WordAnalyse word_analyse;
	fstream input;

	GrammarAnalyse(string in) :word_analyse(in) {
		allNum += 2;
		string input = word_analyse.analyse();
		vector<string> res;
		Stringsplit(input, ' ', res);
		words.push_back(OneWord(res[0], res[1], res[2], res[3]));
		input = word_analyse.analyse();
		res.clear();
		Stringsplit(input, ' ', res);
		//cout << input << endl;
		words.push_back(OneWord(res[0], res[1], res[2], res[3]));
	}
	void printError(string info, int row, int column)//语法错误
	{
		generate = false;
		wrong = true;
		//if(info.find("同步")!=info.npos) return;
		printf("语法错误：row:%2d,column:%2d ", row, column);
		printf(info.c_str());
		errors.push_back(error(info, row, column));
		printf("\n");
	}
	void printError2(string info, int row, int column)//语义错误
	{
		generate = false;
		wrong2 = true;
		//if(info.find("同步")!=info.npos) return;
		printf("语义错误：row:%2d,column:%2d ", row, column);
		printf(info.c_str());
		errors.push_back(error(info, row, column));
		printf("\n");
	}
	void close() {
		word_analyse.close();
	}
	map<string, set<string>> first_set = {
		{"<prog>",{"program"}},
		{"<block>",{"const","var","procedure","begin"}},
		{"<condecl>",{"const"}},
		{"<const>",{"id","nid"}},
		{"<vardecl>",{"var"}},
		{"<proc>",{"procedure"}},
		{"<body>",{"begin"}},
		{"<statement>",{"nid","id","if","while","call","begin","read","write"}},
		{"<lexp>",{"odd","+","-","nid","id","(","integer"}},
		{"<exp>",{"odd","+","-","nid","id","(","integer"}},
		{"<term>",{"id","nid","(","integer"}},
		{"<factor>",{"id","nid","(","integer"}},
	};
	map<string, set<string>> follow_set = {
		{"<prog>",{"#"}},
		{"<block>",{"#",";","begin"}},
		{"<condecl>",{"var","procedure","begin"}},
		{"<const>",{",",";"}},
		{"<vardecl>",{"procedure","begin"}},
		{"<proc>",{"begin",";"}},
		{"<body>",{"#","else","end",";",".","begin"}},
		{"<statement>",{"else","end",";"}},
		{"<lexp>",{"then","do"}},
		{"<exp>",{"else","end","then","do",",",")",";","=","<>","<","<=",">",">="}},
		{"<term>",{"+","-","else","end","then","do",",",")",";","=","<>","<","<=",">",">="}},
		{"<factor>",{"*","/","+","-","else","end","then","do",",",")",";","=","<>","<","<=",">",">="}},
		{"id",{",",":=",";","(",")","*","/","+","-","else","end","then","do",",",")",";","=","<>","<","<=",">",">="}},
		{"integer",{"*","/","+","-","else","end","then","do",",",")",";","=","<>","<","<=",">",">=",}},
	};

	void nextWord()
	{
		if (words[nowNum].type == "ProEnd")	return;
		allNum++;
		nowNum++;
		string input = word_analyse.analyse();
		vector<string> res;
		Stringsplit(input, ' ', res);
		for (int i = 0;i < res.size();i += 4)
		{
			words.push_back(OneWord(res[i], res[i + 1], res[i + 2], res[i + 3]));
		}
		//if (words[nowNum].type != "ProEnd")	cout << words[nowNum].name << " " << words[nowNum].type<<" "<<words[nowNum].row<<" " << words[nowNum].column<< endl;
	}
	void analyse()
	{
		prog();
		if (!wrong) {
			cout << "语法分析结束,程序无语法错误" << endl;
			if (!wrong2) cout << "语义分析结束,代码已经生成" << endl;
			else cout << "语义分析结束,程序出现语义错误" << endl;
		}
		if (wrong) cout << "语法分析结束,程序出现语法错误" << endl;

	}
	void prog()
	{
		if (words[nowNum].name != "program")
		{
			if (words[nowNum].type == "nid")
			{
				printError("缺少program关键字", words[nowNum].column, words[nowNum].row);
				//--nowNum;

			}
			else if (words[nowNum + 1].name == "program") {
				printError("出现错误单词" + words[nowNum].name, words[nowNum].column, words[nowNum].row);
			}
			else if (words[nowNum + 1].type == "nid")
			{
				printError("program关键字拼写错误", words[nowNum].column, words[nowNum].row);
			}
			else {

			}
		}
		nextWord();
		if (words[nowNum].type != "nid") {
			if (words[nowNum].name == ";")
			{
				printError("缺少id", words[nowNum].column, words[nowNum].row);
				--nowNum;
			}
			else if (words[nowNum + 1].type == "nid") {
				printError("出现错误单词" + words[nowNum].name, words[nowNum].column, words[nowNum].row);
			}
			else if (words[nowNum + 1].name == ";")
			{
				printError("使用关键字作为id名或者是错误的id", words[nowNum].column, words[nowNum].row);
			}
			else {

			}
		}
		nextWord();
		while (words[nowNum].type == "nid" || words[nowNum].type == "id") {
			if (words[nowNum].type == "id") printError("id  " + words[nowNum].name + "  重复定义", words[nowNum].column, words[nowNum].row);
			printError("program后不可有多个id", words[nowNum].column, words[nowNum].row);
			nextWord();
		}
		if (words[nowNum].name != ";") {
			if (first_set["<block>"].count(words[nowNum].name))//出现在block的first集中，说明缺失;
			{
				--nowNum;
				printError("缺失分号", words[nowNum].column, words[nowNum].row);
			}
			else if (first_set["<block>"].count(words[nowNum + 1].name))//下一个单词是block的first集，说明这个符号错了
			{
				printError("错误的符号 " + words[nowNum].name + " 应该为分号", words[nowNum].column, words[nowNum].row);
			}
		}
		nextWord();
		block();
	}
	void block()
	{
		//space第一次进入block为3，之后按照规矩来
		//记录当前语句条数，便于回填
		int current = code.len();//下一条生成的地址，便于回填653行
		code.emit("JMP", 0, 0);

		if (words[nowNum].name == "const") {
			condecl();
		}
		if (words[nowNum].name == "var") {
			vardecl();
		}
		if (words[nowNum].name == "procedure") {
			int nowSpace = space;//保存现在的空间
			proc();
			space = nowSpace;//恢复
		}
		//进入body前需要回填开头的jump指令，因为代码是顺序生成的，但是我们执行的时候是body先开始的，所以要跳到body这里
		code.code[current].a = code.len();//跳入到body这里
		code.emit("INT", 0, space);//开辟空间
		body();
		code.emit("OPR", 0, 0);//返回调用点并退栈
		if (words[nowNum].name == ".") return;
		while (!follow_set["<block>"].count(words[nowNum].name)) {
			printError("正在同步<block>,跳过 " + words[nowNum].name, words[nowNum].column, words[nowNum].row);
			nextWord();
		}
	}
	void condecl()
	{
		nextWord();//第一个词语肯定是const
		_const();
		while (words[nowNum].name == "," || words[nowNum].type == "nid")
		{
			if (words[nowNum].name == ",")
			{
				nextWord();
				_const();
			}
			else {
				printError("const中缺少逗号间隔。", words[nowNum].column, words[nowNum].row);
				_const();
			}

		}
		if (words[nowNum].name == ";") {
			nextWord();
			return;
		}
		else {
			printError("缺失分号", words[nowNum].column, words[nowNum].row);
		}
		while (!follow_set["<condecl>"].count(words[nowNum].name)) {
			printError("正在同步<condecl>,跳过 " + words[nowNum].name, words[nowNum].column, words[nowNum].row);
			nextWord();
		}
		//现在指向的是 var procedure 或者 begin

	}
	void _const()
	{
		if (words[nowNum].type != "nid" && words[nowNum].type != "id") {
			if (words[nowNum].name == ":=") {
				printError("缺少id", words[nowNum].column, words[nowNum].row);
				--nowNum;
			}
			else if (words[nowNum + 1].name == ":=") {

				printError("错误的id" + words[nowNum].name + ",可能使用了关键字作为标识符？", words[nowNum].column, words[nowNum].row);
			}
			nextWord();
			nextWord();
			nextWord();
		}
		else {
			string constname = words[nowNum].name;
			nextWord();
			if (words[nowNum].name != ":=") {
				if (words[nowNum].type == "integer") {
					printError("缺少赋值符号", words[nowNum].column, words[nowNum].row);
					--nowNum;
				}
				else if (words[nowNum + 1].type == "integer") {
					printError("错误的赋值符号" + words[nowNum].name, words[nowNum].column, words[nowNum].row);
				}
				nextWord();
				nextWord();
			}
			else {
				nextWord();
				if (words[nowNum].type != "integer") {
					if (words[nowNum].name == "," || words[nowNum].name == ";") {
						printError("缺少整数", words[nowNum].column, words[nowNum].row);
					}
					else if (words[nowNum + 1].name == "," || words[nowNum + 1].name == ";") {
						printError("有错误的整数" + words[nowNum].name, words[nowNum].column, words[nowNum].row);
					}
				}
				else {
					table.recordTable(constname, "CONSTANT", level, stoi(words[nowNum].name));
				}
				nextWord();
			}

		}

		while (!follow_set["<const>"].count(words[nowNum].name) && words[nowNum].type != "nid") {
			printError("正在同步<const>,跳过 " + words[nowNum].name, words[nowNum].column, words[nowNum].row);
			nextWord();
		}
	}
	void vardecl() {
		nextWord();//第一个词语肯定是var
		if (words[nowNum].type != "nid" && words[nowNum].type != "id") {
			if (words[nowNum].name == ",") {
				--nowNum;
				printError("缺少id", words[nowNum].column, words[nowNum].row);
			}
			else if (words[nowNum + 1].name == ",") {
				printError("有错误的id" + words[nowNum].name + "可能使用了保留字作为id？", words[nowNum].column, words[nowNum].row);
			}
		}
		else {
			//是id
			table.recordTable(words[nowNum].name, "VARIABLE", level, 0, space);
			//cout<<words[nowNum].name<<" "<<level<<endl;
			space++;//空间增加
		}
		nextWord();
		while (words[nowNum].type == "nid" || words[nowNum].type == "id") {
			//if (words[nowNum].type == "id") printError("id" + words[nowNum].name + "重复定义", words[nowNum].column, words[nowNum].row);
			printError("var中缺少逗号", words[nowNum].column, words[nowNum].row);
			nextWord();
		}
		while (words[nowNum].name == ",")
		{
			nextWord();
			if (words[nowNum].name == ";") break;
			if (words[nowNum].type != "nid" && words[nowNum].type != "id") {
				if (words[nowNum].name == ",") {
					printError("缺少id", words[nowNum].column, words[nowNum].row);
					--nowNum;
				}
				else if (words[nowNum + 1].name == ",") {
					printError("有错误的id" + words[nowNum].name + "可能使用了保留字作为id？", words[nowNum].column, words[nowNum].row);
				}
			}
			else {
				//是id
				//cout<<words[nowNum].name<<" "<<space<<endl;
				table.recordTable(words[nowNum].name, "VARIABLE", level, 0, space);
				//cout << words[nowNum].name << " " << level << endl;
				space++;//空间增加
			}
			nextWord();//似乎有点粗糙，万一中间只少了一个，也会直接开始同步,试着解决一下
			while (words[nowNum].type == "nid" || words[nowNum].type == "id") {
				if (words[nowNum].type == "id") printError("id" + words[nowNum].name + "重复定义", words[nowNum].column, words[nowNum].row);
				printError("缺少逗号", words[nowNum].column, words[nowNum].row);
				nextWord();
			}

		}
		if (words[nowNum].name == ";")
		{
			nextWord();//正常结束
			return;
		}
		while (!follow_set["<vardecl>"].count(words[nowNum].name)) {
			printError("正在同步<vardecl>,跳过 " + words[nowNum].name, words[nowNum].column, words[nowNum].row);
			nextWord();
		}
	}
	void proc()
	{
		space = 3;
		string tmp;
		int proca = 0;
		int vnmum = 0;
		if (words[nowNum].name != "procedure") {
			if (words[nowNum].type == "id") {
				--nowNum;
				printError("缺少procedure关键字", words[nowNum].column, words[nowNum].row);
			}
			else if (words[nowNum + 1].type == "id") {
				printError("错误的关键字" + words[nowNum].name + "应该为procedure", words[nowNum].column, words[nowNum].row);
			}
		}
		nextWord();
		if (words[nowNum].type != "nid" && words[nowNum].type != "id") {
			if (words[nowNum].name == "(") {
				--nowNum;
				printError("缺少id", words[nowNum].column, words[nowNum].row);
			}
			else if (words[nowNum + 1].name == "(") {
				printError("有错误的id" + words[nowNum].name + "可能使用了保留字作为id？", words[nowNum].column, words[nowNum].row);
			}
		}
		else {
			//是id，填写符号表
			tmp = words[nowNum].name;
			proca = table.table[level].size();
			table.recordTable(words[nowNum].name, "PROCEDURE", level, 0, code.len());//新的过程的地址 是 下一个写入的代码位置，也就是它自己的第一条代码地址
			
		}
		nextWord();
		while (words[nowNum].type == "nid" || words[nowNum].type == "id") {
			//if (words[nowNum].type == "id") printError("id  " + words[nowNum].name + "  重复定义", words[nowNum].column, words[nowNum].row);
			printError("procedure后不可有多个id", words[nowNum].column, words[nowNum].row);
			nextWord();
		}
		if (words[nowNum].name != "(") {

			if (words[nowNum].name == ")" || words[nowNum].type == "nid" || words[nowNum].type == "id") {
				printError("缺少 ( ", words[nowNum].column, words[nowNum].row);
				--nowNum;//回退一个单词
			}
			else if (words[nowNum].name == "," || words[nowNum + 1].name == ")" || words[nowNum + 1].type == "nid") {
				printError("应该为 ( ，出现了错误的单词：" + words[nowNum].name + "可能使用了保留字作为id？", words[nowNum].column, words[nowNum].row);
			}
			//不能全部跳完，要继续语法分析
			while (!first_set["<body>"].count(words[nowNum].name)) {
				printError("正在同步<proc>,跳过 " + words[nowNum].name, words[nowNum].column, words[nowNum].row);
				nextWord();
			}
		}
		else {
			//为'('
			nextWord();
			if (words[nowNum].type == "nid" || words[nowNum].type == "id") {
				//是id,填写表
				table.recordTable(words[nowNum].name, "VARIABLE", level + 1, 0, space);//特别注意这里，因为形参实际是下一级的
				space++;//空间增加
				nextWord();
				while (words[nowNum].type == "nid" || words[nowNum].type == "id") {
					printError("缺少逗号", words[nowNum].column, words[nowNum].row);
					nextWord();
				}
				while (words[nowNum].name == ",")
				{
					nextWord();
					if (words[nowNum].name == ")") break;
					if (words[nowNum].type != "nid" && words[nowNum].type != "id") {
						if (words[nowNum].name == ",") {
							printError("缺少id", words[nowNum].column, words[nowNum].row);
							--nowNum;
						}
						else if (words[nowNum + 1].name == ",") {
							printError("有错误的id" + words[nowNum].name + "可能使用了保留字作为id？", words[nowNum].column, words[nowNum].row);
						}
					}
					else {
						//是id,填写表
						table.recordTable(words[nowNum].name, "VARIABLE", level + 1, 0, space);//因为形参实际是下一级的
						space++;//空间增加
					}
					nextWord();//似乎有点粗糙，万一中间只少了一个，也会直接开始同步,试着解决一下
					//cout << words[nowNum].name << endl;
					while (words[nowNum].type == "nid" || words[nowNum].type == "id") {
						if (words[nowNum].type == "id") printError("id" + words[nowNum].name + "重复定义", words[nowNum].column, words[nowNum].row);
						printError("缺少逗号", words[nowNum].column, words[nowNum].row);
						nextWord();
					}

				}
			}

			if (words[nowNum].name != ")") {
				if (words[nowNum].name == ";") {
					--nowNum;
					printError("缺少 ) ", words[nowNum].column, words[nowNum].row);
				}
				else if (words[nowNum + 1].name == ";") {
					printError("应该为 ) ，出现了错误的单词：" + words[nowNum].name, words[nowNum].column, words[nowNum].row);
				}
			}
			nextWord();
			if (words[nowNum].name != ";") {
				if (first_set["<block>"].count(words[nowNum].name)) {
					--nowNum;
					printError("缺少 ; ", words[nowNum].column, words[nowNum].row);
				}
				else if (first_set["<block>"].count(words[nowNum + 1].name)) {
					printError("应该为 ; ，出现了错误的单词：" + words[nowNum].name, words[nowNum].column, words[nowNum].row);
				}
			}
		}
		table.table[level][proca].val = space-3;
		//在进入block前需要做的工作
		level += 1;
		nextWord();//指向block第一个词
		block();
		//结束后恢复
		level -= 1;

		//销毁该层变量
		int t = table.table[level + 1].size();
		for (int i = 0;i < t;i++)
		{
			table.table[level + 1].pop_back();
		}

		//是分号继续proc
		if (words[nowNum].name == ";")//需要再思考一番 是if还是while
		{
			nextWord();
			proc();
		}
		while (!follow_set["<proc>"].count(words[nowNum].name)) {
			if (words[nowNum].type == "ProEnd")	return;
			printError("正在同步<proc>,跳过 " + words[nowNum].name, words[nowNum].column, words[nowNum].row);
			nextWord();
		}
	}
	void body()
	{
		if (words[nowNum].name != "begin") {
			if (first_set["<statement>"].count(words[nowNum].name) || first_set["<statement>"].count(words[nowNum].type)) {
				--nowNum;
				printError("缺少begin关键字", words[nowNum].column, words[nowNum].row);
			}
			else if (first_set["<statement>"].count(words[nowNum + 1].name)) {
				printError("错误的关键字" + words[nowNum].name + "应该为begin", words[nowNum].column, words[nowNum].row);
			}
		}
		nextWord();
		statement();

		while (words[nowNum].name == ";" || (first_set["<statement>"].count(words[nowNum].name) || first_set["<statement>"].count(words[nowNum].type)))
		{
			if (first_set["<statement>"].count(words[nowNum].name) || first_set["<statement>"].count(words[nowNum].type)) {
				--nowNum;
				if (words[nowNum].name != ";")
					printError("缺少;", words[nowNum].column, words[nowNum].row);
			}
			nextWord();
			statement();
		}
		//处理end

		if (words[nowNum].name != "end") {
			if (follow_set["<body>"].count(words[nowNum].name)) {
				--nowNum;
				printError("<body>缺少end关键字", words[nowNum].column, words[nowNum].row);
			}
			else if (follow_set["<statement>"].count(words[nowNum + 1].name)) {
				printError("<body>错误的关键字" + words[nowNum].name + "应该为end", words[nowNum].column, words[nowNum].row);
			}
		}
		nextWord();
		while (!follow_set["<body>"].count(words[nowNum].name)) {
			printError("正在同步<body>,跳过 " + words[nowNum].name, words[nowNum].column, words[nowNum].row);
			nextWord();
		}
	}
	void statement() {
		if (words[nowNum].name == "#") return;//防止忘记写end
		if (words[nowNum].name == "end") {
			printError("不应该有分号", words[nowNum].column, words[nowNum].row);
			return;//防止加了;，继续statment但是有end
		}

		while (!(first_set["<statement>"].count(words[nowNum].name) || first_set["<statement>"].count(words[nowNum].type) || (follow_set["<statement>"].count(words[nowNum].name) || follow_set["<statement>"].count(words[nowNum].type)))) {
			printError("正在同步到<statement>开头，单词‘" + words[nowNum].name + "’可能多余", words[nowNum].column, words[nowNum].row);
			nextWord();
		}
		if (words[nowNum].type == "id" || words[nowNum].type == "nid" || words[nowNum + 1].name == ":=") {
			//TEST;
			statement_id();
		}
		else if (words[nowNum].name == "if") {
			statement_if();
		}
		else if (words[nowNum].name == "while") {
			statement_while();
		}
		else if (words[nowNum].name == "call") {
			statement_call();
		}
		else if (words[nowNum].name == "begin") {
			body();
		}
		else if (words[nowNum].name == "read") {
			statement_read();
		}
		else if (words[nowNum].name == "write") {
			statement_write();
		}
		else if (first_set["<lexp>"].count(words[nowNum + 1].name)) {
			statement_if();
		}
		else if (words[nowNum + 1].name == "(") {
			if (words[nowNum + 2].type == "id" || words[nowNum + 2].type == "nid") {
				statement_read();
			}
			else {
				statement_write();
			}
		}
		while (!(follow_set["<statement>"].count(words[nowNum].name) || follow_set["<statement>"].count(words[nowNum].type) || first_set["<statement>"].count(words[nowNum].name) || first_set["<statement>"].count(words[nowNum].type))) {
			if (words[nowNum].name == "#") return;//防止忘记写end
			if (words[nowNum].name == "end") {
				printError("不应该有分号", words[nowNum].column, words[nowNum].row);
				return;//防止加了;，继续statment但是有end
			}

			printError("正在同步<statement>,跳过 " + words[nowNum].name, words[nowNum].column, words[nowNum].row);
			nextWord();
		}
	}
	void statement_id() {
		int l, index;
		if (words[nowNum].type != "id") {
			if (words[nowNum].type == "nid") {
				printError("id " + words[nowNum].name + " 尚未定义", words[nowNum].column, words[nowNum].row);
			}
			else if (words[nowNum].name == ":=") {
				--nowNum;
				printError("缺少id", words[nowNum].column, words[nowNum].row);
			}
			else if (words[nowNum + 1].name == ":=") {
				printError("有错误的id" + words[nowNum].name + "可能使用了保留字作为id？", words[nowNum].column, words[nowNum].row);
			}
		}
		else {
			//确实是id
			l = table.find(words[nowNum].name)[0], index = table.find(words[nowNum].name)[1];
			if (index == -1) {
				printError2(words[nowNum].name + "没有定义", words[nowNum].column, words[nowNum].row);
			}
			else if (table.table[l][index].type != "VARIABLE") {
				printError2(words[nowNum].name + "不是变量", words[nowNum].column, words[nowNum].row);
			}

		}
		nextWord();
		if (words[nowNum].name != ":=") {
			if (first_set["<exp>"].count(words[nowNum].name) || first_set["<exp>"].count(words[nowNum].type)) {
				printError("缺少赋值符号", words[nowNum].column, words[nowNum].row);
				--nowNum;
			}
			else if (first_set["<exp>"].count(words[nowNum + 1].name) || first_set["<exp>"].count(words[nowNum + 1].type)) {
				printError("错误的赋值符号" + words[nowNum].name, words[nowNum].column, words[nowNum].row);
			}
			else {
				printError("错了，但不知道发生了什么" + words[nowNum].name, words[nowNum].column, words[nowNum].row);
			}
		}
		nextWord();
		exp();
		//合法变量我们应该把exp产生的值存起来
		if (index != -1) {
			code.emit("STO", level - table.table[l][index].level, table.table[l][index].addr);
			//容易证明的是只要到了正确level那么addr肯定是正确的，这是由于addr生成的规律决定的
			//为什么层差就是正确的地址是因为，在call当中会给定层差，而我们生成call时传递的层差就是对的

		}
	}
	void statement_if() {
		if (words[nowNum].name != "if") {
			if (first_set["<lexp>"].count(words[nowNum].name)) {
				--nowNum;
				printError("缺少if", words[nowNum].column, words[nowNum].row);
			}
			else if (first_set["<lexp>"].count(words[nowNum + 1].name)) {
				printError("有错误的关键字" + words[nowNum].name + "，应该为if", words[nowNum].column, words[nowNum].row);
			}
		}
		nextWord();
		//交给lexp把一个判断操作结果值放到栈顶
		lexp();
		if (words[nowNum].name != "then") {
			if (first_set["<statement>"].count(words[nowNum].name)) {
				--nowNum;
				printError("缺少then", words[nowNum].column, words[nowNum].row);
			}
			else if (first_set["<statement>"].count(words[nowNum + 1].name)) {
				printError("有错误的关键字" + words[nowNum].name + "，应该为then", words[nowNum].column, words[nowNum].row);
			}
		}
		int current = code.len();//为了之后回填
		code.emit("JPC", 0, 0);//非真跳到else或者if的后面
		nextWord();
		statement();
		code.code[current].a = code.len();//跳到if后面，没有else就不用加一
		if (words[nowNum].name == "else") {
			//一样的回填，为了if为真时不在运行这里的statement
			code.code[current].a = code.len() + 1;//让JPC跳到else开头,因为还会生成下面JMP代码，所以要加1，
			current = code.len();
			code.emit("JMP", 0, 0);//这句实际上是if为真执行过后为了不执行statement的
			nextWord();
			statement();
			code.code[current].a = code.len();
		}

	}
	void statement_while() {
		//保存code开始语句,便于循环
		int whileStart = code.len();
		if (words[nowNum].name != "while") {
			if (first_set["<lexp>"].count(words[nowNum].name)) {
				--nowNum;
				printError("缺少while", words[nowNum].column, words[nowNum].row);
			}
			else if (first_set["<lexp>"].count(words[nowNum + 1].name)) {
				printError("有错误的关键字" + words[nowNum].name + "，应该为while", words[nowNum].column, words[nowNum].row);
			}
		}
		//依旧是lexp生成一个值放在栈顶

		nextWord();
		lexp();
		if (words[nowNum].name != "do") {
			if (first_set["<statement>"].count(words[nowNum].name)) {
				--nowNum;
				printError("缺少关键字do", words[nowNum].column, words[nowNum].row);
			}
			else if (first_set["<statement>"].count(words[nowNum + 1].name)) {
				printError("有错误的关键字" + words[nowNum].name + "，应该为do", words[nowNum].column, words[nowNum].row);
			}
		}
		int current = code.len();
		code.emit("JPC", 0, 0);
		nextWord();
		statement();
		code.emit("JMP", 0, whileStart);//回到最初的起点
		code.code[current].a = code.len();//跳出循环
	}
	void statement_call() {
		int l, index;
		int callNum = 0;
		if (words[nowNum].name != "call") {
			if (words[nowNum].type == "id" || words[nowNum].type == "nid") {
				--nowNum;
				printError("缺少关键字call", words[nowNum].column, words[nowNum].row);
			}
			else if (words[nowNum + 1].type == "id" || words[nowNum + 1].type == "nid") {
				printError("有错误的关键字" + words[nowNum].name + "，应该为call", words[nowNum].column, words[nowNum].row);
			}
		}
		nextWord();
		if (words[nowNum].type != "id" && words[nowNum].type != "nid") {
			if (words[nowNum].name == "(") {
				--nowNum;
				printError("缺少id", words[nowNum].column, words[nowNum].row);
			}
			else if (words[nowNum + 1].name == "(") {
				printError("有错误的id" + words[nowNum].name + "可能使用了保留字作为id？", words[nowNum].column, words[nowNum].row);
			}
		}
		else {
			l = table.find2(words[nowNum].name)[0], index = table.find(words[nowNum].name)[1];
			if (index == -1) {
				printError2(words[nowNum].name + "没有定义", words[nowNum].column, words[nowNum].row);
			}
			else if (table.table[l][index].type != "PROCEDURE") {
				printError2(words[nowNum].name + "不是函数", words[nowNum].column, words[nowNum].row);
			}
		}
		nextWord();
		if (words[nowNum].name != "(") {
			if (first_set["<exp>"].count(words[nowNum].name)) {
				printError("缺少(", words[nowNum].column, words[nowNum].row);
				--nowNum;
			}
			else if (first_set["<exp>"].count(words[nowNum + 1].name)) {
				printError("错误的符号" + words[nowNum].name + "应该为(", words[nowNum].column, words[nowNum].row);
			}
		}
		nextWord();
		int parameter = 3;//从第三个开始传递形式参数
		//exp最终会有一个值在栈顶的
		if (first_set["<exp>"].count(words[nowNum].name) || first_set["<exp>"].count(words[nowNum].type)) {
			exp();
			callNum += 1;
			code.emit("STO", -1, parameter++);//-1代表传参
		}
		while (first_set["<exp>"].count(words[nowNum].name)) {
			printError("缺少逗号", words[nowNum].column, words[nowNum].row);
			exp();
		}
		while (words[nowNum].name == ",") {
			nextWord();
			exp();
			callNum += 1;
			code.emit("STO", -1, parameter++);//-1代表传参
			while (first_set["<exp>"].count(words[nowNum].name) || first_set["<exp>"].count(words[nowNum].type)) {
				printError("缺少逗号", words[nowNum].column, words[nowNum].row);
				nextWord();
			}
		}
		if (words[nowNum].name != ")") {
			if (follow_set["<statement>"].count(words[nowNum].name)) {
				printError("缺少)", words[nowNum].column, words[nowNum].row);
				--nowNum;
			}
			else if (follow_set["<statement>"].count(words[nowNum + 1].name)) {
				printError("错误的符号" + words[nowNum].name + "应该为)", words[nowNum].column, words[nowNum].row);
			}
		}
		//确实是过程
		if (table.table[l][index].type == "PROCEDURE") {
			if (callNum != table.table[l][index].val) {
				char t[10];
				sprintf(t, "%d", table.table[l][index].val);
				printError2(table.table[l][index].name + "参数数目应该为"+t, words[nowNum].column, words[nowNum].row);
			}
			code.emit("CAL", level - table.table[l][index].level, table.table[l][index].addr);
		}
		else {
			wrong2 = true;
			printError2(words[nowNum].name + "不是过程名", words[nowNum].column, words[nowNum].row);
		}

		nextWord();
	}
	void statement_read() {
		int l, index;
		if (words[nowNum].name != "read") {
			if (words[nowNum].name == "(") {
				--nowNum;
				printError("缺少关键字read", words[nowNum].column, words[nowNum].row);
			}
			else if (words[nowNum + 1].name == "(") {
				printError("有错误的关键字" + words[nowNum].name + "，应该为read", words[nowNum].column, words[nowNum].row);
			}
		}
		nextWord();
		if (words[nowNum].name != "(") {
			if (words[nowNum].type == "id" || words[nowNum].type == "uid") {
				--nowNum;
				printError("缺少(", words[nowNum].column, words[nowNum].row);
			}
			else if (words[nowNum + 1].type == "id" || words[nowNum + 1].type == "uid") {
				printError("有错误的单词" + words[nowNum].name + "，应该为(", words[nowNum].column, words[nowNum].row);
			}
		}
		nextWord();
		if (words[nowNum].type != "id" && words[nowNum].type != "nid") {
			if (words[nowNum].name == "," || words[nowNum].name == ")") {
				--nowNum;
				printError("缺少id", words[nowNum].column, words[nowNum].row);
			}
			else if (words[nowNum + 1].name == "," || words[nowNum + 1].name == ")") {
				printError("有错误的id" + words[nowNum].name + "可能使用了保留字作为id？", words[nowNum].column, words[nowNum].row);
			}
			else {
				printError("read中非法的单词", words[nowNum].column, words[nowNum].row);
			}
		}
		else {
			//是id的话准备读入
			l = table.find(words[nowNum].name)[0], index = table.find(words[nowNum].name)[1];
			if (index == -1) {
				printError2(words[nowNum].name + "没有定义", words[nowNum].column, words[nowNum].row);
			}
			else if (table.table[l][index].type != "VARIABLE") {
				printError2(words[nowNum].name + "不是变量", words[nowNum].column, words[nowNum].row);
			}
			else {
				//确实找到了定义
				//cout<<words[nowNum].name<<" "<<l<<" "<<table.table[l][index].addr<<endl;
				code.emit("RED", 0, 0);//把一个数据读入栈顶
				code.emit("STO", level - table.table[l][index].level, table.table[l][index].addr);//把栈顶数据存好
			}
		}
		nextWord();
		while (words[nowNum].type == "nid" || words[nowNum].type == "id") {
			//if (words[nowNum].type == "id") printError("id" + words[nowNum].name + "重复定义", words[nowNum].column, words[nowNum].row);
			printError("缺少逗号", words[nowNum].column, words[nowNum].row);
			nextWord();
		}
		while (words[nowNum].name == ",")
		{
			nextWord();
			if (words[nowNum].name == ")") break;
			if (words[nowNum].type != "id" && words[nowNum].type != "nid") {
				if (words[nowNum].name == "," || words[nowNum].name == ")") {
					--nowNum;
					printError("缺少id", words[nowNum].column, words[nowNum].row);
				}
				else if (words[nowNum + 1].name == ",") {
					printError("有错误的id" + words[nowNum].name + "可能使用了保留字作为id？", words[nowNum].column, words[nowNum].row);
				}
				else {
					printError("read中非法的单词", words[nowNum].column, words[nowNum].row);
				}
			}
			else {
				//是id的话准备读入
				l = table.find(words[nowNum].name)[0], index = table.find(words[nowNum].name)[1];
				if (index == -1) {
					printError2(words[nowNum].name + "没有定义", words[nowNum].column, words[nowNum].row);
				}
				else if (table.table[l][index].type != "VARIABLE") {
					printError2(words[nowNum].name + "不是变量", words[nowNum].column, words[nowNum].row);
				}
				else {
					//确实找到了定义
				//	cout<<words[nowNum].name<<" "<<l<<" "<<table.table[l][level].addr<<endl;
					code.emit("RED", 0, 0);//把一个数据读入栈顶
					code.emit("STO", level - table.table[l][index].level, table.table[l][index].addr);//把栈顶数据存好
				}
			}
			nextWord();//似乎有点粗糙，万一中间只少了一个，也会直接开始同步,试着解决一下
			while (words[nowNum].type == "nid" || words[nowNum].type == "id") {
				if (words[nowNum].type == "id") printError("id" + words[nowNum].name + "重复定义", words[nowNum].column, words[nowNum].row);
				printError("缺少逗号", words[nowNum].column, words[nowNum].row);
				nextWord();
			}
		}
		if (words[nowNum].name != ")") {
			if (follow_set["<statement>"].count(words[nowNum].name)) {
				printError("缺少)", words[nowNum].column, words[nowNum].row);
				--nowNum;
			}
			else if (follow_set["<statement>"].count(words[nowNum + 1].name)) {
				printError("错误的符号" + words[nowNum].name + "应该为)", words[nowNum].column, words[nowNum].row);
			}
			else {
				printError("错误的符号" + words[nowNum].name + "应该为)", words[nowNum].column, words[nowNum].row);
			}
		}
		nextWord();
		while (!(follow_set["<statement>"].count(words[nowNum].name) || follow_set["<statement>"].count(words[nowNum].type))) {
			printError("正在同步<statement>,跳过 " + words[nowNum].name, words[nowNum].column, words[nowNum].row);
			nextWord();
		}
		nextWord();
	}
	void statement_write() {
		int l, index;
		if (words[nowNum].name != "write") {
			if (words[nowNum].name == "(") {
				--nowNum;
				printError("缺少关键字write", words[nowNum].column, words[nowNum].row);
			}
			else if (words[nowNum + 1].name == "(") {
				printError("有错误的关键字" + words[nowNum].name + "，应该为write", words[nowNum].column, words[nowNum].row);
			}
		}
		nextWord();
		if (words[nowNum].name != "(") {
			if (first_set["<exp>"].count(words[nowNum].name)) {
				--nowNum;
				printError("缺少(", words[nowNum].column, words[nowNum].row);
			}
			else if (first_set["<exp>"].count(words[nowNum + 1].name)) {
				printError("有错误的单词" + words[nowNum].name + "，应该为(", words[nowNum].column, words[nowNum].row);
			}
		}
		nextWord();
		exp();
		//exp以后会有一个数字在栈顶的，要输出的就是这个
		code.emit("WRT", 0, 0);
		while (first_set["<exp>"].count(words[nowNum].name)) {
			printError("缺少逗号", words[nowNum].column, words[nowNum].row);
			exp();
		}
		while (words[nowNum].name == ",") {
			nextWord();
			exp();
			//TEST;
			//exp以后会有一个数字在栈顶的，要输出的就是这个
			code.emit("WRT", 0, 0);
			while (first_set["<exp>"].count(words[nowNum].name)) {
				printError("缺少逗号", words[nowNum].column, words[nowNum].row);
				nextWord();
			}
		}
		if (words[nowNum].name != ")") {
			if (follow_set["<statement>"].count(words[nowNum].name)) {
				printError("缺少)", words[nowNum].column, words[nowNum].row);
				--nowNum;
			}
			else if (follow_set["<statement>"].count(words[nowNum + 1].name)) {
				printError("错误的符号" + words[nowNum].name + "应该为)", words[nowNum].column, words[nowNum].row);
			}
		}
		nextWord();
	}
	void exp() {
		bool flag = false;
		if (words[nowNum].name == "+" || words[nowNum].name == "-") {
			if (words[nowNum].name == "-") flag = true;
			nextWord();
		}
		term();//应该细化判断
		if (flag) {
			//取反
			code.emit("OPR", 0, 1);
		}
		while (words[nowNum].name == "+" || words[nowNum].name == "-") {
			int tmp = nowNum;
			nextWord();
			term();
			if (words[tmp].name == "+") {
				code.emit("OPR", 0, 2);//相加
			}
			else {
				code.emit("OPR", 0, 3);//相减
			}

			while (first_set["<term>"].count(words[nowNum].name)) {
				printError("缺少<aop>", words[nowNum].column, words[nowNum].row);
				term();
			}
		}
		//		while(!follow_set["<exp>"].count(words[nowNum].name)){
		//			printError("正在同步<exp>,跳过 "+words[nowNum].name,words[nowNum].column,words[nowNum].row);
		//			nextWord();
		//		}

	}
	void lexp() {
		if (words[nowNum].name == "odd") {
			nextWord();
			//exp会把一个要判断的放到栈顶的
			exp();
			code.emit("OPR", 0, 6);//判断奇偶
		}
		else if (first_set["<exp>"].count(words[nowNum + 1].name) || first_set["<exp>"].count(words[nowNum + 1].type)) {
			printError("错误关键字" + words[nowNum].name + "，应该为odd", words[nowNum].column, words[nowNum].row);
			nextWord();
			exp();
		}
		else {
			if (!(first_set["<exp>"].count(words[nowNum + 1].name) || first_set["<exp>"].count(words[nowNum + 1].type)))
			{
				if (words[nowNum].name == "=" || words[nowNum].name == "<>" || words[nowNum].name == "<" || words[nowNum].name == "<=" || words[nowNum].name == ">" || words[nowNum].name == ">=") {
					printError("缺少<exp>内容", words[nowNum].column, words[nowNum].row);
				}
				else {
					//TEST;
					exp();
				}
			}
			else {
				exp();
			}
			if (!(words[nowNum].name == "=" || words[nowNum].name == "<>" || words[nowNum].name == "<" || words[nowNum].name == "<=" || words[nowNum].name == ">" || words[nowNum].name == ">=")) {
				if (first_set["<exp>"].count(words[nowNum].name) || first_set["<exp>"].count(words[nowNum].type)) {
					--nowNum;
					printError("缺少<lop>符号", words[nowNum].column, words[nowNum].row);
				}
				else if (first_set["<exp>"].count(words[nowNum + 1].name) || first_set["<exp>"].count(words[nowNum + 1].type)) {
					printError("有错误的单词" + words[nowNum].name + "，应该为<lop>符号", words[nowNum].column, words[nowNum].row);
				}
				else {
					printError("有错误的单词" + words[nowNum].name + "，应该为<lop>符号", words[nowNum].column, words[nowNum].row);
				}
				//虽然出错了但还是得继续语法分析的
				nextWord();
				exp();
			}
			else {
				string sign = words[nowNum].name;//必定是比较符号之一，因为在else中
				nextWord();
				exp();
				if (sign == "=") {
					code.emit("OPR", 0, 7);
				}
				else if (sign == "<>") {
					code.emit("OPR", 0, 8);
				}
				else if (sign == "<") {
					code.emit("OPR", 0, 9);
				}
				else if (sign == "<=") {
					code.emit("OPR", 0, 12);
				}
				else if (sign == ">") {
					code.emit("OPR", 0, 10);
				}
				else if (sign == ">=") {
					code.emit("OPR", 0, 11);
				}

			}

		}

		while (!follow_set["<lexp>"].count(words[nowNum].name)) {
			printError("正在同步<lexp>,跳过 " + words[nowNum].name, words[nowNum].column, words[nowNum].row);
			nextWord();
		}
	}
	void term() {
		factor();
		//		while(first_set["<factor>"].count(words[nowNum].name)||first_set["<factor>"].count(words[nowNum].type)){
		//			printError("缺少<mop>",words[nowNum].column,words[nowNum].row);
		//			factor();
		//		}
		while (words[nowNum].name == "*" || words[nowNum].name == "/") {
			int tmp = nowNum;
			nextWord();
			factor();
			if (words[tmp].name == "*") {
				code.emit("OPR", 0, 4);//相乘
			}
			else {
				code.emit("OPR", 0, 5);//相除
			}
		}

	}
	void factor() {
		int l, index;
		if (words[nowNum].type == "nid" || words[nowNum].type == "id" || words[nowNum].type == "integer") {
			l = table.find(words[nowNum].name)[0], index = table.find(words[nowNum].name)[1];
			if (index == -1) {
				if (words[nowNum].type == "integer") {
					//虽然没有找到，不过发现是一个数字
					code.emit("LIT", 0, stoi(words[nowNum].name));
				}
				else
				{
					printError2(words[nowNum].name + "没有定义", words[nowNum].column, words[nowNum].row);
				}

			}
			else {
				if (table.table[l][index].type == "CONSTANT")//常量
				{
					code.emit("LIT", 0, table.table[l][index].val);
				}
				else if (table.table[l][index].type == "VARIABLE")//变量
				{
					code.emit("LOD", level - table.table[l][index].level, table.table[l][index].addr);
				}
				else {
					printError2(words[nowNum].name + "为过程名", words[nowNum].column, words[nowNum].row);
				}
			}
			nextWord();
			return;
		}
		else if (words[nowNum].name == "(") {
			//左括号进入表达式
			nextWord();
			exp();
			if (words[nowNum].name != ")") {
				if (follow_set["<factor>"].count(words[nowNum].name)) {
					printError("缺少)", words[nowNum].column, words[nowNum].row);
					--nowNum;
				}
				else if (follow_set["<factor>"].count(words[nowNum + 1].name)) {
					printError("错误的符号" + words[nowNum].name + "应该为)", words[nowNum].column, words[nowNum].row);
				}
			}
			nextWord();
		}
	}
};


int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		vector<string> f = { "错误代码.txt","sum.txt","sum2-传参.txt","fibnacci.txt","fibnacciRead.txt" ,"add_sum_fib.txt","回文数.txt" };
		GrammarAnalyse a(f[6]);
		a.analyse();
		code.write2File();
		a.close();
		return 0;
	}
	else {
		cout << "命令行传参:" << argv[1] << endl;
		GrammarAnalyse a(argv[1]);
		a.analyse();
		code.write2File();
		a.close();
		return 0;
	}

}