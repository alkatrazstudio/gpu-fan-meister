#!/usr/bin/env python3

##############################################################################
# gpu-fan-meister.py - main application launcher                             #
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
import re
import subprocess
import sys
import multiprocessing.pool
import argparse
import importlib


THISFILE = os.path.realpath(__file__)
THISDIR = os.path.dirname(THISFILE)

MSG_TITLE = "GPU Fan Meister"
MAIN_EXE_NAME = os.path.splitext(os.path.basename(THISFILE))[0]
ENV_VAR_PREFIX = MAIN_EXE_NAME.replace("-", "").upper()

ERR_OK = 0
ERR_MAIN = 10
ERR_PLUGIN_INIT = 11
ERR_PLUGIN_DEINIT = 12
ERR_KEYBOARD_INTERRUPT = 130

sys.path.append(os.path.realpath(THISDIR + "/pynotify2"))  # use system packages if available
notify2 = importlib.import_module("notify2")


def show_msg(s):
    if not hasattr(show_msg, "inited"):
        notify2.init(MAIN_EXE_NAME)
        show_msg.inited = True
    n = notify2.Notification(MSG_TITLE, s)
    n.set_timeout(notify2.EXPIRES_NEVER)
    n.set_urgency(notify2.URGENCY_CRITICAL)
    n.show()


def run_exe(args, pass_through=False, suppress_msg=False):
    try:
        res = subprocess.run(
            args,
            stdout=subprocess.PIPE if not pass_through else None,
            stderr=subprocess.PIPE if not pass_through else None,
            check=True,
            universal_newlines=True)
        return res.stdout
    except subprocess.CalledProcessError as e:
        if not suppress_msg:
            show_msg("{} exited with code {}: {}".format(" ".join(args), e.returncode, e.stderr))
        raise RuntimeError(e.stderr)


def main(args):
    #
    # setup the environment
    #
    os.chdir(THISDIR)

    if "QT_AUTO_SCREEN_SCALE_FACTOR" not in os.environ and "QT_SCREEN_SCALE_FACTORS" not in os.environ:
            os.environ["QT_AUTO_SCREEN_SCALE_FACTOR"] = "1"

    main_exe = os.path.join(THISDIR, MAIN_EXE_NAME)
    exit_code = ERR_OK

    #
    # pass all unknown args straight to the main app
    #
    parser = argparse.ArgumentParser(add_help=False)
    parser.add_argument("--plugins", nargs="?", const="", default="")
    known_args, unknown_args = parser.parse_known_args(args)
    if len(unknown_args):
        try:
            # assuming that the app has been started via command line
            # so do not show notifications
            stdout = run_exe([main_exe] + args, suppress_msg=True).strip()
            if "APPIMAGE" in os.environ and "ARGV0" in os.environ:
                argv0 = os.environ["ARGV0"]
            else:
                argv0 = sys.argv[0]
            print(stdout.replace(main_exe, argv0))
        except RuntimeError as e:
            print(str(e))
            exit_code = ERR_MAIN
        return exit_code

    #
    # get enabled plugins
    #
    if known_args.plugins:
        plugins_via_cmdline = True
        plugin_names = map(str.strip, known_args.plugins.split(","))
        plugin_names = list(set(plugin_names))  # get unique values
        stdout = run_exe([main_exe, "--available-plugins"]).strip()
        lines = map(lambda line: line.strip().split(":", 1), stdout.split("\n")) if stdout else []
        available_plugins = {name: dir_name for name, dir_name in lines}
        plugin_items = {}
        try:
            for plugin_name in plugin_names:
                if plugin_name in available_plugins:
                    plugin_items[plugin_name] = available_plugins[plugin_name]
                else:
                    raise RuntimeError("Unavailable plugin: {}".format(plugin_name))
        except RuntimeError:
            return ERR_PLUGIN_INIT
    else:
        try:
            plugins_via_cmdline = False
            stdout = run_exe([main_exe, "--enabled-plugins"]).strip()
            lines = list(map(lambda line: line.strip().split(":", 1), stdout.split("\n")) if stdout else [])
            plugin_items = {name: dir_name for name, dir_name in lines}
            plugin_names = [name for name, _ in lines]
        except RuntimeError:
            return ERR_MAIN

    #
    # validate enabled plugins
    #
    plugins = []
    cats = {}
    added_plugins = []
    for plugin_name in plugin_names:
        try:
            if plugin_name in added_plugins:
                raise RuntimeError("plugin is specified more than once")
            added_plugins.append(plugin_name)
            plugin = {
                "name": plugin_name
            }
            plugin["dir"] = plugin_items[plugin["name"]]
            if not os.path.isdir(plugin["dir"]):
                raise RuntimeError("plugin directory not found")

            # check for conflicting plugins
            parts = plugin["name"].split("-", 1)
            if len(parts) != 2:
                raise RuntimeError("plugin category not set")
            cat = parts[0]
            if cat in cats:
                raise RuntimeError(
                    "another plugin from the same category: {}"
                    .format(cats[cat]))
            cats[cat] = plugin["name"]

            # search for init and deinit executables
            modes = ["init", "deinit"]
            for plugin_dir_item in os.scandir(plugin["dir"]):
                if not plugin_dir_item.is_file():
                    continue
                basename, ext = os.path.splitext(plugin_dir_item.name)
                for mode in modes:
                    if basename.startswith(plugin["name"] + "-" + mode):
                        if mode in plugin:
                            raise RuntimeError(
                                "more than one {} entry: {} and {}"
                                .format(mode, plugin[mode+"_name"], plugin_dir_item.name))
                        plugin[mode] = plugin_dir_item.path
                        plugin[mode+"_name"] = plugin_dir_item.name
                        if not os.access(plugin[mode], os.R_OK):
                            raise RuntimeError("{} entry is not readable: {}".format(mode, plugin[mode]))
                        if not os.access(plugin[mode], os.X_OK):
                            raise RuntimeError("{} entry is not executable: {}".format(mode, plugin[mode]))
            for mode in modes:
                if mode not in plugin:
                    raise RuntimeError("{} executable not found".format(mode))

            plugins.append(plugin)
        except (OSError, RuntimeError) as e:
            show_msg("Plugin \"{}\": {}".format(plugin_name, e))
            exit_code = ERR_PLUGIN_INIT
    plugins = sorted(plugins, key=lambda plugin : plugin["name"])

    if not plugins:
        if plugins_via_cmdline:
            raise RuntimeError("no plugins loaded")
        else:
            try:
                run_exe([main_exe, "--plugin-selector"])
                exit_code = ERR_OK
            except RuntimeError:
                exit_code = ERR_MAIN
            return exit_code

    #
    # set program version in environment
    #
    try:
        stdout = run_exe([main_exe, "--version"])
    except RuntimeError:
        return ERR_MAIN

    try:
        ver_str = stdout.split(" ", 2)[1].strip()
        m = re.match(r"^(\d+)\.(\d+)\.(\d+)$", ver_str)
        if not m:
            raise RuntimeError
        ver_maj, ver_min, ver_pat = ver_str.split(".", 3)
    except (IndexError, RuntimeError, ValueError):
        show_msg("invalid version output: {}".format(stdout))
        return ERR_MAIN

    os.environ[ENV_VAR_PREFIX + "_VER_MAJ"] = ver_maj
    os.environ[ENV_VAR_PREFIX + "_VER_MIN"] = ver_min
    os.environ[ENV_VAR_PREFIX + "_VER_PAT"] = ver_pat

    #
    # run init executables
    #
    inited_plugins = []
    for plugin in plugins:
        try:
            stdout = run_exe([plugin["init"]])
            lines = stdout.split("\n")
            lines = map(str.strip, lines)
            lines = filter(lambda s: s != "" and s[0] != "#", lines)
            keyvals = map(lambda s: s.split("=", 2), lines)
            keyvals = filter(lambda kv: len(kv) == 2, keyvals)
            keyvals = {kv[0].rstrip(): kv[1].lstrip() for kv in keyvals}
            plugin["env"] = {key: val for key, val in keyvals.items() if val != ""}
            inited_plugins.append(plugin)
        except RuntimeError as e:
            show_msg("Plugin \"{}\" init: {}".format(plugin["name"], e))
            exit_code = ERR_PLUGIN_INIT
    plugins = inited_plugins

    #
    # set environment based on plugins output
    #
    for plugin in plugins:
        for key, val in plugin["env"].items():
            if key not in os.environ:
                os.environ[key] = val.strip(os.pathsep)
            else:
                if val[-1] == os.pathsep:
                    os.environ[key] = val + os.environ[key]
                elif val[0] == os.pathsep:
                    os.environ[key] = os.environ[key] + val
                else:
                    os.environ[key] = val

    #
    # run the main executable
    #
    if exit_code == ERR_OK:
        try:
            plugin_names = map(lambda p: p["name"], plugins)
            run_exe([main_exe, "--plugins=" + str.join(",", plugin_names)], True)
        except RuntimeError:
            exit_code = ERR_MAIN

    #
    # run deinit executables
    #
    for plugin in plugins:
        try:
            run_exe([plugin["deinit"]])
        except RuntimeError:
            exit_code = ERR_PLUGIN_DEINIT

    return exit_code


if __name__ == "__main__":
    try:
        # run in a separate thread to defer KeyboardInterrupt
        try:
            pool = multiprocessing.pool.ThreadPool()                       # chaining everything in one line
            exit_code = pool.apply_async(main, args=[sys.argv[1:]]).get()  # hangs the application for some reason
        except KeyboardInterrupt:
            exit_code = ERR_KEYBOARD_INTERRUPT
        sys.exit(exit_code)
    except Exception as e:
        show_msg(str(e))
        raise
