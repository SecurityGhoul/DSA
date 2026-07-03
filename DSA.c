#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/rand.h>
#include <openssl/sha.h>
#include <math.h>

int isPrime(long long n) 
{
	if (n <= 1)
	{
		return 2;
	}
	if (n == 2) return 1;
		if (n % 2 == 0)
		{
			return 2;
		}
    for (long long i = 3; i * i <= n; i += 2) 
	{
        if (n % i == 0) 
		{
			return 2;
        }
    }
	return 1;
}

void fspace( char buffer[1024])
{
	int i = 0;
	while (i < 1024)
	{
		if (buffer[i] == ' ')
		{
			printf("공백이 입력되었습니다\n");
			exit(0);
		}
		i++;
	}
}

void factorize(long int n, long int res[1024],int count)
{
	for (int i = 2; i <= n; i++)
	{
		if (n == 1)
		{
			break;
		}
		else if (n % i == 0 && isPrime(i) == 1)
		{
			res[count] = i;
			(count)++;
			factorize(n / i, res, count);
			return;
		}
	}
}

long long c_randbelow(long long n) //파이썬의 randbelow와 같은 함수 c에도 구현
{
	if (n <= 0) return 0;

	long long max_usable = (9223372036854775807LL / n) * n;
	long long num;

	while (1) {
		if (RAND_bytes((unsigned char*)&num, sizeof(num)) != 1)
		{
			exit(1);
		}

		if (num < 0) num = -num;

		if (num < max_usable)
		{
			return num % n;
		}
	}
}

long long ModPow(long long base, long long exp, long long mod)
{//무작정 pow에 mod하면 메모리가 터지므로 이진수로 치환하여 천천히 쪼갬
	long long res = 1;
	base = base % mod;

	while (exp > 0)
	{
		if (exp % 2 == 1)
		{
			long long t_res = 0, t_base = base, t_mult = res;
			while (t_mult > 0)
			{
				if (t_mult % 2 == 1) t_res = (t_res + t_base) % mod;
				t_base = (t_base + t_base) % mod;
				t_mult /= 2;
			}
			res = t_res;
		}
		long long t_res = 0, t_base = base, t_mult = base;
		while (t_mult > 0) {
			if (t_mult % 2 == 1) t_res = (t_res + t_base) % mod;
			t_base = (t_base + t_base) % mod;
			t_mult /= 2;
		}
		base = t_res;

		exp /= 2;
	}
	return res;
}

long long HASH(unsigned char* str) //제미나이 생성 openssl 기반 SHA256
{
	unsigned char hash[SHA256_DIGEST_LENGTH];

	// OpenSSL 표준 SHA-256 해시화 실행
	SHA256(str, strlen((char*)str), hash);

	// 해시 바이트 배열 중 앞 8바이트를 추출하여 64비트 정수로 변환
	long long hash_num = 0;
	for (int i = 0; i < 8; i++)
	{
		hash_num = (hash_num << 8) | hash[i];
	}

	if (hash_num < 0) hash_num = -hash_num; // 음수 방지 가드

	return hash_num;
}

long long revmod(long long a, long long b) //모듈러 역원 함수
{
	long long b0 = b;
	long long y = 0, x = 1;

	// 모듈러가 1이면 역원은 존재하지 않음
	if (b == 1) return 0;

	while (a > 1) {
		long long q = a / b;
		long long t = b;

		// 유클리드 호제법(숫자 줄이기)
		b = a % b;
		a = t;
		t = y;

		// 역원 x 값 갱신
		y = x - q * y;
		x = t;
	}

	// x가 음수가 나오면 양수로 보정
	if (x < 0) {
		x += b0;
	}

	return x;
}

int main()
{
	char M_buffer[1024];
	memset(M_buffer, 0, sizeof(M_buffer));
	printf("서명할 평문 메시지를 입력하세요\n"); //버퍼를 통해 평문 M 입력받기
	fgets(M_buffer, sizeof(M_buffer), stdin);
	M_buffer[strcspn(M_buffer, "\n")] = '\0'; //fgets 마지막에 입력되는 \n을 \0으로 교체


	char buffer[1024];
	long long p, q, g;
	
	printf("소수 p를 입력해주세요(공백 없이 숫자만 입력하세요)\n");
	fgets(buffer, sizeof(buffer), stdin); //버퍼를 통해 p입력받기
	p = strtoll(buffer, NULL, 10);
	fspace(buffer); //버퍼 입력값에 공백 있는지 검증
	if (isPrime(p) == 2) //p가 소수인지 검증
	{
		printf("소수가 아닙니다\n");
		exit(0);
	}

	long int resbuff[1024] = { 0, };
	factorize(p - 1, resbuff, 0); //소인수분해하여 소인수를 resbuff에 저장

	int t = 0;
	while (t < 1024 && resbuff[t] != 0)
	{
		t++;
	}
	if (t == 0) //에러코드
	{
		printf("소인수를 찾을 수 없습니다.\n");
		return 1;
	}
	q = resbuff[c_randbelow(t)]; //소인수중에 랜덤 q 선별

	long long exp = (p - 1) / q;
	long long h;

	while (1)
	{
		h = c_randbelow(p - 2) + 2; // 2 ~ p-1 사이에서 무작위 h 선별
		g = ModPow(h, exp, p);

		if (g > 1) break; // g가 1보다 크다면 올바른 위수를 가진 것이므로 루프 탈출
	}

	long long x, y;

	x = c_randbelow(q - 1) + 1; //x의 최솟값 1
	y = ModPow(g, x, p);

	long long k = c_randbelow(q - 1) + 1;

	long long r = ModPow(g, k, p) % q;


	long long hash_val = HASH((unsigned char*)M_buffer);

	long long s = (revmod(k, q) * (hash_val + x * r)) % q;
	if (s < 0)
	{
		s += q;
	}

	//주석 해제 시 전체 파라미터 확인가능 (Ctrl + K + U)
	//printf("p : %lld\nq : %lld\ng : %lld\nx : %lld\ny : %lld\ns : %lld\n", p, q, g, x, y, s);

	printf("평문(plaintext) : ");
	for (int i = 0; i < sizeof(M_buffer); i++)
	{
		printf("%c", M_buffer[i]);
	}
	printf("\n(r,s) : (%lld, %lld)\n", r, s);

	printf("%lld %lld %lld %lld\n", p, q, g, y);

	return 0;
}
