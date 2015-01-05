--TEST--
Test strnatcmp() function : basic functionality
--FILE--
<?php
/* Prototype  : int strnatcmp  ( string $str1  , string $str2  )
 * Description: String comparisons using a "natural order" algorithm
 * Source code: ext/standard/string.c
*/
echo "*** Testing strnatcmp() : basic functionality ***\n";

$a1 = "abc1";
$b1 = "abc10";
$c1 = "abc15";
$d1 = "abc2";

$a2 = "ABC1";
$b2 = "ABC10";
$c2 = "ABC15";
$d2 = "ABC2";

echo "Less than tests\n";
var_dump(strnatcmp($a1, $b1));
var_dump(strnatcmp($a1, $c1));
var_dump(strnatcmp($a1, $d1));
var_dump(strnatcmp($b1, $c1));
var_dump(strnatcmp($d1, $c1));

var_dump(strnatcmp($a1, $b2));
var_dump(strnatcmp($a1, $c2));
var_dump(strnatcmp($a1, $d2));
var_dump(strnatcmp($b1, $c2));
var_dump(strnatcmp($d1, $c2));


echo "Equal too tests\n";
var_dump(strnatcmp($b1, $b1));
var_dump(strnatcmp($b1, $b2));	

echo "Greater than tests\n";
var_dump(strnatcmp($b1, $a1));
var_dump(strnatcmp($c1, $a1));
var_dump(strnatcmp($d1, $a1));
var_dump(strnatcmp($c1, $b1));
var_dump(strnatcmp($c1, $d1));

var_dump(strnatcmp($b1, $a2));
var_dump(strnatcmp($c1, $a2));
var_dump(strnatcmp($d1, $a2));
var_dump(strnatcmp($c1, $b2));
var_dump(strnatcmp($c1, $d2));
?>
===DONE===
--EXPECT--
*** Testing strnatcmp() : basic functionality ***
Less than tests
int(-1)
int(-1)
int(-1)
int(-1)
int(-1)
int(1)
int(1)
int(1)
int(1)
int(1)
Equal too tests
int(0)
int(1)
Greater than tests
int(1)
int(1)
int(1)
int(1)
int(1)
int(1)
int(1)
int(1)
int(1)
int(1)
===DONE===