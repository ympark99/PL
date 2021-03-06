import java.util.*;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import java.util.stream.Stream;

final class Tokenizer {
    private final static Token EOF_TOK = new Token(TokenType.EOF);
    private List<Token> tokens;
    private final String input;
    private int position;

    Tokenizer(String input) {
        this.input = input;
        this.tokens = new ArrayList<>();
        this.position = 0;
        this.tokenize();
    }

    Token next() {
        return position < tokens.size() ? tokens.get(position++) : EOF_TOK;
    }

    private void tokenize() {
        String text = input.replaceAll("\\s", "");
        while (!text.isEmpty()) {
            final String cur = text;
            MatchToken match = Arrays.stream(TokenType.values())
                    .filter(t -> t != TokenType.EOF)
                    .flatMap(t -> mapMatchesToken(t, t.match(cur)))
                    .filter(MatchToken::isFromStart)
                    .sorted(Comparator.comparingInt(MatchToken::length).reversed())
                    .findFirst()
                    .orElseThrow(() -> new IllegalArgumentException());
            String content = text.substring(0, match.length());
            text = text.substring(match.length());
            tokens.add(new Token(match.type(), content));
        }
    }

    private Stream<MatchToken> mapMatchesToken(TokenType type, Matcher matcher) {
        List<MatchToken> matches = new ArrayList<>();
        while (matcher.find()) {
            matches.add(new MatchToken(type, matcher.start(), matcher.end()));
        }
        return matches.stream();
    }

    private class MatchToken {
        private final TokenType type;
        private final int from, to;

        MatchToken(TokenType type, int from, int to) {
            this.type = type;
            this.from = from;
            this.to = to;
        }

        int length() {
            return to - from;
        }

        boolean isFromStart() {
            return from == 0;
        }

        TokenType type() {
            return type;
        }
    }
}

enum TokenType {
    NUMBER("(\\d+)"),
    PLUS("\\+"),
    MINUS("-"),
    MULTI("\\*"),
    DIVISION("/"),
    OPEN_PARAM("\\("),
    CLOSE_PARAM("\\)"),
    EOF("");

    private final Pattern pattern;

    TokenType(String regexp) {
        this.pattern = Pattern.compile(regexp);
    }
    Matcher match(String input) {
        return pattern.matcher(input);
    }
}


final class Token {
    private final TokenType type;
    private final String content;

    Token(TokenType type) {
        this.type = type;
        this.content = "";
    }

    Token(TokenType type, String content) {
        this.type = type;
        this.content = content;
    }

    TokenType getType() {
        return type;
    }

    double getNumber() {
        if (type != TokenType.NUMBER) {
            throw new IllegalArgumentException();
        }
        return Double.parseDouble(content);
    }
}

final class Parser {
    private Tokenizer tokenizer;
    private Token token;

    Parser(Tokenizer tokenizer) {
        this.tokenizer = tokenizer;
        this.token = tokenizer.next();
    }

    public double expr() {
        double value = term();
        while (isAny(TokenType.PLUS, TokenType.MINUS)) {
            TokenType oper = token.getType();
            getNextToken();
            value = (oper == TokenType.PLUS) ? value + term() : value - term();
        }
        TokenType isClose = token.getType();
        // ?????? ?????? ??????
        if(isClose == TokenType.CLOSE_PARAM) {
            // ????????? ??????????????? pop, ????????? error
            if (paramStack.st.empty()) {
                throw new IllegalArgumentException();
            } else paramStack.st.pop();
        }
        else if(isClose == TokenType.OPEN_PARAM) // (??? ?????? ??????
            throw new IllegalArgumentException();

        return value;
    }

    private double term() {
        double value = factor();
        while (isAny(TokenType.MULTI, TokenType.DIVISION)) { // * or / ??????
            TokenType oper = token.getType();
            getNextToken();
            double right = factor();
            if (oper == TokenType.MULTI) {
                value = value * right;
            } else if (right != 0){
                value = value / right;
            } else { // 0?????????
                throw new ArithmeticException();
            }
        }
        return value;
    }

    private double factor() {
        double value;
        // (<expr>)
        if (match(TokenType.OPEN_PARAM)) {
            paramStack.st.push("("); // ????????? push
            getNextToken();
            value = expr();
            assertMatch(TokenType.CLOSE_PARAM);
            getNextToken();
        }
        // [-]
        else if(match(TokenType.MINUS)){
            getNextToken();
            value = -1 * factor();
        }
        else{
            assertMatch(TokenType.NUMBER);
            value = token.getNumber();
            getNextToken();
        }
        return value;
    }

    private boolean match(TokenType type) {
        return this.token.getType() == type;
    }

    private boolean isAny(TokenType... types) {
        return Arrays.stream(types).anyMatch(t -> t == token.getType());
    }

    private void assertMatch(TokenType type) {
        if (!match(type))
            throw new IllegalStateException();
    }

    private void getNextToken() { // ?????? ???????????? ??????
        this.token = tokenizer.next();
    }
}

// ?????? ?????? ??????
class paramStack{
    public static Stack<String> st = new Stack<>();
}

public class Calculator {
    public static void main(String[] args) {
        Scanner scan = new Scanner(System.in);
        System.out.print(">> ");
        while (scan.hasNextLine()) {
            String input = scan.nextLine();
            try {
                Tokenizer tokenizer = new Tokenizer(input);
                Parser parser = new Parser(tokenizer);
                double value = parser.expr();
                // ?????? ?????? ?????? ???????????? ?????? ??????
                if(!paramStack.st.empty())
                    throw new EmptyStackException();
                // ????????? ?????? ????????? ?????? x
                if(value == (int)value) System.out.println((int)value);
                else System.out.println(value);
            } catch (ArithmeticException e){ // 0?????? ?????? ??????
                System.out.println("NaN");
            }
            catch (Exception e) {
                System.out.println("Syntax error!!");
                System.exit(1);
            }
            System.out.print(">> ");
        }
    }
}
