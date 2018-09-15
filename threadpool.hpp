#pragma once

#include <cassert>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

template <typename F>
struct ThreadPool {
private:
	std::vector<std::thread> pool;
	std::vector<F> jobs;

	std::atomic_bool joined = false;
	std::atomic_int running = 0;

	std::condition_variable idle;
	std::condition_variable go;
	std::mutex mutex;

	void thread () {
		// signal thread as running
		running++;

		while (true) {
			F job;
			{
				auto lock = std::unique_lock(mutex);

				if (jobs.empty()) {
					// signal thread as waiting
					running--;

					// notify thread is idle
					idle.notify_one();

					// wait until a job, or joined
					go.wait(lock, [this]() {
						return (not jobs.empty()) or joined;
					});

					if (joined) break;

					// signal thread as running
					running++;
				}

				job = jobs.back();
				jobs.pop_back();
			}

			job();
		}
	}

public:
	ThreadPool (const size_t N) {
		for (size_t i = 0; i < N; ++i) {
			pool.emplace_back(std::thread([this] {
				this->thread();
			}));
		}
	}

	~ThreadPool () {
		if (joined) return;

		this->join();
	}
	ThreadPool (const ThreadPool&) = delete;

	void join () {
		wait();
		joined = true;

		go.notify_all(); // notify ALL go's of 'joined'

		for (auto& x : pool) {
			x.join();
		}
	}

	void clear () { jobs.clear(); }
	void reserve (const size_t N) { jobs.reserve(N); }
	void push (const F f) {
		assert(not joined);

		auto guard = std::lock_guard(mutex);
		jobs.emplace_back(f);
		go.notify_one(); // notify ONE go of 'jobs' (modified)
	}

	void wait () {
		auto lock = std::unique_lock(mutex);
		idle.wait(lock, [this]() {
			return jobs.empty() && running == 0;
		});
	}
};
