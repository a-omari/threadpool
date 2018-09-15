#include <cstdio>
#include <functional>
#include "../threadpool.hpp"

using void_f_t = std::function<void(void)>;

void foo () {
	fprintf(stdout, "foo\n");
}

void bar () {
	fprintf(stdout, "bar\n");
}

void test1 (const size_t M) {
	for (size_t i = 0; i < M; ++i) {
		foo();
		bar();
		fprintf(stdout, "lambda\n");
	}
}

void test (const size_t N, const size_t M) {
	auto pool = ThreadPool<void_f_t>(N);

	pool.reserve(M);
	for (size_t i = 0; i < M; ++i) {
		pool.push(foo);
		pool.push(bar);
		pool.push([]() {
			fprintf(stdout, "lambda\n");
		});
	}
}

int main () {
	test(1, 0);
	test(1, 1);
	test(1, 10);

	// benchmark
// 	const auto N = 100000000;
// 	test1(N);
// 	test(2, N);

	for (size_t i = 1; i < 10; ++i) {
		for (size_t j = 1; j < 10000; ++j) {
			test(i, j);
		}
	}

	return 0;
}
