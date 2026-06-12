#include "UnixProcess.hh"

#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

#include "internal/core/FileSystem.hh"
#include "internal/core/Scope.hh"

namespace hyperion {

namespace {

[[noreturn]] void failChild(i32 statusFd) {
    const i32 childErrno = errno;
    (void)::write(statusFd, &childErrno, sizeof(childErrno));
    _exit(127);
}

void setFdCloseOnExec(i32 fd) {
    if (::fcntl(fd, F_SETFD, FD_CLOEXEC) < 0) {
        throw ProcessError{
            ErrorCode::childProcessFailed,
            "fcntl(FD_CLOEXEC) failed before spawn: {}", std::strerror(errno)
        };
    }
}

std::array<i32, 2> createStatusPipe() {
    i32 fds[2]{};
    if (::pipe(fds) < 0) {
        throw ProcessError{
            ErrorCode::childProcessFailed, "pipe failed before spawn: {}",
            std::strerror(errno)
        };
    }

    try {
        setFdCloseOnExec(fds[0]);
        setFdCloseOnExec(fds[1]);
    } catch (...) {
        ::close(fds[0]);
        ::close(fds[1]);
        throw;
    }

    return {fds[0], fds[1]};
}

void redirectStdin(const Opt<Path>& input, i32 devnull, i32 statusFd) {
    if (input) {
        i32 fd = ::open(input->str().c_str(), O_RDONLY);
        if (fd < 0) {
            failChild(statusFd);
        }
        if (::dup2(fd, STDIN_FILENO) < 0) {
            ::close(fd);
            failChild(statusFd);
        }
        ::close(fd);
        return;
    }

    if (devnull >= 0 && ::dup2(devnull, STDIN_FILENO) < 0) {
        failChild(statusFd);
    }
}

void redirectOutput(
    const Opt<Path>& output, i32 targetFd, i32 devnull, i32 statusFd
) {
    if (output) {
        i32 fd =
            ::open(output->str().c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (fd < 0) {
            failChild(statusFd);
        }
        if (::dup2(fd, targetFd) < 0) {
            ::close(fd);
            failChild(statusFd);
        }
        ::close(fd);
        return;
    }

    if (devnull >= 0 && ::dup2(devnull, targetFd) < 0) {
        failChild(statusFd);
    }
}

Pid completeSpawnHandshake(
    pid_t childPid, i32 statusReadFd, std::string_view bin
) {
    i32 childErrno = 0;
    const auto bytesRead = ::read(
        statusReadFd, &childErrno, static_cast<size_t>(sizeof(childErrno))
    );
    ::close(statusReadFd);

    if (bytesRead == 0) {
        return static_cast<Pid>(childPid);
    }

    i32 childStatus = 0;
    (void)::waitpid(childPid, &childStatus, 0);

    if (bytesRead == static_cast<ssize_t>(sizeof(childErrno))) {
        throw ProcessError{
            ErrorCode::childProcessFailed,
            "Failed to spawn process '{}': startup error reported by child: {}",
            bin, std::strerror(childErrno)
        };
    }

    throw ProcessError{
        ErrorCode::childProcessFailed,
        "Failed to spawn process '{}': startup handshake failed", bin
    };
}

[[noreturn]] void runChildProcess(
    const ProcessDescription& desc, i32 statusWriteFd
) {
    if (::setsid() < 0) {
        failChild(statusWriteFd);
    }
    ::signal(SIGHUP, SIG_IGN);

    auto devnull = ::open("/dev/null", O_RDWR);
    ON_SCOPE_EXIT {
        if (devnull >= 0 && devnull > STDERR_FILENO) {
            ::close(devnull);
        }
    };

    redirectStdin(desc.stdin, devnull, statusWriteFd);
    redirectOutput(desc.stdout, STDOUT_FILENO, devnull, statusWriteFd);
    redirectOutput(desc.stderr, STDERR_FILENO, devnull, statusWriteFd);

    for (const auto& [key, value] : desc.env) {
        if (::setenv(key.c_str(), value.c_str(), 1) < 0) {
            failChild(statusWriteFd);
        }
    }

    std::vector<char*> argv;
    argv.reserve(desc.args.size() + 2);
    argv.push_back(const_cast<char*>(desc.bin.c_str()));
    for (const auto& arg : desc.args) {
        argv.push_back(const_cast<char*>(arg.c_str()));
    }
    argv.push_back(nullptr);

    ::execv(desc.bin.c_str(), argv.data());
    failChild(statusWriteFd);
}

}  // namespace

Pid UnixProcess::currentPid() const { return getpid(); }

Pid UnixProcess::spawn(const ProcessDescription& desc) {
    auto [rd, wr] = createStatusPipe();

    const auto pid = ::fork();
    if (pid < 0) {
        ::close(rd);
        ::close(wr);
        throw ProcessError{
            ErrorCode::childProcessFailed, "fork failed: {}",
            std::strerror(errno)
        };
    }

    if (pid > 0) {
        ::close(wr);
        return completeSpawnHandshake(pid, rd, desc.bin);
    }

    ::close(rd);
    runChildProcess(desc, wr);
}

bool UnixProcess::isRunning(Pid pid) const {
    if (::kill(static_cast<pid_t>(pid), 0) != 0 && errno != EPERM) {
        return false;
    }

    auto path = Path::fmt("/proc/{}/stat", pid);

    if (not path.isFile()) {
        return false;
    }

    auto stat = File{path}.read();

    const auto rightParen = stat.rfind(')');
    if (rightParen == Str::npos || rightParen + 2 >= stat.size()) {
        return true;
    }

    const auto state = stat[rightParen + 2];
    return state != 'Z' && state != 'X';
}

}  // namespace hyperion
