import re

class Calculator:
    def __init__(self):
        self.line: str = ''
        self.current: str = ''

    def parser(self, exp: str) -> float:
        self.line = exp
        result = self.expr()
        if self.line != '':
            raise SyntaxError
        return result

    def expr(self) -> float:
        result = self.term()
        while self.is_next('[-+]'):  # + or -
            if self.current == '+':
                result += self.term()
            else:
                result -= self.term()
        return result

    def term(self) -> float:
        result = self.factor()
        while self.is_next('[*/]'):
            if self.current == '*':
                result *= self.factor()
            else:
                try:
                    result /= self.factor()
                    if int(result) == result :
                        result = int(result)
                except ZeroDivisionError:
                    result = float('NaN')  # 숫자 아닌 경우
        return result

    def factor(self) -> float:
        if self.is_next(r'[0-9]*\.?[0-9]+'):  # number 입력 경우
            return float(self.current) if '.' in self.current else int(self.current)
        if self.is_next('-'):  # [-]
            return -self.factor()
        if self.is_next('[(]'):  # (expr)
            result = self.expr()
            if not self.is_next('[)]'): 
                raise SyntaxError
            return result
        raise SyntaxError

    def is_next(self, regexp: str) -> bool:
        m = re.match(r'\s*' + regexp + r'\s*', self.line)
        if m:
            self.current = m.group().strip()
            self.line = self.line[m.end():]
            return True
        return False

if __name__ == '__main__':
    calc = Calculator()
    while True:
        lineInput = input('>> ')
        line = lineInput.replace(" ", "")
        try:
            print(calc.parser(line))
        except SyntaxError as e:
            print('Syntax error!!')
            break
