#!/usr/bin/env python3

##############################################################################
# prepare_local.py - prepare local documentation                             #
#                                                                            #
# Copyright (c) 2018 Alexey Parfenov <zxed@alkatrazstudio.net>               #
#                                                                            #
# This file is part of GPU Fan Meister.                                      #
#                                                                            #
# GPU Fan Meister is free software: you can redistribute it and/or modify it #
# under the terms of the GNU General Public License as published by          #
# the Free Software Foundation, either version 3 of the License,             #
# or (at your option) any later version.                                     #
#                                                                            #
# GPU Fan Meister is distributed in the hope that it will be useful,         #
# but WITHOUT ANY WARRANTY; without even the implied warranty of             #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU           #
# General Public License for more details: https://gnu.org/licenses/gpl.html #
##############################################################################

import base64
import glob
import os
import re
from urllib.parse import urlparse
import shutil
import sys

dst_dir = sys.argv[1]
os.chdir("{dir}/_static/css".format(dir = dst_dir))

KEEP_FONTS = [{
    "basename": "fontawesome-webfont.woff2",
    "mime": "font/woff2"
}]


def find(f, arr):
    try:
        f_arr = filter(f, arr)
        return next(f_arr)
    except:
        return None


def process_fontface(groups):
    pre_s, s, post_s = groups
    lines = []
    keep_fontface = False
    for line in s.split(";"):
        key, val = line.split(":", 2)
        if key.strip() == "src":
            srcList = []
            for srcLine in val.split(","):
                m = re.search(r"^(.*url\(\")([^\"]+)(.*)$", srcLine)
                if m:
                    pre_src, src, post_src = m.group(1, 2, 3)
                    src_file = urlparse(src).path
                    src_basename = os.path.basename(src_file)
                    kf = find(lambda kf: kf["basename"] == src_basename, KEEP_FONTS)
                    if kf:
                        with open(src_file, "rb") as f:
                            src_content = f.read()
                        b64 = base64.b64encode(src_content).decode()
                        b64_src = "data:{mime};base64,{data}".format(mime = kf["mime"], data = b64)
                        srcList.append(pre_src + b64_src + post_src)
                        keep_fontface = True
            val = ",".join(srcList)
        if val:
            lines.append(key + ":" + val)
    if not keep_fontface:
        return ""
    s = ";".join(lines)
    return pre_s + s + post_s


for filename in glob.glob("*.css"):
    with open(filename, "r+") as f:
        s = f.read()
        s = re.sub(r"(@font-face{)([^}]*)(})", lambda x: process_fontface(x.group(1, 2, 3)), s)
        f.seek(0)
        f.write(s)
        f.truncate()

shutil.rmtree("../fonts")
