## Programming Language 과제 2

### 다음의 EBNF로 문법이 정의되는 수식을 위한 계산기를(Recursive-Descent Parser 구현 포함) C/C++, Java, Python으로 각각 구현하시오.

### expr → term {+ term | - term }
### term → factor {* factor | / factor } 
### factor → [ - ] ( number | (expr) ) 
### number → digit {digit} 
### digit → 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9
  
  
▸ 입력
  - 프로그램을 실행하면 초기화면에서 사용자는 수식을 입력함. 엔터키가 입력되면 하나의 수식이 입력된 것으로 처리함
  
▸ 출력
- 문법에 맞는 수식이 입력된 경우에는 수식 결과를 계산하여 출력하고, 다음 수식을 입력 받음
- 문법에 맞지 않는 수식이 입력된 경우에는 “Syntax error!!”를 출력하고, 프로그램 수행을 종료함
