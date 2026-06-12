# Digital Signature Algorithm (DSA) Implementation (C)

이 프로젝트는 공개키 암호학 및 무결성 검증의 핵심 알고리즘인 **디지털 서명 알고리즘(DSA, Digital Signature Algorithm)**을 C 언어와 OpenSSL 라이브러리를 활용하여 구현한 보안 프로그래밍 예제입니다.

## 1. 프로젝트 개요
* **디지털 서명 (DSA)**: 메시지의 기밀성이 아닌 **신원 확인(인증)**과 **데이터 위변조 방지(무결성)**를 목적으로 하며, 수학적 이산로그 문제(Discrete Logarithm Problem)의 난해함에 기반을 둔 알고리즘입니다.
* **평문 전송 및 서명 검증**: 메시지 자체는 평문(Plaintext)으로 안전하게 전송되며, 메시지와 함께 동봉된 거대한 숫자 쌍인 서명값 $(r, s)$를 대조하여 검증을 수행합니다.

## 2. 주요 기능 및 특징
* **암호학적 안전한 난수 생성**: 보안성이 없는 일반 `rand()` 대신, OpenSSL 라이브러리의 `RAND_bytes` API를 사용하여 강력한 난수를 추출하고 Modulo Bias(모듈로 편향)를 수학적으로 제거했습니다.
* **곱셈 오버플로우 방지 (`ModPow`)**: 대형 정수의 거듭제곱 연산 시 `long long` 범위를 초과하는 유실을 막기 위해, 이진 비트 분할 및 덧셈 기반의 모듈러 거듭제곱 알고리즘을 수동 구현하여 안전성을 확보했습니다.
* **정밀한 수학적 파이프라인**: 입력받은 소수 $p$를 기반으로 $p-1$을 정밀하게 소인수분해하여 하부 그룹 위수 $q$를 선별하고, 생성원 $g$, 비밀키 $x$, 공개키 $y$를 도출하는 DSA의 핵심 연산 흐름을 완벽히 구현했습니다.
* **안정적인 입력 및 버퍼 처리**: `fgets`를 사용하여 입력 버퍼 오버플로우를 차단하고, 문자열 입력 시 개행 문자(`\n`) 자동 정정 및 공백 입력 제한 가드가 포함되어 있습니다.
* **재귀 카운터 포인터 제어**: 소인수분해(`factorize`) 함수 구동 시, C언어의 포인터 주소 참조 방식을 활용하여 재귀 호출 스택 간의 데이터 누수 없이 정밀하게 소인수를 축적합니다.

## 3. 서명 생성 원리
본 프로그램은 다음과 같은 암호학적 구조를 따릅니다.
1. **Hash 변환**: 입력된 평문 메시지 $M$을 OpenSSL 표준 SHA-256을 통해 해시화한 후, 앞 8바이트를 추출하여 64비트 정수 $H(M)$으로 정규화합니다.
2. **좌표값 $r$ 생성**: 무작위 난수 $k$를 생성하여 $r = (g^k \pmod p) \pmod q$를 계산합니다.
3. **봉인값 $s$ 생성**: 유클리드 호제법 확장 연산(`revmod`)을 통해 모듈러 역원을 구해낸 뒤, $s = k^{-1}(H(M) + x \cdot r) \pmod q$를 연산하여 최종 숫자 쌍 $(r, s)$를 도출합니다.

## 4. 실행 방법
이 프로그램은 **OpenSSL 라이브러리** 링크가 필요합니다. 

### 컴파일 (GCC 기준)
```bash
gcc -o dsa DSA.c -lssl -lcrypto
```
(Windows Visual Studio 환경의 경우, 프로젝트 속성에서 OpenSSL Include 및 Library 디렉터리 세팅과 libssl.lib, libcrypto.lib 추가 종속성 링크가 필요합니다.)

### 실행
```
./dsa
```

## 5. 정상적인 실행 예시
```
서명할 평문 메시지를 입력하세요
Hello World!
소수 p를 입력해주세요(공백 없이 숫자만 입력하세요)
1000000007

평문(plaintext) : Hello World!
(r,s) : (333013826, 354172751)
```

# [English Version] (.translated by Gemini AI)

This project is a security programming example that implements the **Digital Signature Algorithm (DSA)**, a cornerstone of public-key cryptography and data integrity verification, using the C language and the OpenSSL library.

## 1. Project Overview
* **Digital Signature (DSA)**: Designed not for message confidentiality, but for **identity verification (authentication)** and **data tampering prevention (integrity)**, based on the mathematical hardness of the Discrete Logarithm Problem.
* **Plaintext Transmission**: The message itself is transmitted as clear plaintext, and verification is achieved by auditing the giant pair of integer signature components, $(r, s)$, bundled with the message.

## 2. Key Features
* **Cryptographically Secure Random Numbers**: Avoids insecure standard `rand()`, instead utilizing OpenSSL's `RAND_bytes` API to fetch robust randomness while mathematically eliminating Modulo Bias.
* **Overflow-Resistant Logic (`ModPow`)**: To prevent data loss exceeding the `long long` boundary during large integer exponentiation, a custom modular exponentiation algorithm based on binary bitwise partitioning and addition was implemented.
* **Rigorous Mathematical Pipeline**: Factoring $p-1$ from the user-supplied prime $p$ to isolate the subgroup order $q$, and mathematically deducing the generator $g$, private key $x$, and public key $y$.
* **Secure Input & Buffer Handling**: Uses `fgets` to prevent buffer overflow vulnerabilities, featuring automatic newline (`\n`) trimming and a strict white-space restriction guard.
* **Recursive Pointer Control**: Manages array indexing via C-style pass-by-reference pointers inside the `factorize` function, preventing index leaks across deep recursive stack frames.

## 3. Signature Principles
The program operates under the following cryptographic workflow:
1. **Hash Transform**: Hashes the plaintext $M$ using standard OpenSSL SHA-256, extracting the first 8 bytes into a normalized 64-bit integer $H(M)$.
2. **Component $r$ Generation**: Generates a random nonce $k$ to solve $r = (g^k \pmod p) \pmod q$.
3. **Component $s$ Generation**: Obtains the modular inverse via the Extended Euclidean Algorithm (`revmod`), then computes $s = k^{-1}(H(M) + x \cdot r) \pmod q$ to lock the permanent signature pair $(r, s)$.

## 4. How to Run
This program requires the OpenSSL library dependencies.

### Compile (GCC)
```bash
gcc -o dsa DSA.c -lssl -lcrypto
```
(For Windows Visual Studio, ensure OpenSSL Include/Library Directories are configured, and link libssl.lib and libcrypto.lib via Additional Dependencies.)

### Run
```
./dsa
```
### 5. Execution Example
```
서명할 평문 메시지를 입력하세요
Hello World!
소수 p를 입력해주세요(공백 없이 숫자만 입력하세요)
1000000007

평문(plaintext) : Hello World!
(r,s) : (333013826, 354172751)
```
