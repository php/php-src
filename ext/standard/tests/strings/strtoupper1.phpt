--TEST--
Test strtoupper() function
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

echo "*** Testing strtoupper() with 128 chars ***\n";
for ($i=0; $i<=127; $i++){
  $char = chr($i);
  print(bin2hex($char))." => ".(bin2hex(strtoupper("$char")))."\n";
}

echo "\n*** Testing strtoupper() with basic strings ***\n";
$str = "Mary Had A liTTle LAmb and ShE loveD IT So\n";
var_dump(strtoupper($str));

echo "\n*** Testing strtoupper() with various strings ***";
/* strings to pass strtoupper() */
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
  var_dump( strtoupper($string) );
  $count++;
}

echo "\n*** Testing strtoupper() with two different case strings ***\n";
if (strtoupper("HeLLo woRLd") === strtoupper("hEllo WORLD"))
  echo "strings are same, with Case Insensitive\n";
else
  echo "strings are not same\n";

echo "*** Done ***";
?>
--EXPECTF--
*** Testing strtoupper() with 128 chars ***
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
41 => 41
42 => 42
43 => 43
44 => 44
45 => 45
46 => 46
47 => 47
48 => 48
49 => 49
4a => 4a
4b => 4b
4c => 4c
4d => 4d
4e => 4e
4f => 4f
50 => 50
51 => 51
52 => 52
53 => 53
54 => 54
55 => 55
56 => 56
57 => 57
58 => 58
59 => 59
5a => 5a
5b => 5b
5c => 5c
5d => 5d
5e => 5e
5f => 5f
60 => 60
61 => 41
62 => 42
63 => 43
64 => 44
65 => 45
66 => 46
67 => 47
68 => 48
69 => 49
6a => 4a
6b => 4b
6c => 4c
6d => 4d
6e => 4e
6f => 4f
70 => 50
71 => 51
72 => 52
73 => 53
74 => 54
75 => 55
76 => 56
77 => 57
78 => 58
79 => 59
7a => 5a
7b => 7b
7c => 7c
7d => 7d
7e => 7e
7f => 7f

*** Testing strtoupper() with basic strings ***
string(43) "MARY HAD A LITTLE LAMB AND SHE LOVED IT SO
"

*** Testing strtoupper() with various strings ***
-- Iteration 0 --
string(0) ""

-- Iteration 1 --
string(6) "STRING"

-- Iteration 2 --
string(10) "STRING0234"

-- Iteration 3 --
string(20) "1.233.344STRING12333"

-- Iteration 4 --
string(31) "$$$$$$!!!!@@@@@@@ ABCDEF !!!***"

-- Iteration 5 --
string(13) "ABCD%0ABCDABCD"

-- Iteration 6 --
string(1) "1"

-- Iteration 7 --
string(0) ""

*** Testing strtoupper() with two different case strings ***
strings are same, with Case Insensitive
*** Done ***
