package com.company;

import java.util.*;

// 소수 개수 구하는 클래스
class Prime{
    // 필드
    int start, end;
    ArrayList<Boolean> check;

    // 생성자
    public Prime(int start, int end, ArrayList<Boolean> check) {
        this.start = start;
        this.end = end;
        this.check = check;
    }

    // 소수 개수 구하기
    public int getPrime(){
        int ans = 0;
        // 소수인 경우 개수 1 증가
        for(int i = start; i <= end; i++)
            if(!check.get(i)) ans++;

        return ans;
    }
}

// GCD 구하는 클래스
class Gcd{
    int a, b; // 필드

    // 생성자
    public Gcd(int a, int b){
        this.a = a;
        this.b = b;
    }
    // gcd 구하기
    public int getGcd(){
        int mod = 0;
        while (b != 0){
            mod = a % b;
            a = b;
            b = mod;
        }
        return a;
    }
}

public class Main {
    public static void main(String[] args) {
        double startTime = System.currentTimeMillis(); // 시작시간
        Scanner scan = new Scanner(System.in); // 스캐너 생성
        double N = 0;

        while(true){
            double integer, fraction;
            System.out.print("Input the number of numbers to process: ");
            try {
                N = scan.nextDouble(); // N 입력

                // 1개보다 더 많이 입력했는지 검사
                char ch = scan.nextLine().charAt(0);
                if(ch == ' '){
                    scan = new Scanner(System.in); // 스캐너 초기화
                }
            }
            catch (InputMismatchException e){ // 잘못된 값 입력시
                scan = new Scanner(System.in); // 스캐너 초기화
            }
            catch (StringIndexOutOfBoundsException e){ // 정상입력한 경우
                integer = Math.floor(N);
                fraction = N - integer;
                if((N >= 2 && N <= 30) && fraction == 0){
                    break;
                }
            }
        }

        ArrayList<Integer> al = new ArrayList<>((int)N); // 입력 값 보관할 arraylist 선언

        // 입력한 순서대로 arrayList에 넣어줌(중복인 경우 제거)
        while(true) {
            System.out.println(">> Input the numbers to be processed: ");
            boolean goNext = true;
            double integer, fraction;
            for (int i = 0; i < N; i++) {
                double num = 0;
                try {
                    num = scan.nextDouble();
                }
                catch (InputMismatchException e){ // 잘못된 값 입력시
                    scan = new Scanner(System.in); // 스캐너 초기화
                }
                integer = Math.floor(num);
                fraction = num - integer;
                if(fraction !=0 || (num > 100000 || num < 1)) // 입력 오류 시 재입력
                    goNext = false;

                if(!al.contains(num)) al.add((int)num); // 중복이 아니면 리스트에 넣어줌
            }
            try{
                // N개보다 더 많이 입력했는지 검사
                char ch = scan.nextLine().charAt(0);
                if(ch == ' '){
                    scan = new Scanner(System.in); // 스캐너 초기화
                    goNext = false;
                }
            }
            catch (StringIndexOutOfBoundsException e){
                goNext = true;
            }
            if(goNext) break;
            al.clear();
        }

        // 최대공약수 구하기
        int gcdNum = al.get(0);
        for(int i = 1; i < al.size(); i++){
            Gcd gcd = new Gcd(gcdNum, al.get(i));
            gcdNum = gcd.getGcd();
        }
        System.out.println("GCD of input numbers is " + gcdNum);

        Collections.sort(al); // 크기 순서대로 정렬

        int lastSize = al.get(al.size() - 1); // 마지막 인덱스 사이즈
        // 에라토스테네스의 채 사용하기 위한 Arraylist
        ArrayList<Boolean> check = new ArrayList<>(Arrays.asList(new Boolean[lastSize + 1]));
        Collections.fill(check, Boolean.FALSE); // false로 초기화
        check.set(0, true);
        check.set(1, true);

        // 2부터 가장 큰 수의 제곱근까지 검사
        for(int i = 2; i <= Math.sqrt(lastSize); i++){
            // 소수일경우
            if(!check.get(i)){
                // 소수의 배수는 소수가 아니므로 check
                for(int j = i + i; j <= lastSize; j += i)
                    check.set(j, true);
            }
        }

        for(int i = 0; i < al.size() - 1; i++){
            Prime prime = new Prime(al.get(i), al.get(i+1), check);
            System.out.println("Number of prime numbers between " + al.get(i) + ", " + al.get(i+1) + ": " + prime.getPrime());
        }

        // 실행시간 구하기
        double endTime = System.currentTimeMillis(); // 종료시간
        double runTime = (double)(endTime - startTime) / 1000;
        System.out.println("Total execution time using Java is " + runTime + " seconds!");
    }
}
