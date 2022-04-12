#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>
#include <stack>

using namespace std;

unsigned int number(istream& input);
double factor (istream& in_stream);
double term (istream& in_stream);
double expr (istream& in_stream);

stack<char> st; // 괄호 저장 스택

int main(){
    while (1){
        cout << ">> ";
        string str; // 수식 받을 문자열
        getline(cin, str);
        str.erase(remove(str.begin(), str.end(), ' '), str.end());
        stringstream input(str);

        double ans = expr(input);
        if(!st.empty()){ // 괄호 안닫은 경우 에러
            fprintf(stderr, "Syntax error!!");
            exit(1);
        }
        cout << ans << "\n";
    }
}

char peek(istream& input){
    char ch = input.peek(); // 하나 훔쳐보기
    if(input.good())
        return ch;
    return 0;
}

char lookahead(istream& input){
    input >> ws; // skip whitespaces
    return peek(input);
}

bool isDigit(char ch){
    return ch >= '0' && ch <= '9';
}

unsigned int number(istream& input){
    char ch = lookahead(input);
    if(!isDigit(ch)){
        fprintf(stderr, "Syntax error!!->isdig");
        exit(1);
    }
    unsigned int num = 0;
    while (isDigit(ch) && input >> ch){
        num = num * 10 + ch - '0';
        ch = peek(input);
    }
    return num;
}

bool consume(istream& input, char expected) {
    if(expected <= 0){
        fprintf(stderr, "Syntax error!!->expect");
        exit(1);
    }
    if (lookahead(input) == expected) {
        char ch = (char)input.get();
        // stack push & pop
        if(ch == '(') st.push('(');
        else if(ch == ')') st.pop();

        if(!input.good()){
            fprintf(stderr, "Syntax error!!->good");
            exit(1);
        }
        return true;
    }
    return false;
}

double factor(istream& in_stream){
    double value;
    if (consume(in_stream, '(')) {
        value = expr(in_stream);  // (<expr>)
        consume(in_stream, ')');
    }
    // // -
    // else if (consume(in_stream, '-')) {
    //     value = - factor (in_stream);
    // } 
    else {
        value = number(in_stream); // <number>
    }
    return value;
}

double term (istream& in_stream){
    double value = factor(in_stream);
    while(true){
        if (consume(in_stream, '*'))
            value *= factor(in_stream); // * <factor>
        else if (consume(in_stream, '/'))
            value /= factor(in_stream); // /<factor>
        else
            return value;
    }
}

double expr(istream& in_stream){
    double value = term(in_stream);
    // 중괄호이므로 while
    while(true){
        if (consume(in_stream, '+'))
            value += term (in_stream);
        else if (consume(in_stream, '-'))
            value -= term(in_stream);
        else
            return value;
    }
}
