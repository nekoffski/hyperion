import pytoml
import dataclasses


@dataclasses.dataclass
class DaemonConfig(object):
    port: int
    pid_file: str
    bin_path: str


@dataclasses.dataclass
class LoggingConfig(object):
    daemon_level: str
    cli_level: str
    daemon_file: str
    daemon_err: str


@dataclasses.dataclass
class Config(object):
    daemon: DaemonConfig
    logging: LoggingConfig

    @staticmethod
    def from_file(path: str) -> "Config":
        try:
            with open(path, "r") as f:
                data = pytoml.load(f)
                return Config(
                    daemon=DaemonConfig(**data["daemon"]),
                    logging=LoggingConfig(**data["logging"])
                )
        except Exception as e:
            raise RuntimeError(f"Failed to load config from {path}: {e}")
