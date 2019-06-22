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

import os
import sys
import glob

dst_dir = sys.argv[1]
locale_dir = "locale"

link_fmt = '<link rel="alternate" hreflang="{lang}" href="https://gpufanmeister.alkatrazstudio.net/{lang}/doc/{{filename}}" />'
link_fmt_en = '<link rel="alternate" hreflang="{lang}" href="https://gpufanmeister.alkatrazstudio.net/doc/{{filename}}" />'

langs = ["en"]
for name in os.listdir(locale_dir):
    if os.path.isdir(os.path.join(locale_dir, name)):
        langs.append(name)

links = []
for lang in langs:
    if lang == "en":
        fmt = link_fmt_en
    else:
        fmt = link_fmt
    link = fmt.format(lang=lang)
    links.append(link)

links = "\n".join(links)

os.chdir(dst_dir)

for fname in glob.iglob('**/*.html', recursive=True):
    with open(fname, 'r+') as f:
        html = f.read()
        html = html.replace('</title>', '</title>' + links.format(filename=fname))
        f.seek(0)
        f.write(html)
