--TEST--
Test strtolower() function
--SKIPIF--
<?php
if( substr(PHP_OS, 0, 3) == 'WIN') {
  if (!setlocale(LC_ALL, 'C')) {
    die('skip need "C" locale (this windows is broken)');
  }
} else {
  if (!setlocale(LC_ALL, 'en_US.UTF-8', 'en')) {
    die('skip need "en_US.UTF-8" locale');
  }
}
?>
--FILE--
<?php
if( substr(PHP_OS, 0, 3) == 'WIN') {
  setlocale(LC_ALL, 'C');
} else {
  setlocale(LC_ALL, 'en_US.UTF-8');
}

echo "*** Testing strtolower() with 128 chars ***\n";
for ($i=0; $i<=127; $i++){
  $char = chr($i);
  print(bin2hex($char))." => ".(bin2hex(strtolower("$char")))."\n";
}

echo "*** Testing strlower() with basic strings ***\n";
$str = "Mary Had A liTTle LAmb and ShE loveD IT So\n";
var_dump(strtolower($str));

echo "\n*** Testing strtolower() with various strings ***";
/* strings to pass strtolower() */
$strings = array (
  "",
  "string",
  "stRINg0234",
  "1.233.344StrinG12333",
  "$$$$$$!!!!@@@@@@@ ABCDEF !!!***",
  "ABCD\0abcdABCD",
  TRUE,
  FALSE,
);

$count = 0;
/* loop through to check possible variations */
foreach ($strings as $string) {
  echo "\n-- Iteration $count --\n";
  var_dump( strtolower($string) );
  $count++;
}

echo "\n*** Testing strtolower() with two different case strings ***\n";
if (strtolower("HeLLo woRLd") === strtolower("hEllo WORLD"))
  echo "strings are same, with Case Insensitive\n";
else
  echo "strings are not same\n";

echo "*** Done ***";
?>
--EXPECTF--
*** Testing strtolower() with 128 chars ***
00 => 00
01 => 01
02 => 02
03 => 03
04 => 04
05 => 05
06 => 06
07 => 07
08 => 08
09 => 09
0a => 0a
0b => 0b
0c => 0c
0d => 0d
0e => 0e
0f => 0f
10 => 10
11 => 11
12 => 12
13 => 13
14 => 14
15 => 15
16 => 16
17 => 17
18 => 18
19 => 19
1a => 1a
1b => 1b
1c => 1c
1d => 1d
1e => 1e
1f => 1f
20 => 20
21 => 21
22 => 22
23 => 23
24 => 24
25 => 25
26 => 26
27 => 27
28 => 28
29 => 29
2a => 2a
2b => 2b
2c => 2c
2d => 2d
2e => 2e
2f => 2f
30 => 30
31 => 31
32 => 32
33 => 33
34 => 34
35 => 35
36 => 36
37 => 37
38 => 38
39 => 39
3a => 3a
3b => 3b
3c => 3c
3d => 3d
3e => 3e
3f => 3f
40 => 40
41 => 61
42 => 62
43 => 63
44 => 64
45 => 65
46 => 66
47 => 67
48 => 68
49 => 69
4a => 6a
4b => 6b
4c => 6c
4d => 6d
4e => 6e
4f => 6f
50 => 70
51 => 71
52 => 72
53 => 73
54 => 74
55 => 75
56 => 76
57 => 77
58 => 78
59 => 79
5a => 7a
5b => 5b
5c => 5c
5d => 5d
5e => 5e
5f => 5f
60 => 60
61 => 61
62 => 62
63 => 63
64 => 64
65 => 65
66 => 66
67 => 67
68 => 68
69 => 69
6a => 6a
6b => 6b
6c => 6c
6d => 6d
6e => 6e
6f => 6f
70 => 70
71 => 71
72 => 72
73 => 73
74 => 74
75 => 75
76 => 76
77 => 77
78 => 78
79 => 79
7a => 7a
7b => 7b
7c => 7c
7d => 7d
7e => 7e
7f => 7f
*** Testing strlower() with basic strings ***
string(43) "mary had a little lamb and she loved it so
"

*** Testing strtolower() with various strings ***
-- Iteration 0 --
string(0) ""

-- Iteration 1 --
string(6) "string"

-- Iteration 2 --
string(10) "string0234"

-- Iteration 3 --
string(20) "1.233.344string12333"

-- Iteration 4 --
string(31) "$$$$$$!!!!@@@@@@@ abcdef !!!***"

-- Iteration 5 --
string(13) "abcd%0abcdabcd"

-- Iteration 6 --
string(1) "1"

-- Iteration 7 --
string(0) ""

*** Testing strtolower() with two different case strings ***
strings are same, with Case Insensitive
*** Done ***
