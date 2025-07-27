#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# make_unicode_fold_data.py
# Copyright (c) 2016-2024  K.Kosako

import sys
import re

SOURCE_FILE = 'CaseFolding.txt'
GPERF_UNFOLD_KEY_FILE = 'unicode_unfold_key.gperf'
GPERF_FOLD_KEY_FILES  = ['unicode_fold1_key.gperf', 'unicode_fold2_key.gperf', 'unicode_fold3_key.gperf']


DataName = 'OnigUnicodeFolds'

ENCODING = 'utf-8'

LINE_REG = re.compile("([0-9A-F]{1,6}); (.); ([0-9A-F]{1,6})(?: ([0-9A-F]{1,6}))?(?: ([0-9A-F]{1,6}))?;(?:\s*#\s*)(.*)")
VERSION_REG  = re.compile("#.*-(\d+)\.(\d+)\.(\d+)\.txt")

VERSION_INFO = [-1, -1, -1]

FOLDS = {}
TURKISH_FOLDS = {}
LOCALE_FOLDS  = {}

UNFOLDS = {}
TURKISH_UNFOLDS = {}
LOCALE_UNFOLDS  = {}

COPYRIGHT = '''
/*-
 * Copyright (c) 2017-2024  K.Kosako
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
'''.strip()


class Entry:
    def __init__(self, fold):
        self.fold = fold
        self.unfolds = []
        self.fold_len = len(fold)
        self.index = -1
        self.comment = None

def fold_key(fold):
    sfold = map(lambda i: "%06x" % i, fold)
    return ':'.join(sfold)

def form16(x, size):
    form = "0x%06x" if x > 0xffff else "0x%04x"
    s = form % x
    rem = size - len(s)
    if rem > 0:
        s = ' ' * rem + s

    return s

def form3bytes(x):
    x0 = x & 0xff
    x1 = (x>>8)  & 0xff
    x2 = (x>>16) & 0xff
    return "\\x%02x\\x%02x\\x%02x" % (x2, x1, x0)

def enc_len(code, encode):
    u = unichr(code)
    s = u.encode(encode)
    return len(s)

def check_version_info(s):
  m = VERSION_REG.match(s)
  if m is not None:
    VERSION_INFO[0] = int(m.group(1))
    VERSION_INFO[1] = int(m.group(2))
    VERSION_INFO[2] = int(m.group(3))

def parse_line(s):
    if len(s) == 0:
      return False
    if s[0] == '#':
      if VERSION_INFO[0] < 0:
        check_version_info(s)
      return False

    m = LINE_REG.match(s)
    if m is None:
        print >> sys.stderr, s.encode(ENCODING)
        sys.exit(-1)

    s_unfold = m.group(1)
    s_type   = m.group(2)
    s_fold   = m.group(3)
    comment  = m.group(6)

    if s_type == 'S':
        return False;

    unfold = int(s_unfold, 16)
    f1     = int(s_fold, 16)
    fold = [f1]
    if m.group(4) is not None:
        f2 = int(m.group(4), 16)
        fold.append(f2)
        if m.group(5) is not None:
            f3 = int(m.group(5), 16)
            fold.append(f3)

    if s_type == 'T':
        dic   = TURKISH_FOLDS
        undic = TURKISH_UNFOLDS
    else:
        dic   = FOLDS
        undic = UNFOLDS

    key = fold_key(fold)
    e = dic.get(key, None)
    if e is None:
        e = Entry(fold)
        e.comment = comment
        dic[key] = e

    e.unfolds.append(unfold)

    if undic.get(unfold, None) is not None:
        print >> sys.stderr, ("unfold dup: 0x%04x %s\n" % (unfold, s_type))
    undic[unfold] = e

    return True

def parse_file(f):
    line = f.readline()
    while line:
        s = line.strip()
        parse_line(s)
        line = f.readline()

def make_locale():
    for unfold, te in TURKISH_UNFOLDS.items():
        e = UNFOLDS.get(unfold, None)
        if e is None:
            continue

        fkey = fold_key(e.fold)
        if len(e.unfolds) == 1:
            del FOLDS[fkey]
        else:
            e.unfolds.remove(unfold)
            e = Entry(e.fold)
            e.unfolds.append(unfold)

        LOCALE_FOLDS[fkey] = e
        LOCALE_UNFOLDS[unfold] = e
        del UNFOLDS[unfold]

def output_typedef(f):
    s = """\
typedef unsigned long OnigCodePoint;
"""
    print >> f, s

def divide_by_fold_len(d):
    l = d.items()
    l1 = filter(lambda x:x[1].fold_len == 1, l)
    l2 = filter(lambda x:x[1].fold_len == 2, l)
    l3 = filter(lambda x:x[1].fold_len == 3, l)
    sl1 = sorted(l1, key=lambda x:x[0])
    sl2 = sorted(l2, key=lambda x:x[0])
    sl3 = sorted(l3, key=lambda x:x[0])
    return (sl1, sl2, sl3)

def output_comment(f, s):
    f.write(" /* %s */" % s)

def output_data_n1(f, n, fn, c, out_comment):
    for k, e in fn:
        e.index = c
        if out_comment and n > 1 and e.comment is not None:
            output_comment(f, e.comment)
            print('', file=f)

        f.write(' ')
        f.write("/*%4d*/ " % c)
        for i in range(0, n):
            s = form16(e.fold[i], 8)
            f.write(" %s," % s)

        usize = len(e.unfolds)
        f.write("  %d," % usize)
        for u in e.unfolds:
            s = form16(u, 8)
            f.write(" %s," % s)

        if out_comment and n == 1 and e.comment is not None:
            if len(e.comment) < 35:
                s = e.comment
            else:
                s = e.comment[0:33] + '..'

            output_comment(f, s)

        f.write("\n")
        c += n + 1 + usize

    return c

def output_data_n(f, name, n, fn, lfn, out_comment):
    print("OnigCodePoint %s%d[] = {" % (name, n), file=f)
    c = 0
    c = output_data_n1(f, n,  fn, c, out_comment)
    print("#define FOLDS%d_NORMAL_END_INDEX   %d" % (n, c), file=f)
    print(" /* ----- LOCALE ----- */", file=f)
    c = output_data_n1(f, n, lfn, c, out_comment)
    print("#define FOLDS%d_END_INDEX   %d" % (n, c), file=f)
    print("};", file=f)

def output_fold_data(f, name, out_comment):
    f1, f2, f3 = divide_by_fold_len(FOLDS)
    lf1, lf2, lf3 = divide_by_fold_len(LOCALE_FOLDS)

    output_data_n(f, name, 1, f1, lf1, out_comment)
    print('', file=f)
    output_data_n(f, name, 2, f2, lf2, out_comment)
    print('', file=f)
    output_data_n(f, name, 3, f3, lf3, out_comment)
    print('', file=f)

def output_macros(f, name):
    print >> f, "#define FOLDS1_FOLD(i)         (%s1 + (i))" % name
    print >> f, "#define FOLDS2_FOLD(i)         (%s2 + (i))" % name
    print >> f, "#define FOLDS3_FOLD(i)         (%s3 + (i))" % name

    print >> f, "#define FOLDS1_UNFOLDS_NUM(i)  %s1[(i)+1]" % name
    print >> f, "#define FOLDS2_UNFOLDS_NUM(i)  %s2[(i)+2]" % name
    print >> f, "#define FOLDS3_UNFOLDS_NUM(i)  %s3[(i)+3]" % name

    print >> f, "#define FOLDS1_UNFOLDS(i)      (%s1 + (i) + 2)" % name
    print >> f, "#define FOLDS2_UNFOLDS(i)      (%s2 + (i) + 3)" % name
    print >> f, "#define FOLDS3_UNFOLDS(i)      (%s3 + (i) + 4)" % name

    print >> f, "#define FOLDS1_NEXT_INDEX(i)   ((i) + 2 + %s1[(i)+1])" % name
    print >> f, "#define FOLDS2_NEXT_INDEX(i)   ((i) + 3 + %s1[(i)+2])" % name
    print >> f, "#define FOLDS3_NEXT_INDEX(i)   ((i) + 4 + %s1[(i)+3])" % name

def output_fold_source(f, out_comment):
    print("/* This file was generated by make_unicode_fold_data.py. */", file=f)
    print(COPYRIGHT, file=f)
    print("\n", file=f)
    print('#include "regenc.h"', file=f)
    print('', file=f)
    if VERSION_INFO[0] < 0:
      raise RuntimeError("Version is not found")

    print("#define UNICODE_CASEFOLD_VERSION  %02d%02d%02d" % (VERSION_INFO[0], VERSION_INFO[1], VERSION_INFO[2]))
    print('')
    #output_macros(f, DataName)
    print('', file=f)
    #output_typedef(f)
    output_fold_data(f, DataName, out_comment)


def output_gperf_unfold_key(f):
    head = "%{\n/* This gperf source file was generated by make_unicode_fold_data.py */\n\n" + COPYRIGHT + """\

#include "regint.h"
%}

struct ByUnfoldKey {
  OnigCodePoint code;
  short int   index;
  short int   fold_len;
};
%%
"""
    f.write(head)
    UNFOLDS.update(LOCALE_UNFOLDS)
    l = UNFOLDS.items()
    sl = sorted(l, key=lambda x:(x[1].fold_len, x[1].index))
    for k, e in sl:
        f.write('"%s", /*0x%04x*/ %4d, %d\n' %
                (form3bytes(k), k, e.index, e.fold_len))

    print('%%', file=f)

def output_gperf_fold_key(f, key_len):
    head = "%{\n/* This gperf source file was generated by make_unicode_fold_data.py */\n\n" + COPYRIGHT + """\

#include "regint.h"
%}

short int
%%
"""
    f.write(head)
    l = FOLDS.items()
    l = filter(lambda x:x[1].fold_len == key_len, l)
    sl = sorted(l, key=lambda x:x[1].index)
    for k, e in sl:
        skey = ''.join(map(lambda i: form3bytes(i), e.fold))
        f.write('"%s", %4d\n' % (skey, e.index))

    print('%%', file=f)

def output_gperf_source():
   with open(GPERF_UNFOLD_KEY_FILE, 'w') as f:
       output_gperf_unfold_key(f)

   FOLDS.update(LOCALE_FOLDS)

   for i in range(1, 4):
       with open(GPERF_FOLD_KEY_FILES[i-1], 'w') as f:
           output_gperf_fold_key(f, i)

def unfolds_byte_length_check(encode):
    l = UNFOLDS.items()
    sl = sorted(l, key=lambda x:(x[1].fold_len, x[1].index))
    for unfold, e in sl:
        key_len = enc_len(unfold, encode)
        fold_len = sum(map(lambda c: enc_len(c, encode), e.fold))
        if key_len > fold_len:
            sfolds = ' '.join(map(lambda c: "0x%06x" % c, e.fold))
            s = "%s byte length: %d > %d: 0x%06x => %s" % (encode, key_len, fold_len, unfold, sfolds)
            print >> sys.stderr, s

def double_fold_check():
    l = UNFOLDS.items()
    sl = sorted(l, key=lambda x:(x[1].fold_len, x[1].index))
    for unfold, e in sl:
        for f in e.fold:
            #print >> sys.stderr, ("check 0x%06x" % f)
            e2 = UNFOLDS.get(f)
            if e2 is not None:
                s = "double folds: 0x%06x => %s, 0x%06x => %s" % (unfold, e.fold, f, e2.fold)
                print >> sys.stderr, s

def unfold_is_multi_code_folds_head_check():
    l = UNFOLDS.items()
    l2 = filter(lambda x:x[1].fold_len == 2, l)
    l3 = filter(lambda x:x[1].fold_len == 3, l)
    sl = sorted(l, key=lambda x:(x[1].fold_len, x[1].index))
    for unfold, _ in sl:
        for k, e in l2:
            if e.fold[0] == unfold:
                s = "unfold 0x%06x is multi-code fold head in %s" % (unfold, e.fold)
                print >> sys.stderr, s
        for k, e in l3:
            if e.fold[0] == unfold:
                s = "unfold 0x%06x is multi-code fold head in %s" % (unfold, e.fold)
                print >> sys.stderr, s

def make_one_folds(l):
    h = {}
    for unfold, e in l:
        if e.fold_len != 1:
            continue
        fold = e.fold[0]
        unfolds = h.get(fold)
        if unfolds is None:
            unfolds = [unfold]
            h[fold] = unfolds
        else:
            unfolds.append(unfold)

    return h

def make_foldn_heads(l, fold_len, one_folds):
    h = {}
    for unfold, e in l:
        if e.fold_len != fold_len:
            continue
        unfolds = one_folds.get(e.fold[0])
        h[e.fold[0]] = (e, unfolds)

    return h

def fold2_expansion_num(e, one_folds):
    n = len(e.unfolds)
    n0 = 1
    u0 = one_folds.get(e.fold[0])
    if u0 is not None:
        n0 += len(u0)
    n1 = 1
    u1 = one_folds.get(e.fold[1])
    if u1 is not None:
        n1 += len(u1)
    n += (n0 * n1)
    return n

def fold3_expansion_num(e, one_folds):
    n = len(e.unfolds)
    n0 = 1
    u0 = one_folds.get(e.fold[0])
    if u0 is not None:
        n0 += len(u0)
    n1 = 1
    u1 = one_folds.get(e.fold[1])
    if u1 is not None:
        n1 += len(u1)
    n2 = 1
    u2 = one_folds.get(e.fold[2])
    if u2 is not None:
        n2 += len(u2)
    n += (n0 * n1 * n2)
    return n

def get_all_folds_expansion_num(x, one_folds, fold2_heads, fold3_heads):
    e = UNFOLDS[x]
    n = 0
    if e.fold_len == 1:
        n1 = len(e.unfolds) + 1 # +1: fold
        fx = e.fold[0]
        r = fold2_heads.get(fx)
        n2 = n3 = 0
        if r is not None:
            e2, _ = r
            n2 = fold2_expansion_num(e2, one_folds)
        r = fold3_heads.get(fx)
        if r is not None:
            e3, _ = r
            n3 = fold3_expansion_num(e3, one_folds)
        n = max(n1, n2, n3)
    elif e.fold_len == 2:
        n = fold2_expansion_num(e, one_folds)
    elif e.fold_len == 3:
        n = fold3_expansion_num(e, one_folds)
    else:
        raise RuntimeError("Invalid fold_len %d" % (e.fold_len))

    return n

def get_all_folds_expansion_max_num():
    l = UNFOLDS.items()
    one_folds = make_one_folds(l)
    fold2_heads = make_foldn_heads(l, 2, one_folds)
    fold3_heads = make_foldn_heads(l, 3, one_folds)
    sl = sorted(l, key=lambda x:(x[1].fold_len, x[1].index))
    nmax = 0
    max_unfold = None
    for unfold, e in sl:
        n = get_all_folds_expansion_num(unfold, one_folds, fold2_heads, fold3_heads)
        if nmax < n:
            nmax = n
            max_unfold = unfold

    return (nmax, max_unfold)

## main ##
with open(SOURCE_FILE, 'r') as f:
    parse_file(f)

make_locale()

out_comment = True
output_fold_source(sys.stdout, out_comment)

output_gperf_source()

#unfolds_byte_length_check('utf-8')
#unfolds_byte_length_check('utf-16')
double_fold_check()
unfold_is_multi_code_folds_head_check()

#max_num, max_code = get_all_folds_expansion_max_num()
#max_num -= 1  # remove self
#print >> sys.stderr, "max expansion: 0x%06x: %d" % (max_code, max_num)
