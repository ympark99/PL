import time
start = time.time()

# gcd 구하기
def gcd(a, b) :
    while b != 0 :
        mod = a % b
        a = b
        b = mod
    return a

# 소수 개수 구하기
def findAns(start, end, check) :
    ans = 0
    for i in range(start, end + 1) :
        if check[i] == False : ans += 1
    return ans

while True :
    N = input(">> Input the number of numbers to process: ")
    # 2와 30 사이 자연수어야 패스
    if N.isdigit() :
        N = int(N)
        if 2 <= N and N <= 30 : break

while True :
    go_next = True
    n_list = list(input(">> Input the numbers to be processed: \n").split())
    
    # 자연수여야 패스
    if not ''.join(map(str,n_list)).isdigit() :
        continue
    else :
        n_list = list(map(int, n_list)) # 정수로 변환

    # N개 입력 아니면 false
    if N != len(n_list) : continue;
    
    # 입력 값 벗어날 경우 false
    for i in range(len(n_list)) :
        if n_list[i] > 100000 or n_list[i] < 1 :
            go_next = False
            break
    
    n_list = list(set(n_list)) # 중복 제거    
    if go_next : break

# 최대공약수 구하기
gcdNum = n_list[0]
for i in range(len(n_list)) : 
    gcdNum = gcd(gcdNum, n_list[i])

print("GCD of input numbers is {}".format(gcdNum))

n_list.sort() # 크기 순서대로 정렬

check = list(False for i in range(n_list[-1] + 1)) # 에라토스테네스 채 사용하기 위한 리스트
check[0] = True
check[1] = True

# 2부터 가장 큰 수의 제곱근까지 검사(파이썬 for문은 마지막 수 포함안하므로 +1)
for i in range(2, int(n_list[-1]**(1/2)) + 1) :
    if check[i] == False :
        j = i + i
        while j <= n_list[-1] : # 소수의 배수 check
            check[j] = True
            j += i

for i in range(0, len(n_list) - 1) :
    print("Number of prime numbers between {}, {}: {}".format(n_list[i], n_list[i+1], findAns(n_list[i], n_list[i+1], check)))
    
print("Total execution time using Python is", time.time() - start, "seconds!")