--TEST--
Test fnmatch() function: Variations
--SKIPIF--
<?php
if (!function_exists('fnmatch'))
    die("skip fnmatch() function is not available");
?>
--FILE--
<?php

echo "*** Testing fnmatch() with file and various patterns ***\n";
$file_name = __DIR__."/match.tmp";

/* avoid using \, it breaks the pattern */
if (substr(PHP_OS, 0, 3) == 'WIN') {
    $file_name = str_replace('\\','/', $file_name);
}

fopen($file_name, "w");

$pattern_arr = array(
0 => "*.tmp",
1 => "match*",
2 => "mat*",
3 => "mat*tmp",
4 => "m*t",
5 => "ma[pt]ch*",
6 => "*.t*",
7 => "***.tmp",
8 => "match**",
9 => "*.t*p",
10 => "",
11 => "match",
12 => ".tmp",
13 => "?match",
14 => "match?tmp",
15 => "?tmp",
16 => "match?",
17 => "?match?",
18 => "match.tmp",
19 => "/match.tmp",
20 => "/match.tmp/",
21 => 'match.tmp',
22 => 'match.tmp\0',
23 => "match.tmp\0",
24 => "match\0.tmp",
25 => chr(109).chr(97)."tch.tmp",
26 => chr(109).chr(97).chr(116).chr(99).chr(104).".tmp",
27 => chr(109).chr(97).chr(116).chr(99).chr(104).chr(46).chr(116).chr(120).chr(116),
28 => chr(109).chr(97).chr(116).chr(99).chr(104).".".chr(116).chr(120).chr(116),
29 => "MATCH.TMP",
30 => "MATCH*",
31 => $file_name,

/* binary inputs */
32 => b"match*",
33 => b"*.tmp",
34 => b"mat*",
35 => b"mat*tmp",
36 => b"m*t",
);

for( $i = 0; $i<count($pattern_arr); $i++ ) {
  echo "-- Iteration $i --\n";
  try {
    var_dump( fnmatch($pattern_arr[$i], $file_name) );
  } catch (Error $e) {
    echo $e->getMessage(), "\n";
  }
}
unlink($file_name);


echo "\n*** Testing fnmatch() with other types other than files ***";

/* defining a common function */
function match_( $pattern, $string ) {
  for( $i = 0; $i<count($pattern); $i++ ) {
    echo "-- Iteration $i --\n";
    for( $j = 0; $j<count($string); $j++ ) {
      try {
        var_dump( fnmatch($pattern[$i], $string[$j]) );
      } catch (Error $e) {
        echo $e->getMessage(), "\n";
      }
    }
  }
}

echo "\n--- With Integers ---\n";
$int_arr = array(
  16,
  16.00,
  020,
  020.00,
  0xF,
  0xF0000
);
match_($int_arr, $int_arr);

echo "\n--- With Strings ---\n";
$str_arr = array(
  "string",
  "string\0",
  'string',
  "str\0ing",
  "stringstring",

  /* binary input */
  b"string"
);
match_($str_arr, $str_arr);

echo "\n--- With booleans ---\n";
$bool_arr = array(
  TRUE,
  true,
  1,
  10,
  FALSE,
  false,
  0,
  "",
  "string"
);
match_($bool_arr, $bool_arr);

echo "\n--- With NULL ---\n";
$null_arr = array(
  "",
  "\0",
  "string",
  0
);
match_($null_arr, $null_arr);

echo "\n*** Done ***\n";
?>
--EXPECT--
*** Testing fnmatch() with file and various patterns ***
-- Iteration 0 --
bool(true)
-- Iteration 1 --
bool(false)
-- Iteration 2 --
bool(false)
-- Iteration 3 --
bool(false)
-- Iteration 4 --
bool(false)
-- Iteration 5 --
bool(false)
-- Iteration 6 --
bool(true)
-- Iteration 7 --
bool(true)
-- Iteration 8 --
bool(false)
-- Iteration 9 --
bool(true)
-- Iteration 10 --
bool(false)
-- Iteration 11 --
bool(false)
-- Iteration 12 --
bool(false)
-- Iteration 13 --
bool(false)
-- Iteration 14 --
bool(false)
-- Iteration 15 --
bool(false)
-- Iteration 16 --
bool(false)
-- Iteration 17 --
bool(false)
-- Iteration 18 --
bool(false)
-- Iteration 19 --
bool(false)
-- Iteration 20 --
bool(false)
-- Iteration 21 --
bool(false)
-- Iteration 22 --
bool(false)
-- Iteration 23 --
fnmatch(): Argument #1 ($pattern) must not contain any null bytes
-- Iteration 24 --
fnmatch(): Argument #1 ($pattern) must not contain any null bytes
-- Iteration 25 --
bool(false)
-- Iteration 26 --
bool(false)
-- Iteration 27 --
bool(false)
-- Iteration 28 --
bool(false)
-- Iteration 29 --
bool(false)
-- Iteration 30 --
bool(false)
-- Iteration 31 --
bool(true)
-- Iteration 32 --
bool(false)
-- Iteration 33 --
bool(true)
-- Iteration 34 --
bool(false)
-- Iteration 35 --
bool(false)
-- Iteration 36 --
bool(false)

*** Testing fnmatch() with other types other than files ***
--- With Integers ---
-- Iteration 0 --
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
bool(false)
-- Iteration 1 --
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
bool(false)
-- Iteration 2 --
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
bool(false)
-- Iteration 3 --
bool(false)
bool(false)
bool(false)
bool(true)
bool(false)
bool(false)
-- Iteration 4 --
bool(false)
bool(false)
bool(false)
bool(false)
bool(true)
bool(false)
-- Iteration 5 --
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(true)

--- With Strings ---
-- Iteration 0 --
bool(true)
fnmatch(): Argument #2 ($filename) must not contain any null bytes
bool(true)
fnmatch(): Argument #2 ($filename) must not contain any null bytes
bool(false)
bool(true)
-- Iteration 1 --
fnmatch(): Argument #1 ($pattern) must not contain any null bytes
fnmatch(): Argument #1 ($pattern) must not contain any null bytes
fnmatch(): Argument #1 ($pattern) must not contain any null bytes
fnmatch(): Argument #1 ($pattern) must not contain any null bytes
fnmatch(): Argument #1 ($pattern) must not contain any null bytes
fnmatch(): Argument #1 ($pattern) must not contain any null bytes
-- Iteration 2 --
bool(true)
fnmatch(): Argument #2 ($filename) must not contain any null bytes
bool(true)
fnmatch(): Argument #2 ($filename) must not contain any null bytes
bool(false)
bool(true)
-- Iteration 3 --
fnmatch(): Argument #1 ($pattern) must not contain any null bytes
fnmatch(): Argument #1 ($pattern) must not contain any null bytes
fnmatch(): Argument #1 ($pattern) must not contain any null bytes
fnmatch(): Argument #1 ($pattern) must not contain any null bytes
fnmatch(): Argument #1 ($pattern) must not contain any null bytes
fnmatch(): Argument #1 ($pattern) must not contain any null bytes
-- Iteration 4 --
bool(false)
fnmatch(): Argument #2 ($filename) must not contain any null bytes
bool(false)
fnmatch(): Argument #2 ($filename) must not contain any null bytes
bool(true)
bool(false)
-- Iteration 5 --
bool(true)
fnmatch(): Argument #2 ($filename) must not contain any null bytes
bool(true)
fnmatch(): Argument #2 ($filename) must not contain any null bytes
bool(false)
bool(true)

--- With booleans ---
-- Iteration 0 --
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
-- Iteration 1 --
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
-- Iteration 2 --
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
-- Iteration 3 --
bool(false)
bool(false)
bool(false)
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
-- Iteration 4 --
bool(false)
bool(false)
bool(false)
bool(false)
bool(true)
bool(true)
bool(false)
bool(true)
bool(false)
-- Iteration 5 --
bool(false)
bool(false)
bool(false)
bool(false)
bool(true)
bool(true)
bool(false)
bool(true)
bool(false)
-- Iteration 6 --
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(true)
bool(false)
bool(false)
-- Iteration 7 --
bool(false)
bool(false)
bool(false)
bool(false)
bool(true)
bool(true)
bool(false)
bool(true)
bool(false)
-- Iteration 8 --
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(true)

--- With NULL ---
-- Iteration 0 --
bool(true)
fnmatch(): Argument #2 ($filename) must not contain any null bytes
bool(false)
bool(false)
-- Iteration 1 --
fnmatch(): Argument #1 ($pattern) must not contain any null bytes
fnmatch(): Argument #1 ($pattern) must not contain any null bytes
fnmatch(): Argument #1 ($pattern) must not contain any null bytes
fnmatch(): Argument #1 ($pattern) must not contain any null bytes
-- Iteration 2 --
bool(false)
fnmatch(): Argument #2 ($filename) must not contain any null bytes
bool(true)
bool(false)
-- Iteration 3 --
bool(false)
fnmatch(): Argument #2 ($filename) must not contain any null bytes
bool(false)
bool(true)

*** Done ***
