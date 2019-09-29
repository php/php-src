--TEST--
Bug #72888 (Segfault on clone on splFileObject)
--FILE--
<?php
$x = new SplFileObject(__FILE__);

try {
	$y=clone $x;
} catch (Error $e) {
	var_dump($e->getMessage());
}
var_dump($y);
?>
--EXPECTF--
string(60) "Trying to clone an uncloneable object of class SplFileObject"

Notice: Undefined variable: y in %sbug72888.php on line %d
NULL
