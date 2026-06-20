import internal
import os
import signal

from assertpy import *


class TestDaemon(internal.TestBase):
    def test_daemon_process(self):
        c = self.get_conf()
        deamon_path = c.daemon.bin_path
        pid_file = c.daemon.pid_file

        try:
            # spawn the daemon process
            pid = internal.run_in_background(deamon_path)
            assert_that(pid).is_not_none()

            # check pidfile
            internal.wait_for(
                lambda: assert_that(pid_file).is_file(),
                timeout=5,
                interval=0.5
            )
            with open(pid_file, 'r') as f:
                pid_from_file = int(f.read().strip())
            assert_that(pid_from_file).is_equal_to(pid)

            # shutdown
            os.kill(pid, signal.SIGINT)

            # check if pidfile is gone
            internal.wait_for(
                lambda: assert_that(pid_file).does_not_exist(),
                timeout=5,
                interval=0.5
            )

        finally:
            # Clean up the daemon process
            os.kill(pid, signal.SIGKILL)
