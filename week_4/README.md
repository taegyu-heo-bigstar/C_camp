# Week 4: 같은 주소값과 서로 다른 프로세스 주소 공간

## 학습 목표

이 실습의 핵심 메시지는 다음과 같습니다.

> 주소값이 같다고 해서 같은 물리 메모리를 의미하지 않는다.  
> 각 프로세스는 자기만의 가상 주소 공간을 가진다.  
> 따라서 서로 다른 프로세스에서 같은 `0x...` 주소가 보여도, 그 주소는 각 프로세스의 page table을 통해 별도로 해석된다.

Linux 커널 문서는 page table이 CPU가 보는 **virtual address**를 외부 메모리 버스가 보는 **physical address**로 매핑한다고 설명합니다.
즉, 사용자 프로그램에서 출력한 포인터 값은 보통 물리 주소가 아니라 프로세스의 가상 주소입니다.

참고 자료:

- Linux Kernel Documentation: Page Tables  
  <https://docs.kernel.org/mm/page_tables.html>
- man7.org: `fork(2)`  
  <https://man7.org/linux/man-pages/man2/fork.2.html>
- man7.org: `mmap(2)`  
  <https://man7.org/linux/man-pages/man2/mmap.2.html>
- man7.org: `/proc/pid/maps`  
  <https://man7.org/linux/man-pages/man5/proc_pid_maps.5.html>

---

## 파일 구성

| 파일 | 목적 |
|---|---|
| `same_va_fork.c` | `fork()` 이후 부모와 자식이 같은 가상 주소를 보이지만 값은 분리될 수 있음을 확인 |
| `same_va_mmap.c` | 독립적으로 실행한 두 프로세스가 같은 가상 주소에 각자 다른 내용을 저장할 수 있음을 확인 |
| `Makefile` | 컴파일과 실행 명령 단순화 |

---

## 빌드

```bash
make
```

개별 컴파일:

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic -g same_va_fork.c -o same_va_fork
gcc -std=c11 -Wall -Wextra -Wpedantic -g same_va_mmap.c -o same_va_mmap
```

정리:

```bash
make clean
```

---

## 예제 1. `fork()`로 같은 가상 주소, 다른 값 보여주기

실행:

```bash
make run-fork
```

또는:

```bash
./same_va_fork
```

관찰해야 할 부분:

```text
부모의 heap_value 주소 = 자식의 heap_value 주소
부모의 *heap_value 값 != 자식의 *heap_value 값
```

예상 해석:

```text
주소값은 같다.
하지만 같은 메모리 객체를 공유하는 것은 아니다.
각 프로세스의 가상 주소 공간에서 같은 숫자 주소가 서로 다른 실제 메모리 상태를 가리킬 수 있다.
```

정확히 말하면, `fork()` 직후에는 부모와 자식의 일부 page가 copy-on-write 방식으로 공유될 수 있습니다.
그러나 자식이 쓰기를 수행하면 해당 page가 분리되어, 같은 가상 주소값을 갖더라도 부모와 자식은 서로 다른 값을 보게 됩니다.

---

## `/proc/<pid>/maps`로 주소 공간 보기

`same_va_fork`는 부모와 자식이 15초 동안 대기하도록 작성되어 있습니다.
프로그램 출력에 표시되는 PID를 확인한 뒤, 다른 터미널에서 다음 명령을 실행합니다.

```bash
cat /proc/<parent_pid>/maps
cat /proc/<child_pid>/maps
```

예:

```bash
cat /proc/12001/maps
cat /proc/12002/maps
```

`/proc/<pid>/maps`는 해당 프로세스에 현재 매핑된 메모리 영역과 접근 권한을 보여주는 파일입니다.
출력의 address field는 그 매핑이 프로세스 주소 공간에서 차지하는 주소 범위를 의미합니다.

수업에서 찾아볼 영역:

```text
[heap]
[stack]
실행 파일 경로
libc.so
ld-linux
[vvar]
[vdso]
```

질문:

1. 부모와 자식의 `[heap]` 주소 범위가 비슷하거나 같은가?
2. 부모와 자식의 `[stack]` 주소 범위가 비슷하거나 같은가?
3. 같은 주소 범위가 보인다고 해서 같은 물리 메모리라고 말할 수 있는가?
4. 자식이 값을 바꿨는데 부모 값이 유지되는 이유는 무엇인가?

---

## 예제 2. 독립 프로세스 2개가 같은 가상 주소를 직접 매핑하기

실행:

```bash
make run-mmap
```

또는:

```bash
./same_va_mmap 111 & ./same_va_mmap 222 & wait
```

관찰해야 할 부분:

```text
프로세스 A의 mapped VA = 0x700000000000
프로세스 B의 mapped VA = 0x700000000000

프로세스 A의 내용 = label=111
프로세스 B의 내용 = label=222
```

예상 해석:

```text
두 프로세스에서 같은 가상 주소값이 등장한다.
그러나 각 프로세스의 주소 공간은 별개이므로,
같은 주소값이 같은 실제 메모리 상태를 의미하지 않는다.
```

주의:

- `same_va_mmap.c`는 x86-64 Linux 사용자 공간에서 비어 있을 가능성이 높은 주소 후보를 사용합니다.
- 환경에 따라 후보 주소 매핑이 실패할 수 있습니다.
- 실패하면 `same_va_mmap.c` 안의 `candidates` 배열 주소를 변경해 다시 시도합니다.
- `MAP_FIXED_NOREPLACE`를 사용하므로 기존 매핑을 강제로 덮어쓰지 않습니다.

---

## 두 예제의 차이

| 예제 | 관계 | 보여주는 개념 |
|---|---|---|
| `same_va_fork.c` | 부모-자식 프로세스 | `fork()` 후 같은 가상 주소 배치, copy-on-write |
| `same_va_mmap.c` | 독립 실행 프로세스 | 서로 다른 프로세스가 같은 가상 주소값을 각자 사용할 수 있음 |

수업에서는 먼저 `fork()` 예제를 보여주고, 그다음 `mmap()` 예제로 확장하는 순서가 좋습니다.

---

## 핵심 문장

포인터 값은 주소입니다.
하지만 Linux 사용자 프로세스에서 그 주소는 보통 물리 주소가 아니라 프로세스의 가상 주소 공간 안의 주소입니다.
서로 다른 프로세스는 각자 별도의 주소 공간과 page table을 가집니다.
따라서 두 프로세스에서 같은 `0x700000000000` 주소가 보이더라도, 그 주소가 같은 물리 메모리나 같은 값을 의미한다고 말할 수 없습니다.
`fork()` 직후에는 부모와 자식이 같은 가상 주소 배치를 보일 수 있고, Linux는 이를 copy-on-write 방식으로 효율적으로 처리합니다.
