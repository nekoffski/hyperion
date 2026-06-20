import unittest
import os
import enum
from assertpy import *
from .cfg import Config


class TestBase(unittest.TestCase):
    def setUp(self):
        super().setUp()

        self._home = os.environ.get("HYPERION_HOME")
        assert_that(self._home).is_not_none().is_not_empty()

        self._config = Config.from_file(
            os.path.join(self._home, "etc/hyperion.toml"))
        assert_that(self._config).is_not_none()

        self.set_up()

    def tearDown(self):
        self.tear_down()
        super().tearDown()

    def set_up(self):
        pass

    def tear_down(self):
        pass

    def get_home(self) -> str:
        return self._home

    def get_conf(self) -> Config:
        return self._config
