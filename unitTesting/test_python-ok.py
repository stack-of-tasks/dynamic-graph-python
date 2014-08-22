import sys, os
import re

pkgConfigPath = os.environ.get("PKG_CONFIG_PATH")
if pkgConfigPath == None:
  pkgConfigPath = ''
pathList = re.split(':', pkgConfigPath)

print pathList

