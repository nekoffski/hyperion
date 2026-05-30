#include "Thread.hh"

namespace hyperion {

void Thread::start() {}

void Thread::join() {}

void ThreadGroup::start() {
    for (auto& thread : m_threads) {
        thread->start();
    }
}

void ThreadGroup::join() {
    for (auto& thread : m_threads) {
        thread->join();
    }
}

}  // namespace hyperion
