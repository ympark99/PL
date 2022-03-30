#include <iostream>
#include <vector>
#include <algorithm>
#include <math.h>
#include <time.h>
#include <cstdio>

using namespace std;

// gcd 구하기
int gcd(int a, int b){
    int mod;
    while (b != 0){
        mod = a % b;
        a = b;
        b = mod;
    }
    return a;
}

// 소수 개수 구하기
int findAns(int start, int end, vector<bool> check){
    int ans = 0;
    // 소수인경우 개수 1 증가
    for(int i = start; i <= end; i++){
        if(!check[i]) ans++;
    }

    return ans;
}

int main(){
    clock_t start, end;
    double runtime = 0;
    double N;

    start = clock(); // 시간 측정

    while(1){
        double integer, fraction; // 정수부, 소수부
        cout << ">> Input the number of numbers to process: ";
        cin >> N;
        if(cin.fail()){ // 입력 오류시 버퍼 비우고 재입력
            cin.clear();
            cin.ignore(1024, '\n');
        }
        else{
            // 1개보다 더 많이 입력한 경우
            if(getchar() == ' '){
                cin.clear();
                cin.ignore(1024, '\n');
            }        
            fraction = modf(N, &integer); // 정수, 소수 부분 구하기
            if((N >= 2 && N <= 30) && fraction == 0){
                break; // 2와 30 사이, 자연수이면 패스
            }
        }
    }
    
    vector<int> v; // 입력한 자연수 저장할 vector

    // 입력한 순서대로 vector에 넣어줌(중복인 경우 제거)
    while(1){
        vector<int> input;
        double num = 0;
        double integer, fraction;
        bool goNext = true;
        cout << ">> Input the numbers to be processed: \n";

        for(int i = 0; i < N; i++){
            cin >> num;
            if(cin.fail()){ // 입력 오류시 버퍼 비우고 재입력
                cin.clear();
                cin.ignore(1024, '\n');
                goNext = false;
                break;
            }            
            fraction = modf(num, &integer); // 정수, 소수 부분 구하기
            if(fraction != 0 || (num > 100000 || num < 1)) // 실수이거나 입력 오류 시 재입력
                goNext = false;
            
            bool addNum = true;
            for(int i = 0; i < v.size(); i++){
                if(num == v[i]){ // 겹칠경우 추가 x
                    addNum = false;
                    break;
                }
            }
            if(addNum) v.push_back((int)num);
        }
        // N개보다 더 많이 입력한 경우
        if(getchar() == ' '){
            cin.clear();
            cin.ignore(1024, '\n');
            goNext = false;
        }        
        if(goNext) break;
        v.clear();
    }

    // 최대공약수 구하기
    int gcdNum = v[0];
    for(int i = 1; i < v.size(); i++){
        gcdNum = gcd(gcdNum, v[i]);
    }
    cout << "GCD of input numbers is " << gcdNum << '\n';

    sort(v.begin(), v.end()); // 크기 순서대로 정렬

    vector<bool> check(v.back() + 1, false); // 에라토스테네스의 채 사용하기 위한 vector
    check.at(0) = check.at(1) = true;

    // 2부터 가장 큰 수의 제곱근까지 검사
    for(int i = 2; i <= sqrt(v.back()); i++){
        // 소수라면
        if(!check[i]){
            // 소수의 배수는 소수가 아니므로 check
            for(int j = i + i; j <= v.back(); j += i)
                check[j] = true;
        }
    }

    for(int i = 0; i < v.size() - 1; i++){
        cout << "Number of prime numbers between " << v[i] << ", " << v[i+1] << ": " << findAns(v[i], v[i+1], check) << '\n';
    }

    end = clock();
    runtime = (double)(end - start) / CLOCKS_PER_SEC; // 실행시간 구하기
    cout << "Total execution time using C++ is " << runtime << " seconds!\n";

    return 0;
}