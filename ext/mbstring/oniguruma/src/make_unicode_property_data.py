#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# make_unicode_property_data.py
# Copyright (c) 2016-2024  K.Kosako

import sys
import re

POSIX_LIST = [
  'NEWLINE', 'Alpha', 'Blank', 'Cntrl', 'Digit', 'Graph', 'Lower',
  'Print', 'PosixPunct', 'Space', 'Upper', 'XDigit', 'Word', 'Alnum',
  'ASCII'
]

MAX_CODE_POINT = 0x10ffff

GRAPHEME_CLUSTER_BREAK_NAME_PREFIX = 'Grapheme_Cluster_Break_'

UD_FIRST_REG = re.compile("<.+,\s*First>")
UD_LAST_REG  = re.compile("<.+,\s*Last>")
PR_TOTAL_REG = re.compile("#\s*Total\s+(?:code\s+points|elements):")
PR_LINE_REG  = re.compile("([0-9A-Fa-f]+)(?:..([0-9A-Fa-f]+))?\s*;\s*(\w+)")
PA_LINE_REG  = re.compile("(\w+)\s*;\s*(\w+)")
PVA_LINE_REG = re.compile("(sc|gc)\s*;\s*(\w+)\s*;\s*(\w+)(?:\s*;\s*(\w+))?")
BL_LINE_REG  = re.compile("([0-9A-Fa-f]+)\.\.([0-9A-Fa-f]+)\s*;\s*(.*)")
UNICODE_VERSION_REG = re.compile("#\s*.*-(\d+)\.(\d+)\.(\d+)\.txt")
EMOJI_VERSION_REG   = re.compile("(?i)#.+Version\s+(\d+)\.(\d+)")

VERSION_INFO = [-1, -1, -1]
EMOJI_VERSION_INFO = [-1, -1]

DIC  = { }
KDIC = { }
PropIndex = { }
PROPERTY_NAME_MAX_LEN = 0
PROPS = None

def normalize_prop_name(name):
  name = re.sub(r'[ _]', '', name)
  name = name.lower()
  return name

def fix_block_name(name):
  s = re.sub(r'[- ]+', '_', name)
  return 'In_' + s

def print_ranges(ranges):
  for (start, end) in ranges:
    print("0x%06x, 0x%06x" % (start, end))

  print(len(ranges))

def print_prop_and_index(prop, i):
  print("%-35s %3d" % (prop + ',', i))
  PropIndex[prop] = i

PRINT_CACHE = { }

def print_property(prop, data, desc):
  print('')
  print("/* PROPERTY: '%s': %s */" % (prop, desc))

  prev_prop = dic_find_by_value(PRINT_CACHE, data)
  if prev_prop is not None:
    print("#define CR_%s CR_%s" % (prop, prev_prop))
  else:
    PRINT_CACHE[prop] = data
    print("static const OnigCodePoint")
    print("CR_%s[] = { %d," % (prop, len(data)))
    for (start, end) in data:
      print("0x%04x, 0x%04x," % (start, end))

    print("}; /* END of CR_%s */" % prop)


def dic_find_by_value(dic, v):
  for key, val in dic.items():
    if val == v:
      return key

  return None

def make_reverse_dic(dic):
  rev = {}
  for key, val in dic.items():
    d = rev.get(val, None)
    if d is None:
      rev[val] = [key]
    else:
      d.append(key)

  return rev

def normalize_ranges(in_ranges, sort=False):
  if sort:
    ranges = sorted(in_ranges)
  else:
    ranges = in_ranges

  r = []
  prev = None
  for (start, end) in ranges:
    if prev is not None and prev >= start - 1:
      (pstart, pend) = r.pop()
      end = max(pend, end)
      start = pstart

    r.append((start, end))
    prev = end

  return r

def inverse_ranges(in_ranges):
  r = []
  prev = 0x000000
  for (start, end) in in_ranges:
    if prev < start:
      r.append((prev, start - 1))

    prev = end + 1

  if prev < MAX_CODE_POINT:
    r.append((prev, MAX_CODE_POINT))

  return r

def add_ranges(r1, r2):
  r = r1 + r2
  return normalize_ranges(r, True)

def sub_one_range(one_range, rs):
  r = []
  (s1, e1) = one_range
  n = len(rs)
  for i in range(0, n):
    (s2, e2) = rs[i]
    if s2 >= s1 and s2 <= e1:
      if s2 > s1:
        r.append((s1, s2 - 1))
      if e2 >= e1:
        return r

      s1 = e2 + 1
    elif s2 < s1 and e2 >= s1:
      if e2 < e1:
        s1 = e2 + 1
      else:
        return r

  r.append((s1, e1))
  return r

def sub_ranges(r1, r2):
  r = []
  for one_range in r1:
    rs = sub_one_range(one_range, r2)
    r.extend(rs)

  return r

def add_ranges_in_dic(dic):
  r = []
  for k, v in dic.items():
    r = r + v

  return normalize_ranges(r, True)

def normalize_ranges_in_dic(dic, sort=False):
  for k, v in dic.items():
    r = normalize_ranges(v, sort)
    dic[k] = r

def merge_dic(to_dic, from_dic):
  to_keys   = to_dic.keys()
  from_keys = from_dic.keys()
  common = list(set(to_keys) & set(from_keys))
  if len(common) != 0:
    print("merge_dic: collision: %s" % sorted(common), file=sys.stderr)

  to_dic.update(from_dic)

def merge_props(to_dic, from_dic):
  to_keys   = to_dic.keys()
  from_keys = from_dic.keys()
  common = list(set(to_keys) & set(from_keys))
  if len(common) != 0:
    print("merge_props: collision: %s" % sorted(common), file=sys.stderr)

  for k in from_keys:
    to_dic[k] = True

def add_range_into_dic(dic, name, start, end):
  d = dic.get(name, None)
  if d is None:
    d = [(start, end)]
    dic[name] = d
  else:
    d.append((start, end))

def list_sub(a, b):
  x = set(a) - set(b)
  return list(x)


def parse_unicode_data_file(f):
  dic = { }
  assigned = []
  for line in f:
    s = line.strip()
    if len(s) == 0:
      continue
    if s[0] == '#':
      continue

    a = s.split(';')
    code = int(a[0], 16)
    desc = a[1]
    prop = a[2]
    if UD_FIRST_REG.match(desc) is not None:
      start = code
      end   = None
    elif UD_LAST_REG.match(desc) is not None:
      end = code
    else:
      start = end = code

    if end is not None:
      assigned.append((start, end))
      add_range_into_dic(dic, prop, start, end)
      if len(prop) == 2:
        add_range_into_dic(dic, prop[0:1], start, end)

  normalize_ranges_in_dic(dic)
  return dic, assigned

def parse_properties(path, klass, prop_prefix = None, version_reg = None):
  version_match = None
  with open(path, 'r') as f:
    dic = { }
    prop = None
    for line in f:
      s = line.strip()
      if len(s) == 0:
        continue

      if s[0] == '#' and version_reg is not None and version_match is None:
        version_match = version_reg.match(s)
        if version_match is not None:
          continue

      m = PR_LINE_REG.match(s)
      if m:
        prop = m.group(3)
        if prop_prefix is not None:
          prop = prop_prefix + prop

        if m.group(2):
          start = int(m.group(1), 16)
          end   = int(m.group(2), 16)
          add_range_into_dic(dic, prop, start, end)
        else:
          start = int(m.group(1), 16)
          add_range_into_dic(dic, prop, start, start)

      elif PR_TOTAL_REG.match(s) is not None:
        KDIC[prop] = klass

  normalize_ranges_in_dic(dic)
  return (dic, version_match)

def parse_property_aliases(path):
  a = { }
  with open(path, 'r') as f:
    for line in f:
      s = line.strip()
      if len(s) == 0:
        continue

      m = PA_LINE_REG.match(s)
      if not(m):
        continue

      if m.group(1) == m.group(2):
        continue

      a[m.group(1)] = m.group(2)

  return a

def parse_property_value_aliases(path):
  a = { }
  with open(path, 'r') as f:
    for line in f:
      s = line.strip()
      if len(s) == 0:
        continue

      m = PVA_LINE_REG.match(s)
      if not(m):
        continue

      cat = m.group(1)
      x2  = m.group(2)
      x3  = m.group(3)
      x4  = m.group(4)
      if cat == 'sc':
        if x2 != x3:
          a[x2] = x3
        if x4 and x4 != x3:
          a[x4] = x3
      else:
        if x2 != x3:
          a[x3] = x2
        if x4 and x4 != x2:
          a[x4] = x2

  return a

def parse_blocks(path):
  dic = { }
  blocks = []
  with open(path, 'r') as f:
    for line in f:
      s = line.strip()
      if len(s) == 0:
        continue

      m = BL_LINE_REG.match(s)
      if not(m):
        continue

      start = int(m.group(1), 16)
      end   = int(m.group(2), 16)
      block = fix_block_name(m.group(3))
      add_range_into_dic(dic, block, start, end)
      blocks.append(block)

  noblock = fix_block_name('No_Block')
  dic[noblock] = inverse_ranges(add_ranges_in_dic(dic))
  blocks.append(noblock)
  return dic, blocks

def add_primitive_props(assigned):
  DIC['Assigned'] = normalize_ranges(assigned)
  DIC['Any']     = [(0x000000, 0x10ffff)]
  DIC['ASCII']   = [(0x000000, 0x00007f)]
  DIC['NEWLINE'] = [(0x00000a, 0x00000a)]
  DIC['Cn'] = inverse_ranges(DIC['Assigned'])
  DIC['C'].extend(DIC['Cn'])
  DIC['C'] = normalize_ranges(DIC['C'], True)

  d = []
  d.extend(DIC['Ll'])
  d.extend(DIC['Lt'])
  d.extend(DIC['Lu'])
  DIC['LC'] = normalize_ranges(d, True)

def add_posix_props(dic):
  alnum = []
  alnum.extend(dic['Alphabetic'])
  alnum.extend(dic['Nd'])  # Nd == Decimal_Number
  alnum = normalize_ranges(alnum, True)

  blank = [(0x0009, 0x0009)]
  blank.extend(dic['Zs'])  # Zs == Space_Separator
  blank = normalize_ranges(blank, True)

  word = []
  word.extend(dic['Alphabetic'])
  word.extend(dic['M'])   # M == Mark
  word.extend(dic['Nd'])
  word.extend(dic['Pc'])  # Pc == Connector_Punctuation
  word = normalize_ranges(word, True)

  graph = sub_ranges(dic['Any'], dic['White_Space'])
  graph = sub_ranges(graph, dic['Cc'])
  graph = sub_ranges(graph, dic['Cs'])  # Cs == Surrogate
  graph = sub_ranges(graph, dic['Cn'])  # Cn == Unassigned
  graph = normalize_ranges(graph, True)

  p = []
  p.extend(graph)
  p.extend(dic['Zs'])
  p = normalize_ranges(p, True)

  dic['Alpha']  = dic['Alphabetic']
  dic['Upper']  = dic['Uppercase']
  dic['Lower']  = dic['Lowercase']
  dic['PosixPunct'] = add_ranges(dic['P'], dic['S'])  # P == Punctuation
  dic['Digit']  = dic['Nd']
  dic['XDigit'] = [(0x0030, 0x0039), (0x0041, 0x0046), (0x0061, 0x0066)]
  dic['Alnum']  = alnum
  dic['Space']  = dic['White_Space']
  dic['Blank']  = blank
  dic['Cntrl']  = dic['Cc']
  dic['Word']   = word
  dic['Graph']  = graph
  dic['Print']  = p


def set_max_prop_name(name):
  global PROPERTY_NAME_MAX_LEN
  n = len(name)
  if n > PROPERTY_NAME_MAX_LEN:
    PROPERTY_NAME_MAX_LEN = n

def entry_prop_name(name, index):
  set_max_prop_name(name)
  if OUTPUT_LIST_MODE and index >= len(POSIX_LIST):
    print("%s" % (name), file=UPF)

def entry_and_print_prop_and_index(name, index):
  entry_prop_name(name, index)
  nname = normalize_prop_name(name)
  print_prop_and_index(nname, index)

def parse_and_merge_properties(path, klass, prop_prefix = None, version_reg = None):
  dic, ver_m = parse_properties(path, klass, prop_prefix, version_reg)
  merge_dic(DIC, dic)
  merge_props(PROPS, dic)
  return dic, ver_m


### main ###
argv = sys.argv
argc = len(argv)

COPYRIGHT = '''
/*-
 * Copyright (c) 2016-2024  K.Kosako
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

POSIX_ONLY = False
INCLUDE_GRAPHEME_CLUSTER_DATA = False

for i in range(1, argc):
  arg = argv[i]
  if arg == '-posix':
    POSIX_ONLY = True
  elif arg == '-gc':
    INCLUDE_GRAPHEME_CLUSTER_DATA = True
  else:
    print("Invalid argument: %s" % arg, file=sys.stderr)


OUTPUT_LIST_MODE = not(POSIX_ONLY)

with open('UnicodeData.txt', 'r') as f:
  dic, assigned = parse_unicode_data_file(f)
  DIC = dic
  add_primitive_props(assigned)

PROPS = DIC.fromkeys(DIC, True)
PROPS = {k: v for k, v in PROPS.items() if k not in POSIX_LIST}

_, ver_m = parse_and_merge_properties('DerivedCoreProperties.txt', 'Derived Property', None, UNICODE_VERSION_REG)
if ver_m is not None:
  VERSION_INFO[0] = int(ver_m.group(1))
  VERSION_INFO[1] = int(ver_m.group(2))
  VERSION_INFO[2] = int(ver_m.group(3))

dic, _ = parse_and_merge_properties('Scripts.txt', 'Script')
DIC['Unknown'] = inverse_ranges(add_ranges_in_dic(dic))

parse_and_merge_properties('PropList.txt',   'Binary Property')

_, ver_m = parse_and_merge_properties('emoji-data.txt', 'Emoji Property', None, EMOJI_VERSION_REG)
if ver_m is not None:
  EMOJI_VERSION_INFO[0] = int(ver_m.group(1))
  EMOJI_VERSION_INFO[1] = int(ver_m.group(2))

PROPS['Unknown'] = True
KDIC['Unknown'] = 'Script'

ALIASES = parse_property_aliases('PropertyAliases.txt')
a = parse_property_value_aliases('PropertyValueAliases.txt')
merge_dic(ALIASES, a)

dic, BLOCKS = parse_blocks('Blocks.txt')
merge_dic(DIC, dic)

if INCLUDE_GRAPHEME_CLUSTER_DATA:
  dic, _ = parse_properties('GraphemeBreakProperty.txt',
                            'GraphemeBreak Property',
                            GRAPHEME_CLUSTER_BREAK_NAME_PREFIX)
  merge_dic(DIC, dic)
  merge_props(PROPS, dic)
  #prop = GRAPHEME_CLUSTER_BREAK_NAME_PREFIX + 'Other'
  #DIC[prop] = inverse_ranges(add_ranges_in_dic(dic))
  #PROPS[prop] = True
  #KDIC[prop] = 'GrapemeBreak Property'

add_posix_props(DIC)
PROP_LIST = sorted(PROPS.keys())


s = '''%{
/* Generated by make_unicode_property_data.py. */
'''
print(s)
print(COPYRIGHT)
print('')

for prop in POSIX_LIST:
  if prop == 'PosixPunct':
    desc = "POSIX [[:punct:]]"
  else:
    desc = "POSIX [[:%s:]]" % prop

  print_property(prop, DIC[prop], desc)

print('')

if not(POSIX_ONLY):
  for prop in PROP_LIST:
    klass = KDIC.get(prop, None)
    if klass is None:
      n = len(prop)
      if n == 1:
        klass = 'Major Category'
      elif n == 2:
        klass = 'General Category'
      else:
        klass = '-'

    print_property(prop, DIC[prop], klass)

  for block in BLOCKS:
    print_property(block, DIC[block], 'Block')


print('')
print("static const OnigCodePoint*\nconst CodeRanges[] = {")

for prop in POSIX_LIST:
  print("  CR_%s," % prop)

if not(POSIX_ONLY):
  for prop in PROP_LIST:
    print("  CR_%s," % prop)

  for prop in BLOCKS:
    print("  CR_%s," % prop)

s = '''};

#define pool_offset(s) offsetof(struct unicode_prop_name_pool_t, unicode_prop_name_pool_str##s)

%}
struct PoolPropertyNameCtype {
  short int name;
  short int ctype;
};

%%
'''
sys.stdout.write(s)

if OUTPUT_LIST_MODE:
  UPF = open("UNICODE_PROPERTIES", "w")
  if VERSION_INFO[0] < 0:
    raise RuntimeError("Unicode Version is not found")
  if EMOJI_VERSION_INFO[0] < 0:
    raise RuntimeError("Emoji Version is not found")

  print("Unicode Properties (Unicode Version: %d.%d.%d,  Emoji: %d.%d)" % (VERSION_INFO[0], VERSION_INFO[1], VERSION_INFO[2], EMOJI_VERSION_INFO[0], EMOJI_VERSION_INFO[1]), file=UPF)
  print('', file=UPF)

index = -1
for prop in POSIX_LIST:
  index += 1
  entry_and_print_prop_and_index(prop, index)

if not(POSIX_ONLY):
  for prop in PROP_LIST:
    index += 1
    entry_and_print_prop_and_index(prop, index)

  NALIASES = map(lambda x:(normalize_prop_name(x[0]), x[0], x[1]), ALIASES.items())
  NALIASES = sorted(NALIASES)
  for (nk, k, v) in NALIASES:
    nv = normalize_prop_name(v)
    if PropIndex.get(nk, None) is not None:
      print("ALIASES: already exists: %s => %s" % (k, v), file=sys.stderr)
      continue
    aindex = PropIndex.get(nv, None)
    if aindex is None:
      #print("ALIASES: value is not exist: %s => %s" % (k, v), file=sys.stderr)
      continue

    entry_prop_name(k, aindex)
    print_prop_and_index(nk, aindex)

  for name in BLOCKS:
    index += 1
    entry_and_print_prop_and_index(name, index)

print('%%')
print('')
if not(POSIX_ONLY):
  if VERSION_INFO[0] < 0:
    raise RuntimeError("Unicode Version is not found")
  if EMOJI_VERSION_INFO[0] < 0:
    raise RuntimeError("Emoji Version is not found")

  print("#define UNICODE_PROPERTY_VERSION  %02d%02d%02d" % (VERSION_INFO[0], VERSION_INFO[1], VERSION_INFO[2]))
  print("#define UNICODE_EMOJI_VERSION     %02d%02d" % (EMOJI_VERSION_INFO[0], EMOJI_VERSION_INFO[1]))
  print('')

print("#define PROPERTY_NAME_MAX_SIZE  %d" % (PROPERTY_NAME_MAX_LEN + 10))
print("#define CODE_RANGES_NUM         %d" % (index + 1))

index_props = make_reverse_dic(PropIndex)
print('')
for i in range(index + 1):
  for p in index_props[i]:
    print("#define PROP_INDEX_%s %d" % (p.upper(), i))

if OUTPUT_LIST_MODE:
  UPF.close()

sys.exit(0)
