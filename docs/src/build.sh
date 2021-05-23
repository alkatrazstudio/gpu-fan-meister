#!/usr/bin/env bash
set -e
cd "$(dirname -- "$(readlink -f -- "$0")")"

THELANG="$1"
MODE="$2"

if [[ -z $THELANG ]]
then
    THELANG="all"
fi
if [[ -z $MODE ]]
then
    MODE="local"
fi

function enterVenv
{
    if [[ ! -d venv ]]
    then
        python3 -m venv venv
        . venv/bin/activate
        pip install --upgrade pip
        pip install -r requirements.txt
    else
        . venv/bin/activate
    fi
}

function buildLang
{
    local THELANG="$1"

    case "$MODE" in
        po)
            if [[ $THELANG != "en" ]]
            then
                sphinx-build -M gettext rst build/i18n -c .
                sphinx-intl update -p build/i18n/gettext -l "$THELANG"
            fi
            return
        ;;

        site)
            if [[ $THELANG == "en" ]]
            then
                DESTDIR="../../site/dist/doc"
            else
                DESTDIR="../../site/dist/$THELANG/doc"
            fi
        ;;

        *)
            DESTDIR="../$THELANG"
            MODE=local
        ;;
    esac

    rm -rf "$DESTDIR"
    sphinx-build -b html -c . -W -q -D language="$THELANG" rst "$DESTDIR"
    find "$DESTDIR" -iname ".*" -exec rm -rf {} +
    cp -a static/. "$DESTDIR/_static/."

    "./prepare_$MODE.py" "$DESTDIR"
}

enterVenv

if [[ $THELANG == "all" ]]
then
    buildLang en
    for DIRNAME in locale/*
    do
        if [[ -d $DIRNAME ]]
        then
            buildLang "$(basename -- $DIRNAME)"
        fi
    done
else
    buildLang "$THELANG"
fi
