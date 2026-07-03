#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>

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
	long long r, s, p, q, g, y, w, u_1, u_2, v;
	char M_buffer[1024];
	memset(M_buffer, 0, sizeof(M_buffer));
	printf("전달받은 평문 메시지를 입력하세요\n");
	fgets(M_buffer, sizeof(M_buffer), stdin);
	M_buffer[strcspn(M_buffer, "\n")] = '\0';
	long long hash_val = HASH((unsigned char*)M_buffer);

	char parameter[1024];
	printf("각 파라미터를 입력하세요\nr : ");
	fgets(parameter, sizeof(parameter), stdin);
	r = strtoll(parameter, NULL, 10);
	printf("s : ");
	fgets(parameter, sizeof(parameter), stdin);
	s = strtoll(parameter, NULL, 10);

	printf("p : ");
	fgets(parameter, sizeof(parameter), stdin);
	p = strtoll(parameter, NULL, 10);
	printf("q : ");
	fgets(parameter, sizeof(parameter), stdin);
	q = strtoll(parameter, NULL, 10);
	printf("g : ");
	fgets(parameter, sizeof(parameter), stdin);
	g = strtoll(parameter, NULL, 10);
	printf("y : ");
	fgets(parameter, sizeof(parameter), stdin);
	y = strtoll(parameter, NULL, 10);


	w = revmod(s, q);
	u_1 = HASH((unsigned char*)M_buffer) * w;
	u_2 = r * w;

	if (r == ((ModPow(g, u_1, p)) * (ModPow(y, u_2,p))) % q)
		printf("\nVaild notarization\n");
	else
		printf("\nInvalid noatrization\n");

	return 0;
}
