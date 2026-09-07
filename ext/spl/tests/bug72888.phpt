--TEST--
Bug #72888 (Segfault on clone on splFileObject)
--FILE--
<?php
$x = new SplFileObject(__FILE__);

try {
    $y=clone $x;
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
var_dump($y);
?>
--EXPECTF--
Error: Trying to clone an uncloneable object of class SplFileObject

Warning: Undefined variable $y in %s on line %d
NULL
