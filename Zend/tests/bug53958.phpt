--TEST--
Bug #53958 (Closures can't 'use' shared variables by value and by reference)
--FILE--
<?php
// TEST 1
$a = 1;
$fn1 = function() use ($a) {echo "$a\n"; $a++;};
$fn2 = function() use ($a) {echo "$a\n"; $a++;};
$a = 5;
$fn1(); // 1
$fn2(); // 1
$fn1(); // 1
$fn2(); // 1

// TEST 2
$b = 1;
$fn1 = function() use (&$b) {echo "$b\n"; $b++;};
$fn2 = function() use (&$b) {echo "$b\n"; $b++;};
$b = 5;
$fn1(); // 5
$fn2(); // 6
$fn1(); // 7
$fn2(); // 8

// TEST 3
$c = 1;
$fn1 = function() use (&$c) {echo "$c\n"; $c++;};
$fn2 = function() use ($c) {echo "$c\n"; $c++;};
$c = 5;
$fn1(); // 5
$fn2(); // 1
$fn1(); // 6
$fn2(); // 1

// TEST 4
$d = 1;
$fn1 = function() use ($d) {echo "$d\n"; $d++;};
$fn2 = function() use (&$d) {echo "$d\n"; $d++;};
$d = 5;
$fn1(); // 1
$fn2(); // 5
$fn1(); // 1
$fn2(); // 6
?>
--EXPECT--
1
1
1
1
5
6
7
8
5
1
6
1
1
5
1
6
