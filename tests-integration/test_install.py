import internal

from assertpy import *


class TestInstall(internal.TestBase):
    def test_bin_path_exists(self):
        c = self.get_conf()
        assert_that(c.daemon.bin_path).is_not_none().is_not_empty()
        assert_that(c.daemon.bin_path).is_file()
