# flake8: noqa
import os

pkgConfigPath = os.environ.get("PKG_CONFIG_PATH")
if pkgConfigPath is None:
    pkgConfigPath = ""
pathList = re.split(":", pkgConfigPath)  # noqa

print(pathList)
