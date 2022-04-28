import java.util.Scanner;
import java.util.Arrays;
import java.util.stream.Collectors;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import java.util.ArrayList;
import java.util.Comparator;
import java.util.List;
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
            final String current = text;
            TokenMatch match = Arrays.stream(TokenType.values())
                    .filter(t -> t != TokenType.EOF)
                    .flatMap(t -> mapTokenMatches(t, t.match(current)))
                    .filter(TokenMatch::isFromStart)
                    .sorted(Comparator.comparingInt(TokenMatch::length).reversed())
                    .findFirst()
                    .orElseThrow(() -> new IllegalArgumentException("cannot find token for " + current));
            String content = text.substring(0, match.length());
            text = text.substring(match.length());
            tokens.add(new Token(match.type(), content));
        }
    }

    private Stream<TokenMatch> mapTokenMatches(TokenType type, Matcher matcher) {
        List<TokenMatch> matches = new ArrayList<>();
        while (matcher.find()) {
            matches.add(new TokenMatch(type, matcher.start(), matcher.end()));
        }
        return matches.stream();
    }

    private class TokenMatch {

        private final TokenType type;
        private final int from, to;

        TokenMatch(TokenType type, int from, int to) {
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
    SLASH("/"),
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

    boolean matches(String input) {
        return pattern.matcher(input).matches();
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

    double value() {
        if (type != TokenType.NUMBER) {
            throw new IllegalArgumentException("cannot get value of " + type.name());
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
        double value = parseTerm();
        while (any(TokenType.PLUS, TokenType.MINUS)) {
            TokenType op = token.getType();
            getNextToken();
            value = op == TokenType.PLUS ? value + parseTerm() : value - parseTerm();
        }
        return value;
    }

    private double parseTerm() {
        double value = factor();
        while (any(TokenType.MULTI, TokenType.SLASH)) {
            TokenType op = token.getType();
            getNextToken();
            double right = factor();
            if (op == TokenType.MULTI) {
                value = value * right;
            } else if (right != 0){
                value = value / right;
            } else {
                throw new IllegalArgumentException("cannot divide by 0");
            }
        }
        return value;
    }

    private double factor() {
        double value;
        if (match(TokenType.OPEN_PARAM)) {
            getNextToken();
            value = expr();
            assertMatch(TokenType.CLOSE_PARAM);
            getNextToken();
            return value;
        }
        // todo [-]
        assertMatch(TokenType.NUMBER);
        value = token.value();
        getNextToken();
        return value;
    }

    private boolean match(TokenType type) {
        return this.token.getType() == type;
    }

    private boolean any(TokenType... types) {
        return Arrays.stream(types).anyMatch(t -> t == token.getType());
    }

    private void assertMatch(TokenType type) {
        if (!match(type))
            throw new IllegalStateException("expected " + type.name() + " got " + this.token.getType());
    }

    private void assertAny(TokenType... types) {
        if (!any(types)) {
            throw new IllegalStateException("expected any of " + Arrays.stream(types).map(TokenType::name).collect(Collectors.joining(", ")) + " got " + this.token.getType());
        }
    }

    private void getNextToken() {
        this.token = tokenizer.next();
    }
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
                // 정수인 경우 소수점 출력 x
                if(value == (int)value) System.out.println((int)value);
                else System.out.println(value);
            } catch (Exception e) {
                System.out.println(e.getMessage());
                System.out.println("Syntax error!!");
                System.exit(1);
            }
            System.out.print(">> ");
        }
    }
}
