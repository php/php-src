#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# gperf_unfold_key_conv.py
# Copyright (c) 2016-2025  K.Kosako

import sys
import re

REG_LINE_GPERF = re.compile('#line .+gperf"')
REG_HASH_FUNC = re.compile('hash\s*\(register\s+const\s+char\s*\*\s*str,\s*register\s+size_t\s+len\s*\)')
REG_STR_AT = re.compile('str\[(\d+)\]')
REG_UNFOLD_KEY = re.compile('onigenc_unicode_unfold_key\s*\(register\s+const\s+char\s*\*\s*str,\s*register\s+size_t\s+len\)')
REG_ENTRY = re.compile('\{".+?",\s*/\*(.+?)\*/\s*(-?\d+),\s*(\d)\}')
REG_EMPTY_ENTRY = re.compile('\{"",\s*(-?\d+),\s*(\d)\}')
REG_IF_LEN = re.compile('\s*if\s*\(\s*len\s*<=\s*MAX_WORD_LENGTH.+')
REG_GET_HASH = re.compile('(?:register\s+)?(?:unsigned\s+)?int\s+key\s*=\s*hash\s*\(str,\s*len\);')
REG_GET_CODE = re.compile('(?:register\s+)?const\s+char\s*\*\s*s\s*=\s*wordlist\[key\]\.name;')
REG_CODE_CHECK = re.compile('if\s*\(\*str\s*==\s*\*s\s*&&\s*!strncmp.+\)')
REG_VOID_LEN = re.compile('^\s*\(void\s*\)\s*len\s*;')

def parse_line(s):
    s = s.rstrip()

    r = re.sub(REG_LINE_GPERF, '', s)
    if r != s: return r
    r = re.sub(REG_HASH_FUNC, 'hash(OnigCodePoint codes[])', s)
    if r != s: return r
    r = re.sub(REG_STR_AT, 'onig_codes_byte_at(codes, \\1)', s)
    if r != s: return r
    r = re.sub(REG_UNFOLD_KEY, 'onigenc_unicode_unfold_key(OnigCodePoint code)', s)
    if r != s: return r
    r = re.sub(REG_ENTRY, '{\\1, \\2, \\3}', s)
    if r != s: return r
    r = re.sub(REG_EMPTY_ENTRY, '{0xffffffff, \\1, \\2}', s)
    if r != s: return r
    r = re.sub(REG_IF_LEN, '', s)
    if r != s: return r
    r = re.sub(REG_GET_HASH, 'int key = hash(&code);', s)
    if r != s: return r
    r = re.sub(REG_GET_CODE, 'OnigCodePoint gcode = wordlist[key].code;', s)
    if r != s: return r
    r = re.sub(REG_CODE_CHECK, 'if (code == gcode && wordlist[key].index >= 0)', s)
    if r != s: return r
    r = re.sub(REG_VOID_LEN, '', s)
    if r != s: return r

    return s

def parse_file(f):
    print("/* This file was converted by gperf_unfold_key_conv.py\n      from gperf output file. */")

    line = f.readline()
    while line:
        s = parse_line(line)
        print(s)
        line = f.readline()


# main
parse_file(sys.stdin)
