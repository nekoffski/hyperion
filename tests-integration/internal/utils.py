import enum
import platform
import subprocess


class OS(enum.Enum):
    LINUX = "linux"
    MACOS = "macos"


def wait_for(condition, timeout: int = 10, interval: float = 0.5):
    import time

    last_exception = None
    start_time = time.time()

    while time.time() - start_time < timeout:
        try:
            condition()
        except AssertionError as e:
            last_exception = e
        else:
            return
        time.sleep(interval)
    raise TimeoutError(f"Condition not met within timeout: {last_exception}")


def get_os() -> OS:
    system = platform.system().lower()
    if system == "linux":
        return OS.LINUX
    elif system == "darwin":
        return OS.MACOS
    else:
        raise Exception(f"Unsupported OS: {system}")


def _create_process(cmd: str, args: list[str] = None, env: dict[str, str] = None):
    if args is None:
        args = []

    process = subprocess.Popen(
        [cmd] + args,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        env=env,
    )
    return process


def run_in_background(cmd: str, args: list[str] = None, env: dict[str, str] = None) -> int:
    return _create_process(cmd, args, env).pid


def run(cmd: str, args: list[str] = None, env: dict[str, str] = None) -> tuple[int, str, str]:
    process = _create_process(cmd, args, env)
    stdout, stderr = process.communicate()
    return process.returncode, stdout.decode(), stderr.decode()


def find_process(name: str) -> list[int]:
    if get_os() == OS.LINUX:
        result = subprocess.run(["pgrep", "-f", name], stdout=subprocess.PIPE)
        return [int(pid) for pid in result.stdout.decode().splitlines()]
    elif get_os() == OS.MACOS:
        result = subprocess.run(["pgrep", "-f", name], stdout=subprocess.PIPE)
        return [int(pid) for pid in result.stdout.decode().splitlines()]
    else:
        raise Exception("Unsupported OS")
