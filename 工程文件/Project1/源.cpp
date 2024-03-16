#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <vector>
#include <set>
#define TEST cout<<"---------------"<<words[nowNum].type<<"    "<<words[nowNum].name<<"   �У�"<<words[nowNum].column<<"     �У�"<<words[nowNum].row<<endl;
using namespace std;


// ʹ���ַ��ָ�
void Stringsplit(const string& str, const char split, vector<string>& res)
{
	if (str == "")        return;
	//���ַ���ĩβҲ����ָ����������ȡ���һ��
	string strs = str + split;
	size_t pos = strs.find(split);
	size_t last = 0;
	// ���Ҳ����������ַ��������������� npos
	while (pos != strs.npos)
	{

		string temp = strs.substr(last, pos - last);//��last��pos������Ϊpos-last
		res.push_back(temp);
		last = pos + 1;
		pos = strs.find(split, pos + 1);
	}
}

int level = 0;//���
int space = 3;//���ٵĿռ�
bool generate = true;//��������˾Ͳ����ɴ���

class OneCode {
public:
	OneCode() {};
	OneCode(string f, int l, int a1) :func(f), L(l), a(a1) {};
	string func;
	int L;
	int a;//��ַƫ������������ת�Ǿ��Ե�ַ�����ڳ�����˵��¼��ֵ
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
		cout << "�����������" << endl;
	}
	int len() {
		return nowCode;//nowcodeָ����һ��д��ĵط�
	}
};
Code code;

class TableItem {
public:
	TableItem(string n, string t, int l, int v, int a) :name(n), type(t), level(l), val(v), addr(a) {};
	string name;
	string type;
	int level; //���
	int val;
	int addr;
};
class Table {
public:
	map<int, vector<TableItem>> table;//keyΪlevel��value��ʵ��һ��TableItem�Ϳ����ˣ���Ϊͬһʱ������ÿ��ֻ������һ������
	Table() {}
	void recordTable(string n, string t, int l = 0, int v = 0, int a = 0) {
		table[l].push_back(TableItem(n, t, l, v, a));
	}
	//����name�ڷ��ű��λ��
	vector<int> find(string n)
	{
		//�ȴ�����level����
		for (int i = level;i >= 0;i--) {
			for (int j = table[i].size() - 1;j >= 0;--j) {
				if (table[i][j].name == n) return { i,j };
			}
		}
		return { -1,-1 };
	}
	vector<int> find2(string n)
	{
		//���ҹ��̣��������ӹ��̣�����Ҫ��ô��
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

//�ʷ�������������һ�����ʵĽṹ
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
			//��ʶ��
			if (code == 0) {
				InsertId();
				//output<<"(id,"+strToken +','+ to_string(worldNum) +")"<<endl;
				//fprintf(output,"(id,%s,%d)\n",strToken.c_str(),worldNum);
				sprintf(output2, "nid %s %d %d", strToken.c_str(), line, column - 1);
				//cout<<"(id,"+strToken +','+ to_string(worldNum) +")"<<endl;

			}
			//������
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
			//����С����
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
			//����111aaa
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
				//fprintf(output,"Error: �Ƿ���־�� %s ������ ��:%d, ��:%d.\n",strToken.c_str(),line,column-1);
				printf("�ʷ�����: �Ƿ���־�� %s ������ ��:%d, ��:%d.\n", strToken.c_str(), line, column - 1);
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
						printf("�ʷ�����: ��������� '%s' ������ ��:%d, ��:%d�������Ƿ�ȱʧ����\n", strToken.c_str(), line, column - 1);
						generate = false;
						//fprintf(output,"Error: ��������� '%s' ������ ��:%d, ��:%d�������Ƿ�ȱʧ����\n",strToken.c_str(),line,column-1);						
					}
				}
			}
		}
		else {
			if (strToken != "\n" && strToken != "")
			{
				printf("�ʷ�����: ��������� '%s' ������ ��:%d, ��:%d�������Ƿ�ȱʧ����\n", strToken.c_str(), line, column - 1);
				generate = false;
				//fprintf(output,"Error: ��������� '%s' ������ ��:%d, ��:%d�������Ƿ�ȱʧ����\n",strToken.c_str(),line,column-1);				
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
	bool wrong = false;//�﷨����
	bool wrong2 = false;//�������
	vector<OneWord> words;//�洢����
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
	void printError(string info, int row, int column)//�﷨����
	{
		generate = false;
		wrong = true;
		//if(info.find("ͬ��")!=info.npos) return;
		printf("�﷨����row:%2d,column:%2d ", row, column);
		printf(info.c_str());
		errors.push_back(error(info, row, column));
		printf("\n");
	}
	void printError2(string info, int row, int column)//�������
	{
		generate = false;
		wrong2 = true;
		//if(info.find("ͬ��")!=info.npos) return;
		printf("�������row:%2d,column:%2d ", row, column);
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
			cout << "�﷨��������,�������﷨����" << endl;
			if (!wrong2) cout << "�����������,�����Ѿ�����" << endl;
			else cout << "�����������,��������������" << endl;
		}
		if (wrong) cout << "�﷨��������,��������﷨����" << endl;

	}
	void prog()
	{
		if (words[nowNum].name != "program")
		{
			if (words[nowNum].type == "nid")
			{
				printError("ȱ��program�ؼ���", words[nowNum].column, words[nowNum].row);
				//--nowNum;

			}
			else if (words[nowNum + 1].name == "program") {
				printError("���ִ��󵥴�" + words[nowNum].name, words[nowNum].column, words[nowNum].row);
			}
			else if (words[nowNum + 1].type == "nid")
			{
				printError("program�ؼ���ƴд����", words[nowNum].column, words[nowNum].row);
			}
			else {

			}
		}
		nextWord();
		if (words[nowNum].type != "nid") {
			if (words[nowNum].name == ";")
			{
				printError("ȱ��id", words[nowNum].column, words[nowNum].row);
				--nowNum;
			}
			else if (words[nowNum + 1].type == "nid") {
				printError("���ִ��󵥴�" + words[nowNum].name, words[nowNum].column, words[nowNum].row);
			}
			else if (words[nowNum + 1].name == ";")
			{
				printError("ʹ�ùؼ�����Ϊid�������Ǵ����id", words[nowNum].column, words[nowNum].row);
			}
			else {

			}
		}
		nextWord();
		while (words[nowNum].type == "nid" || words[nowNum].type == "id") {
			if (words[nowNum].type == "id") printError("id  " + words[nowNum].name + "  �ظ�����", words[nowNum].column, words[nowNum].row);
			printError("program�󲻿��ж��id", words[nowNum].column, words[nowNum].row);
			nextWord();
		}
		if (words[nowNum].name != ";") {
			if (first_set["<block>"].count(words[nowNum].name))//������block��first���У�˵��ȱʧ;
			{
				--nowNum;
				printError("ȱʧ�ֺ�", words[nowNum].column, words[nowNum].row);
			}
			else if (first_set["<block>"].count(words[nowNum + 1].name))//��һ��������block��first����˵��������Ŵ���
			{
				printError("����ķ��� " + words[nowNum].name + " Ӧ��Ϊ�ֺ�", words[nowNum].column, words[nowNum].row);
			}
		}
		nextWord();
		block();
	}
	void block()
	{
		//space��һ�ν���blockΪ3��֮���չ����
		//��¼��ǰ������������ڻ���
		int current = code.len();//��һ�����ɵĵ�ַ�����ڻ���653��
		code.emit("JMP", 0, 0);

		if (words[nowNum].name == "const") {
			condecl();
		}
		if (words[nowNum].name == "var") {
			vardecl();
		}
		if (words[nowNum].name == "procedure") {
			int nowSpace = space;//�������ڵĿռ�
			proc();
			space = nowSpace;//�ָ�
		}
		//����bodyǰ��Ҫ���ͷ��jumpָ���Ϊ������˳�����ɵģ���������ִ�е�ʱ����body�ȿ�ʼ�ģ�����Ҫ����body����
		code.code[current].a = code.len();//���뵽body����
		code.emit("INT", 0, space);//���ٿռ�
		body();
		code.emit("OPR", 0, 0);//���ص��õ㲢��ջ
		if (words[nowNum].name == ".") return;
		while (!follow_set["<block>"].count(words[nowNum].name)) {
			printError("����ͬ��<block>,���� " + words[nowNum].name, words[nowNum].column, words[nowNum].row);
			nextWord();
		}
	}
	void condecl()
	{
		nextWord();//��һ������϶���const
		_const();
		while (words[nowNum].name == "," || words[nowNum].type == "nid")
		{
			if (words[nowNum].name == ",")
			{
				nextWord();
				_const();
			}
			else {
				printError("const��ȱ�ٶ��ż����", words[nowNum].column, words[nowNum].row);
				_const();
			}

		}
		if (words[nowNum].name == ";") {
			nextWord();
			return;
		}
		else {
			printError("ȱʧ�ֺ�", words[nowNum].column, words[nowNum].row);
		}
		while (!follow_set["<condecl>"].count(words[nowNum].name)) {
			printError("����ͬ��<condecl>,���� " + words[nowNum].name, words[nowNum].column, words[nowNum].row);
			nextWord();
		}
		//����ָ����� var procedure ���� begin

	}
	void _const()
	{
		if (words[nowNum].type != "nid" && words[nowNum].type != "id") {
			if (words[nowNum].name == ":=") {
				printError("ȱ��id", words[nowNum].column, words[nowNum].row);
				--nowNum;
			}
			else if (words[nowNum + 1].name == ":=") {

				printError("�����id" + words[nowNum].name + ",����ʹ���˹ؼ�����Ϊ��ʶ����", words[nowNum].column, words[nowNum].row);
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
					printError("ȱ�ٸ�ֵ����", words[nowNum].column, words[nowNum].row);
					--nowNum;
				}
				else if (words[nowNum + 1].type == "integer") {
					printError("����ĸ�ֵ����" + words[nowNum].name, words[nowNum].column, words[nowNum].row);
				}
				nextWord();
				nextWord();
			}
			else {
				nextWord();
				if (words[nowNum].type != "integer") {
					if (words[nowNum].name == "," || words[nowNum].name == ";") {
						printError("ȱ������", words[nowNum].column, words[nowNum].row);
					}
					else if (words[nowNum + 1].name == "," || words[nowNum + 1].name == ";") {
						printError("�д��������" + words[nowNum].name, words[nowNum].column, words[nowNum].row);
					}
				}
				else {
					table.recordTable(constname, "CONSTANT", level, stoi(words[nowNum].name));
				}
				nextWord();
			}

		}

		while (!follow_set["<const>"].count(words[nowNum].name) && words[nowNum].type != "nid") {
			printError("����ͬ��<const>,���� " + words[nowNum].name, words[nowNum].column, words[nowNum].row);
			nextWord();
		}
	}
	void vardecl() {
		nextWord();//��һ������϶���var
		if (words[nowNum].type != "nid" && words[nowNum].type != "id") {
			if (words[nowNum].name == ",") {
				--nowNum;
				printError("ȱ��id", words[nowNum].column, words[nowNum].row);
			}
			else if (words[nowNum + 1].name == ",") {
				printError("�д����id" + words[nowNum].name + "����ʹ���˱�������Ϊid��", words[nowNum].column, words[nowNum].row);
			}
		}
		else {
			//��id
			table.recordTable(words[nowNum].name, "VARIABLE", level, 0, space);
			//cout<<words[nowNum].name<<" "<<level<<endl;
			space++;//�ռ�����
		}
		nextWord();
		while (words[nowNum].type == "nid" || words[nowNum].type == "id") {
			//if (words[nowNum].type == "id") printError("id" + words[nowNum].name + "�ظ�����", words[nowNum].column, words[nowNum].row);
			printError("var��ȱ�ٶ���", words[nowNum].column, words[nowNum].row);
			nextWord();
		}
		while (words[nowNum].name == ",")
		{
			nextWord();
			if (words[nowNum].name == ";") break;
			if (words[nowNum].type != "nid" && words[nowNum].type != "id") {
				if (words[nowNum].name == ",") {
					printError("ȱ��id", words[nowNum].column, words[nowNum].row);
					--nowNum;
				}
				else if (words[nowNum + 1].name == ",") {
					printError("�д����id" + words[nowNum].name + "����ʹ���˱�������Ϊid��", words[nowNum].column, words[nowNum].row);
				}
			}
			else {
				//��id
				//cout<<words[nowNum].name<<" "<<space<<endl;
				table.recordTable(words[nowNum].name, "VARIABLE", level, 0, space);
				//cout << words[nowNum].name << " " << level << endl;
				space++;//�ռ�����
			}
			nextWord();//�ƺ��е�ֲڣ���һ�м�ֻ����һ����Ҳ��ֱ�ӿ�ʼͬ��,���Ž��һ��
			while (words[nowNum].type == "nid" || words[nowNum].type == "id") {
				if (words[nowNum].type == "id") printError("id" + words[nowNum].name + "�ظ�����", words[nowNum].column, words[nowNum].row);
				printError("ȱ�ٶ���", words[nowNum].column, words[nowNum].row);
				nextWord();
			}

		}
		if (words[nowNum].name == ";")
		{
			nextWord();//��������
			return;
		}
		while (!follow_set["<vardecl>"].count(words[nowNum].name)) {
			printError("����ͬ��<vardecl>,���� " + words[nowNum].name, words[nowNum].column, words[nowNum].row);
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
				printError("ȱ��procedure�ؼ���", words[nowNum].column, words[nowNum].row);
			}
			else if (words[nowNum + 1].type == "id") {
				printError("����Ĺؼ���" + words[nowNum].name + "Ӧ��Ϊprocedure", words[nowNum].column, words[nowNum].row);
			}
		}
		nextWord();
		if (words[nowNum].type != "nid" && words[nowNum].type != "id") {
			if (words[nowNum].name == "(") {
				--nowNum;
				printError("ȱ��id", words[nowNum].column, words[nowNum].row);
			}
			else if (words[nowNum + 1].name == "(") {
				printError("�д����id" + words[nowNum].name + "����ʹ���˱�������Ϊid��", words[nowNum].column, words[nowNum].row);
			}
		}
		else {
			//��id����д���ű�
			tmp = words[nowNum].name;
			proca = table.table[level].size();
			table.recordTable(words[nowNum].name, "PROCEDURE", level, 0, code.len());//�µĹ��̵ĵ�ַ �� ��һ��д��Ĵ���λ�ã�Ҳ�������Լ��ĵ�һ�������ַ
			
		}
		nextWord();
		while (words[nowNum].type == "nid" || words[nowNum].type == "id") {
			//if (words[nowNum].type == "id") printError("id  " + words[nowNum].name + "  �ظ�����", words[nowNum].column, words[nowNum].row);
			printError("procedure�󲻿��ж��id", words[nowNum].column, words[nowNum].row);
			nextWord();
		}
		if (words[nowNum].name != "(") {

			if (words[nowNum].name == ")" || words[nowNum].type == "nid" || words[nowNum].type == "id") {
				printError("ȱ�� ( ", words[nowNum].column, words[nowNum].row);
				--nowNum;//����һ������
			}
			else if (words[nowNum].name == "," || words[nowNum + 1].name == ")" || words[nowNum + 1].type == "nid") {
				printError("Ӧ��Ϊ ( �������˴���ĵ��ʣ�" + words[nowNum].name + "����ʹ���˱�������Ϊid��", words[nowNum].column, words[nowNum].row);
			}
			//����ȫ�����꣬Ҫ�����﷨����
			while (!first_set["<body>"].count(words[nowNum].name)) {
				printError("����ͬ��<proc>,���� " + words[nowNum].name, words[nowNum].column, words[nowNum].row);
				nextWord();
			}
		}
		else {
			//Ϊ'('
			nextWord();
			if (words[nowNum].type == "nid" || words[nowNum].type == "id") {
				//��id,��д��
				table.recordTable(words[nowNum].name, "VARIABLE", level + 1, 0, space);//�ر�ע�������Ϊ�β�ʵ������һ����
				space++;//�ռ�����
				nextWord();
				while (words[nowNum].type == "nid" || words[nowNum].type == "id") {
					printError("ȱ�ٶ���", words[nowNum].column, words[nowNum].row);
					nextWord();
				}
				while (words[nowNum].name == ",")
				{
					nextWord();
					if (words[nowNum].name == ")") break;
					if (words[nowNum].type != "nid" && words[nowNum].type != "id") {
						if (words[nowNum].name == ",") {
							printError("ȱ��id", words[nowNum].column, words[nowNum].row);
							--nowNum;
						}
						else if (words[nowNum + 1].name == ",") {
							printError("�д����id" + words[nowNum].name + "����ʹ���˱�������Ϊid��", words[nowNum].column, words[nowNum].row);
						}
					}
					else {
						//��id,��д��
						table.recordTable(words[nowNum].name, "VARIABLE", level + 1, 0, space);//��Ϊ�β�ʵ������һ����
						space++;//�ռ�����
					}
					nextWord();//�ƺ��е�ֲڣ���һ�м�ֻ����һ����Ҳ��ֱ�ӿ�ʼͬ��,���Ž��һ��
					//cout << words[nowNum].name << endl;
					while (words[nowNum].type == "nid" || words[nowNum].type == "id") {
						if (words[nowNum].type == "id") printError("id" + words[nowNum].name + "�ظ�����", words[nowNum].column, words[nowNum].row);
						printError("ȱ�ٶ���", words[nowNum].column, words[nowNum].row);
						nextWord();
					}

				}
			}

			if (words[nowNum].name != ")") {
				if (words[nowNum].name == ";") {
					--nowNum;
					printError("ȱ�� ) ", words[nowNum].column, words[nowNum].row);
				}
				else if (words[nowNum + 1].name == ";") {
					printError("Ӧ��Ϊ ) �������˴���ĵ��ʣ�" + words[nowNum].name, words[nowNum].column, words[nowNum].row);
				}
			}
			nextWord();
			if (words[nowNum].name != ";") {
				if (first_set["<block>"].count(words[nowNum].name)) {
					--nowNum;
					printError("ȱ�� ; ", words[nowNum].column, words[nowNum].row);
				}
				else if (first_set["<block>"].count(words[nowNum + 1].name)) {
					printError("Ӧ��Ϊ ; �������˴���ĵ��ʣ�" + words[nowNum].name, words[nowNum].column, words[nowNum].row);
				}
			}
		}
		table.table[level][proca].val = space-3;
		//�ڽ���blockǰ��Ҫ���Ĺ���
		level += 1;
		nextWord();//ָ��block��һ����
		block();
		//������ָ�
		level -= 1;

		//���ٸò����
		int t = table.table[level + 1].size();
		for (int i = 0;i < t;i++)
		{
			table.table[level + 1].pop_back();
		}

		//�Ƿֺż���proc
		if (words[nowNum].name == ";")//��Ҫ��˼��һ�� ��if����while
		{
			nextWord();
			proc();
		}
		while (!follow_set["<proc>"].count(words[nowNum].name)) {
			if (words[nowNum].type == "ProEnd")	return;
			printError("����ͬ��<proc>,���� " + words[nowNum].name, words[nowNum].column, words[nowNum].row);
			nextWord();
		}
	}
	void body()
	{
		if (words[nowNum].name != "begin") {
			if (first_set["<statement>"].count(words[nowNum].name) || first_set["<statement>"].count(words[nowNum].type)) {
				--nowNum;
				printError("ȱ��begin�ؼ���", words[nowNum].column, words[nowNum].row);
			}
			else if (first_set["<statement>"].count(words[nowNum + 1].name)) {
				printError("����Ĺؼ���" + words[nowNum].name + "Ӧ��Ϊbegin", words[nowNum].column, words[nowNum].row);
			}
		}
		nextWord();
		statement();

		while (words[nowNum].name == ";" || (first_set["<statement>"].count(words[nowNum].name) || first_set["<statement>"].count(words[nowNum].type)))
		{
			if (first_set["<statement>"].count(words[nowNum].name) || first_set["<statement>"].count(words[nowNum].type)) {
				--nowNum;
				if (words[nowNum].name != ";")
					printError("ȱ��;", words[nowNum].column, words[nowNum].row);
			}
			nextWord();
			statement();
		}
		//����end

		if (words[nowNum].name != "end") {
			if (follow_set["<body>"].count(words[nowNum].name)) {
				--nowNum;
				printError("<body>ȱ��end�ؼ���", words[nowNum].column, words[nowNum].row);
			}
			else if (follow_set["<statement>"].count(words[nowNum + 1].name)) {
				printError("<body>����Ĺؼ���" + words[nowNum].name + "Ӧ��Ϊend", words[nowNum].column, words[nowNum].row);
			}
		}
		nextWord();
		while (!follow_set["<body>"].count(words[nowNum].name)) {
			printError("����ͬ��<body>,���� " + words[nowNum].name, words[nowNum].column, words[nowNum].row);
			nextWord();
		}
	}
	void statement() {
		if (words[nowNum].name == "#") return;//��ֹ����дend
		if (words[nowNum].name == "end") {
			printError("��Ӧ���зֺ�", words[nowNum].column, words[nowNum].row);
			return;//��ֹ����;������statment������end
		}

		while (!(first_set["<statement>"].count(words[nowNum].name) || first_set["<statement>"].count(words[nowNum].type) || (follow_set["<statement>"].count(words[nowNum].name) || follow_set["<statement>"].count(words[nowNum].type)))) {
			printError("����ͬ����<statement>��ͷ�����ʡ�" + words[nowNum].name + "�����ܶ���", words[nowNum].column, words[nowNum].row);
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
			if (words[nowNum].name == "#") return;//��ֹ����дend
			if (words[nowNum].name == "end") {
				printError("��Ӧ���зֺ�", words[nowNum].column, words[nowNum].row);
				return;//��ֹ����;������statment������end
			}

			printError("����ͬ��<statement>,���� " + words[nowNum].name, words[nowNum].column, words[nowNum].row);
			nextWord();
		}
	}
	void statement_id() {
		int l, index;
		if (words[nowNum].type != "id") {
			if (words[nowNum].type == "nid") {
				printError("id " + words[nowNum].name + " ��δ����", words[nowNum].column, words[nowNum].row);
			}
			else if (words[nowNum].name == ":=") {
				--nowNum;
				printError("ȱ��id", words[nowNum].column, words[nowNum].row);
			}
			else if (words[nowNum + 1].name == ":=") {
				printError("�д����id" + words[nowNum].name + "����ʹ���˱�������Ϊid��", words[nowNum].column, words[nowNum].row);
			}
		}
		else {
			//ȷʵ��id
			l = table.find(words[nowNum].name)[0], index = table.find(words[nowNum].name)[1];
			if (index == -1) {
				printError2(words[nowNum].name + "û�ж���", words[nowNum].column, words[nowNum].row);
			}
			else if (table.table[l][index].type != "VARIABLE") {
				printError2(words[nowNum].name + "���Ǳ���", words[nowNum].column, words[nowNum].row);
			}

		}
		nextWord();
		if (words[nowNum].name != ":=") {
			if (first_set["<exp>"].count(words[nowNum].name) || first_set["<exp>"].count(words[nowNum].type)) {
				printError("ȱ�ٸ�ֵ����", words[nowNum].column, words[nowNum].row);
				--nowNum;
			}
			else if (first_set["<exp>"].count(words[nowNum + 1].name) || first_set["<exp>"].count(words[nowNum + 1].type)) {
				printError("����ĸ�ֵ����" + words[nowNum].name, words[nowNum].column, words[nowNum].row);
			}
			else {
				printError("���ˣ�����֪��������ʲô" + words[nowNum].name, words[nowNum].column, words[nowNum].row);
			}
		}
		nextWord();
		exp();
		//�Ϸ���������Ӧ�ð�exp������ֵ������
		if (index != -1) {
			code.emit("STO", level - table.table[l][index].level, table.table[l][index].addr);
			//����֤������ֻҪ������ȷlevel��ôaddr�϶�����ȷ�ģ���������addr���ɵĹ��ɾ�����
			//Ϊʲô��������ȷ�ĵ�ַ����Ϊ����call���л����������������callʱ���ݵĲ����ǶԵ�

		}
	}
	void statement_if() {
		if (words[nowNum].name != "if") {
			if (first_set["<lexp>"].count(words[nowNum].name)) {
				--nowNum;
				printError("ȱ��if", words[nowNum].column, words[nowNum].row);
			}
			else if (first_set["<lexp>"].count(words[nowNum + 1].name)) {
				printError("�д���Ĺؼ���" + words[nowNum].name + "��Ӧ��Ϊif", words[nowNum].column, words[nowNum].row);
			}
		}
		nextWord();
		//����lexp��һ���жϲ������ֵ�ŵ�ջ��
		lexp();
		if (words[nowNum].name != "then") {
			if (first_set["<statement>"].count(words[nowNum].name)) {
				--nowNum;
				printError("ȱ��then", words[nowNum].column, words[nowNum].row);
			}
			else if (first_set["<statement>"].count(words[nowNum + 1].name)) {
				printError("�д���Ĺؼ���" + words[nowNum].name + "��Ӧ��Ϊthen", words[nowNum].column, words[nowNum].row);
			}
		}
		int current = code.len();//Ϊ��֮�����
		code.emit("JPC", 0, 0);//��������else����if�ĺ���
		nextWord();
		statement();
		code.code[current].a = code.len();//����if���棬û��else�Ͳ��ü�һ
		if (words[nowNum].name == "else") {
			//һ���Ļ��Ϊ��ifΪ��ʱ�������������statement
			code.code[current].a = code.len() + 1;//��JPC����else��ͷ,��Ϊ������������JMP���룬����Ҫ��1��
			current = code.len();
			code.emit("JMP", 0, 0);//���ʵ������ifΪ��ִ�й���Ϊ�˲�ִ��statement��
			nextWord();
			statement();
			code.code[current].a = code.len();
		}

	}
	void statement_while() {
		//����code��ʼ���,����ѭ��
		int whileStart = code.len();
		if (words[nowNum].name != "while") {
			if (first_set["<lexp>"].count(words[nowNum].name)) {
				--nowNum;
				printError("ȱ��while", words[nowNum].column, words[nowNum].row);
			}
			else if (first_set["<lexp>"].count(words[nowNum + 1].name)) {
				printError("�д���Ĺؼ���" + words[nowNum].name + "��Ӧ��Ϊwhile", words[nowNum].column, words[nowNum].row);
			}
		}
		//������lexp����һ��ֵ����ջ��

		nextWord();
		lexp();
		if (words[nowNum].name != "do") {
			if (first_set["<statement>"].count(words[nowNum].name)) {
				--nowNum;
				printError("ȱ�ٹؼ���do", words[nowNum].column, words[nowNum].row);
			}
			else if (first_set["<statement>"].count(words[nowNum + 1].name)) {
				printError("�д���Ĺؼ���" + words[nowNum].name + "��Ӧ��Ϊdo", words[nowNum].column, words[nowNum].row);
			}
		}
		int current = code.len();
		code.emit("JPC", 0, 0);
		nextWord();
		statement();
		code.emit("JMP", 0, whileStart);//�ص���������
		code.code[current].a = code.len();//����ѭ��
	}
	void statement_call() {
		int l, index;
		int callNum = 0;
		if (words[nowNum].name != "call") {
			if (words[nowNum].type == "id" || words[nowNum].type == "nid") {
				--nowNum;
				printError("ȱ�ٹؼ���call", words[nowNum].column, words[nowNum].row);
			}
			else if (words[nowNum + 1].type == "id" || words[nowNum + 1].type == "nid") {
				printError("�д���Ĺؼ���" + words[nowNum].name + "��Ӧ��Ϊcall", words[nowNum].column, words[nowNum].row);
			}
		}
		nextWord();
		if (words[nowNum].type != "id" && words[nowNum].type != "nid") {
			if (words[nowNum].name == "(") {
				--nowNum;
				printError("ȱ��id", words[nowNum].column, words[nowNum].row);
			}
			else if (words[nowNum + 1].name == "(") {
				printError("�д����id" + words[nowNum].name + "����ʹ���˱�������Ϊid��", words[nowNum].column, words[nowNum].row);
			}
		}
		else {
			l = table.find2(words[nowNum].name)[0], index = table.find(words[nowNum].name)[1];
			if (index == -1) {
				printError2(words[nowNum].name + "û�ж���", words[nowNum].column, words[nowNum].row);
			}
			else if (table.table[l][index].type != "PROCEDURE") {
				printError2(words[nowNum].name + "���Ǻ���", words[nowNum].column, words[nowNum].row);
			}
		}
		nextWord();
		if (words[nowNum].name != "(") {
			if (first_set["<exp>"].count(words[nowNum].name)) {
				printError("ȱ��(", words[nowNum].column, words[nowNum].row);
				--nowNum;
			}
			else if (first_set["<exp>"].count(words[nowNum + 1].name)) {
				printError("����ķ���" + words[nowNum].name + "Ӧ��Ϊ(", words[nowNum].column, words[nowNum].row);
			}
		}
		nextWord();
		int parameter = 3;//�ӵ�������ʼ������ʽ����
		//exp���ջ���һ��ֵ��ջ����
		if (first_set["<exp>"].count(words[nowNum].name) || first_set["<exp>"].count(words[nowNum].type)) {
			exp();
			callNum += 1;
			code.emit("STO", -1, parameter++);//-1������
		}
		while (first_set["<exp>"].count(words[nowNum].name)) {
			printError("ȱ�ٶ���", words[nowNum].column, words[nowNum].row);
			exp();
		}
		while (words[nowNum].name == ",") {
			nextWord();
			exp();
			callNum += 1;
			code.emit("STO", -1, parameter++);//-1������
			while (first_set["<exp>"].count(words[nowNum].name) || first_set["<exp>"].count(words[nowNum].type)) {
				printError("ȱ�ٶ���", words[nowNum].column, words[nowNum].row);
				nextWord();
			}
		}
		if (words[nowNum].name != ")") {
			if (follow_set["<statement>"].count(words[nowNum].name)) {
				printError("ȱ��)", words[nowNum].column, words[nowNum].row);
				--nowNum;
			}
			else if (follow_set["<statement>"].count(words[nowNum + 1].name)) {
				printError("����ķ���" + words[nowNum].name + "Ӧ��Ϊ)", words[nowNum].column, words[nowNum].row);
			}
		}
		//ȷʵ�ǹ���
		if (table.table[l][index].type == "PROCEDURE") {
			if (callNum != table.table[l][index].val) {
				char t[10];
				sprintf(t, "%d", table.table[l][index].val);
				printError2(table.table[l][index].name + "������ĿӦ��Ϊ"+t, words[nowNum].column, words[nowNum].row);
			}
			code.emit("CAL", level - table.table[l][index].level, table.table[l][index].addr);
		}
		else {
			wrong2 = true;
			printError2(words[nowNum].name + "���ǹ�����", words[nowNum].column, words[nowNum].row);
		}

		nextWord();
	}
	void statement_read() {
		int l, index;
		if (words[nowNum].name != "read") {
			if (words[nowNum].name == "(") {
				--nowNum;
				printError("ȱ�ٹؼ���read", words[nowNum].column, words[nowNum].row);
			}
			else if (words[nowNum + 1].name == "(") {
				printError("�д���Ĺؼ���" + words[nowNum].name + "��Ӧ��Ϊread", words[nowNum].column, words[nowNum].row);
			}
		}
		nextWord();
		if (words[nowNum].name != "(") {
			if (words[nowNum].type == "id" || words[nowNum].type == "uid") {
				--nowNum;
				printError("ȱ��(", words[nowNum].column, words[nowNum].row);
			}
			else if (words[nowNum + 1].type == "id" || words[nowNum + 1].type == "uid") {
				printError("�д���ĵ���" + words[nowNum].name + "��Ӧ��Ϊ(", words[nowNum].column, words[nowNum].row);
			}
		}
		nextWord();
		if (words[nowNum].type != "id" && words[nowNum].type != "nid") {
			if (words[nowNum].name == "," || words[nowNum].name == ")") {
				--nowNum;
				printError("ȱ��id", words[nowNum].column, words[nowNum].row);
			}
			else if (words[nowNum + 1].name == "," || words[nowNum + 1].name == ")") {
				printError("�д����id" + words[nowNum].name + "����ʹ���˱�������Ϊid��", words[nowNum].column, words[nowNum].row);
			}
			else {
				printError("read�зǷ��ĵ���", words[nowNum].column, words[nowNum].row);
			}
		}
		else {
			//��id�Ļ�׼������
			l = table.find(words[nowNum].name)[0], index = table.find(words[nowNum].name)[1];
			if (index == -1) {
				printError2(words[nowNum].name + "û�ж���", words[nowNum].column, words[nowNum].row);
			}
			else if (table.table[l][index].type != "VARIABLE") {
				printError2(words[nowNum].name + "���Ǳ���", words[nowNum].column, words[nowNum].row);
			}
			else {
				//ȷʵ�ҵ��˶���
				//cout<<words[nowNum].name<<" "<<l<<" "<<table.table[l][index].addr<<endl;
				code.emit("RED", 0, 0);//��һ�����ݶ���ջ��
				code.emit("STO", level - table.table[l][index].level, table.table[l][index].addr);//��ջ�����ݴ��
			}
		}
		nextWord();
		while (words[nowNum].type == "nid" || words[nowNum].type == "id") {
			//if (words[nowNum].type == "id") printError("id" + words[nowNum].name + "�ظ�����", words[nowNum].column, words[nowNum].row);
			printError("ȱ�ٶ���", words[nowNum].column, words[nowNum].row);
			nextWord();
		}
		while (words[nowNum].name == ",")
		{
			nextWord();
			if (words[nowNum].name == ")") break;
			if (words[nowNum].type != "id" && words[nowNum].type != "nid") {
				if (words[nowNum].name == "," || words[nowNum].name == ")") {
					--nowNum;
					printError("ȱ��id", words[nowNum].column, words[nowNum].row);
				}
				else if (words[nowNum + 1].name == ",") {
					printError("�д����id" + words[nowNum].name + "����ʹ���˱�������Ϊid��", words[nowNum].column, words[nowNum].row);
				}
				else {
					printError("read�зǷ��ĵ���", words[nowNum].column, words[nowNum].row);
				}
			}
			else {
				//��id�Ļ�׼������
				l = table.find(words[nowNum].name)[0], index = table.find(words[nowNum].name)[1];
				if (index == -1) {
					printError2(words[nowNum].name + "û�ж���", words[nowNum].column, words[nowNum].row);
				}
				else if (table.table[l][index].type != "VARIABLE") {
					printError2(words[nowNum].name + "���Ǳ���", words[nowNum].column, words[nowNum].row);
				}
				else {
					//ȷʵ�ҵ��˶���
				//	cout<<words[nowNum].name<<" "<<l<<" "<<table.table[l][level].addr<<endl;
					code.emit("RED", 0, 0);//��һ�����ݶ���ջ��
					code.emit("STO", level - table.table[l][index].level, table.table[l][index].addr);//��ջ�����ݴ��
				}
			}
			nextWord();//�ƺ��е�ֲڣ���һ�м�ֻ����һ����Ҳ��ֱ�ӿ�ʼͬ��,���Ž��һ��
			while (words[nowNum].type == "nid" || words[nowNum].type == "id") {
				if (words[nowNum].type == "id") printError("id" + words[nowNum].name + "�ظ�����", words[nowNum].column, words[nowNum].row);
				printError("ȱ�ٶ���", words[nowNum].column, words[nowNum].row);
				nextWord();
			}
		}
		if (words[nowNum].name != ")") {
			if (follow_set["<statement>"].count(words[nowNum].name)) {
				printError("ȱ��)", words[nowNum].column, words[nowNum].row);
				--nowNum;
			}
			else if (follow_set["<statement>"].count(words[nowNum + 1].name)) {
				printError("����ķ���" + words[nowNum].name + "Ӧ��Ϊ)", words[nowNum].column, words[nowNum].row);
			}
			else {
				printError("����ķ���" + words[nowNum].name + "Ӧ��Ϊ)", words[nowNum].column, words[nowNum].row);
			}
		}
		nextWord();
		while (!(follow_set["<statement>"].count(words[nowNum].name) || follow_set["<statement>"].count(words[nowNum].type))) {
			printError("����ͬ��<statement>,���� " + words[nowNum].name, words[nowNum].column, words[nowNum].row);
			nextWord();
		}
		nextWord();
	}
	void statement_write() {
		int l, index;
		if (words[nowNum].name != "write") {
			if (words[nowNum].name == "(") {
				--nowNum;
				printError("ȱ�ٹؼ���write", words[nowNum].column, words[nowNum].row);
			}
			else if (words[nowNum + 1].name == "(") {
				printError("�д���Ĺؼ���" + words[nowNum].name + "��Ӧ��Ϊwrite", words[nowNum].column, words[nowNum].row);
			}
		}
		nextWord();
		if (words[nowNum].name != "(") {
			if (first_set["<exp>"].count(words[nowNum].name)) {
				--nowNum;
				printError("ȱ��(", words[nowNum].column, words[nowNum].row);
			}
			else if (first_set["<exp>"].count(words[nowNum + 1].name)) {
				printError("�д���ĵ���" + words[nowNum].name + "��Ӧ��Ϊ(", words[nowNum].column, words[nowNum].row);
			}
		}
		nextWord();
		exp();
		//exp�Ժ����һ��������ջ���ģ�Ҫ����ľ������
		code.emit("WRT", 0, 0);
		while (first_set["<exp>"].count(words[nowNum].name)) {
			printError("ȱ�ٶ���", words[nowNum].column, words[nowNum].row);
			exp();
		}
		while (words[nowNum].name == ",") {
			nextWord();
			exp();
			//TEST;
			//exp�Ժ����һ��������ջ���ģ�Ҫ����ľ������
			code.emit("WRT", 0, 0);
			while (first_set["<exp>"].count(words[nowNum].name)) {
				printError("ȱ�ٶ���", words[nowNum].column, words[nowNum].row);
				nextWord();
			}
		}
		if (words[nowNum].name != ")") {
			if (follow_set["<statement>"].count(words[nowNum].name)) {
				printError("ȱ��)", words[nowNum].column, words[nowNum].row);
				--nowNum;
			}
			else if (follow_set["<statement>"].count(words[nowNum + 1].name)) {
				printError("����ķ���" + words[nowNum].name + "Ӧ��Ϊ)", words[nowNum].column, words[nowNum].row);
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
		term();//Ӧ��ϸ���ж�
		if (flag) {
			//ȡ��
			code.emit("OPR", 0, 1);
		}
		while (words[nowNum].name == "+" || words[nowNum].name == "-") {
			int tmp = nowNum;
			nextWord();
			term();
			if (words[tmp].name == "+") {
				code.emit("OPR", 0, 2);//���
			}
			else {
				code.emit("OPR", 0, 3);//���
			}

			while (first_set["<term>"].count(words[nowNum].name)) {
				printError("ȱ��<aop>", words[nowNum].column, words[nowNum].row);
				term();
			}
		}
		//		while(!follow_set["<exp>"].count(words[nowNum].name)){
		//			printError("����ͬ��<exp>,���� "+words[nowNum].name,words[nowNum].column,words[nowNum].row);
		//			nextWord();
		//		}

	}
	void lexp() {
		if (words[nowNum].name == "odd") {
			nextWord();
			//exp���һ��Ҫ�жϵķŵ�ջ����
			exp();
			code.emit("OPR", 0, 6);//�ж���ż
		}
		else if (first_set["<exp>"].count(words[nowNum + 1].name) || first_set["<exp>"].count(words[nowNum + 1].type)) {
			printError("����ؼ���" + words[nowNum].name + "��Ӧ��Ϊodd", words[nowNum].column, words[nowNum].row);
			nextWord();
			exp();
		}
		else {
			if (!(first_set["<exp>"].count(words[nowNum + 1].name) || first_set["<exp>"].count(words[nowNum + 1].type)))
			{
				if (words[nowNum].name == "=" || words[nowNum].name == "<>" || words[nowNum].name == "<" || words[nowNum].name == "<=" || words[nowNum].name == ">" || words[nowNum].name == ">=") {
					printError("ȱ��<exp>����", words[nowNum].column, words[nowNum].row);
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
					printError("ȱ��<lop>����", words[nowNum].column, words[nowNum].row);
				}
				else if (first_set["<exp>"].count(words[nowNum + 1].name) || first_set["<exp>"].count(words[nowNum + 1].type)) {
					printError("�д���ĵ���" + words[nowNum].name + "��Ӧ��Ϊ<lop>����", words[nowNum].column, words[nowNum].row);
				}
				else {
					printError("�д���ĵ���" + words[nowNum].name + "��Ӧ��Ϊ<lop>����", words[nowNum].column, words[nowNum].row);
				}
				//��Ȼ�����˵����ǵü����﷨������
				nextWord();
				exp();
			}
			else {
				string sign = words[nowNum].name;//�ض��ǱȽϷ���֮һ����Ϊ��else��
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
			printError("����ͬ��<lexp>,���� " + words[nowNum].name, words[nowNum].column, words[nowNum].row);
			nextWord();
		}
	}
	void term() {
		factor();
		//		while(first_set["<factor>"].count(words[nowNum].name)||first_set["<factor>"].count(words[nowNum].type)){
		//			printError("ȱ��<mop>",words[nowNum].column,words[nowNum].row);
		//			factor();
		//		}
		while (words[nowNum].name == "*" || words[nowNum].name == "/") {
			int tmp = nowNum;
			nextWord();
			factor();
			if (words[tmp].name == "*") {
				code.emit("OPR", 0, 4);//���
			}
			else {
				code.emit("OPR", 0, 5);//���
			}
		}

	}
	void factor() {
		int l, index;
		if (words[nowNum].type == "nid" || words[nowNum].type == "id" || words[nowNum].type == "integer") {
			l = table.find(words[nowNum].name)[0], index = table.find(words[nowNum].name)[1];
			if (index == -1) {
				if (words[nowNum].type == "integer") {
					//��Ȼû���ҵ�������������һ������
					code.emit("LIT", 0, stoi(words[nowNum].name));
				}
				else
				{
					printError2(words[nowNum].name + "û�ж���", words[nowNum].column, words[nowNum].row);
				}

			}
			else {
				if (table.table[l][index].type == "CONSTANT")//����
				{
					code.emit("LIT", 0, table.table[l][index].val);
				}
				else if (table.table[l][index].type == "VARIABLE")//����
				{
					code.emit("LOD", level - table.table[l][index].level, table.table[l][index].addr);
				}
				else {
					printError2(words[nowNum].name + "Ϊ������", words[nowNum].column, words[nowNum].row);
				}
			}
			nextWord();
			return;
		}
		else if (words[nowNum].name == "(") {
			//�����Ž�����ʽ
			nextWord();
			exp();
			if (words[nowNum].name != ")") {
				if (follow_set["<factor>"].count(words[nowNum].name)) {
					printError("ȱ��)", words[nowNum].column, words[nowNum].row);
					--nowNum;
				}
				else if (follow_set["<factor>"].count(words[nowNum + 1].name)) {
					printError("����ķ���" + words[nowNum].name + "Ӧ��Ϊ)", words[nowNum].column, words[nowNum].row);
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
		vector<string> f = { "�������.txt","sum.txt","sum2-����.txt","fibnacci.txt","fibnacciRead.txt" ,"add_sum_fib.txt","������.txt" };
		GrammarAnalyse a(f[6]);
		a.analyse();
		code.write2File();
		a.close();
		return 0;
	}
	else {
		cout << "�����д���:" << argv[1] << endl;
		GrammarAnalyse a(argv[1]);
		a.analyse();
		code.write2File();
		a.close();
		return 0;
	}

}