--TEST--
Bug #78238 (BCMath returns "-0")
--EXTENSIONS--
bcmath
--FILE--
<?php
var_dump(bcadd("0", "-0.1"));

$a = bcmul("0.9", "-0.1", 1);
$b = bcmul("0.90", "-0.1", 1);
var_dump($a, $b);
?>
--EXPECT--
string(1) "0"
string(3) "0.0"
string(3) "0.0"
