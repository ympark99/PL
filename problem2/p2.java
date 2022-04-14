import jdk.jfr.Unsigned;

import java.io.InputStream;
import java.util.*;

interface Stack{
    java.util.Stack<Character> st = new java.util.Stack<>();
}

class Peek{
    InputStream inputStream;
    public Peek(InputStream inputStream){
        this.inputStream = inputStream;
    }
    public char getPeek(){
        char ch = inputStream.read();
        if(inputStream.good())
            return ch;
        return 0;
    }
}

class Lookahead{
    InputStream inputStream;
    public Lookahead(InputStream inputStream){
        this.inputStream = inputStream;
    }
    public char getLookAhead(){
        Peek peek = new Peek(inputStream);
        inputStream >> ws;
        return peek.getPeek();
    }
}

class Number{
    InputStream inputStream;
    public Number(InputStream inputStream){
        this.inputStream = inputStream;
    }
    public int getNumber(){
        Lookahead lookahead = new Lookahead(inputStream);
        Peek peek = new Peek(inputStream);
        char ch = lookahead.getLookAhead();
        if(!isDigit(ch)){
            System.out.print("Syntax error!!");
            System.exit(1);
        }
        int num = 0;
        while (isDigit(ch) && inputStream >> ch){
            num = num * 10 + ch - '0';
            ch = peek.getPeek();
        }
        return num;
    }
    public long unsigned32(int n){
        return n & 0xFFFFFFFFL;
    }
    boolean isDigit(char ch){
        return ch >= '0' && ch <= '9';
    }
}

class Consume{
    InputStream inputStream;
    public Consume(InputStream inputStream){
        this.inputStream = inputStream;
    }
    public boolean getConsume(char expected){
        Lookahead lookahead = new Lookahead(inputStream);
        if(expected <= 0){
            System.out.print("Syntax error!!");
            System.exit(1);
        }
        if(lookahead.getLookAhead() == expected){
//            char ch = (char)inputStream.input();
            char ch = 'a';
            // stack push & pop
            if(ch == '(') Stack.st.push('(');
            else if(ch == ')'){
                if(Stack.st.empty()){
                    System.out.print("Syntax error!!");
                    System.exit(1);
                }
                Stack.st.pop();
            }

            if(!inputStream.good()){
                System.out.print("Syntax error!!");
                System.exit(1);
            }
            return true;
        }
        return false;
    }
}

class Factor{
    InputStream inputStream;
    public Factor(InputStream inputStream){
        this.inputStream = inputStream;
    }
    public double getFactor(){
        Expr expr = new Expr(inputStream);
        Consume consume = new Consume(inputStream);
        Factor factor = new Factor(inputStream);
        Number number = new Number(inputStream);
        double value;
        if(consume.getConsume('(')){
            value = expr.getExpr();
            consume.getConsume(')');
        }
        else if(consume.getConsume('-')){
            value = - factor.getFactor();
        }
        else value = number.getNumber();
        return value;
    }
}

class Term{
    InputStream inputStream;
    public Term(InputStream inputStream){
        this.inputStream = inputStream;
    }
    public double getTerm(){
        Factor factor = new Factor(inputStream);
        Consume consume = new Consume(inputStream);
        double value = factor.getFactor();
        while(true){
            if(consume.getConsume('*'))
                value *= factor.getFactor();
            else if(consume.getConsume('/'))
                value /= factor.getFactor();
            else
                return value;
        }
    }
}

class Expr{
    InputStream inputStream;
    public Expr(InputStream inputStream){
        this.inputStream = inputStream;
    }
    public double getExpr(){
        Term term = new Term(inputStream);
        Consume consume = new Consume(inputStream);
        double value = term.getTerm();
        // 중괄호이므로 while
        while(true){
            if(consume.getConsume('+'))
                value += term.getTerm();
            else if(consume.getConsume('-'))
                value -= term.getTerm();
            else if(!consume.getConsume(')')) // )가 더 많은경우 검사
                return value;
        }
    }
}

public class p2 {
    public static void main(String[] args) {
        Scanner scan = new Scanner(System.in);
        while(true){
            System.out.print(">> ");
            String str = scan.nextLine(); // 수식 받을 문자열
            str = str.replaceAll(" ", ""); // 공백 제거
//            String[] strings = str.split(" ");
//
//            List<Integer> ints = Stream.of(strings).
//                    map(value -> Integer.valueOf(value)).
//                    collect(Collectors.toList());
        }
    }
}
