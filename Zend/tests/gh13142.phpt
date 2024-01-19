--TEST--
GH-13142 (Undefined variable name is shortened when contains \0)
--FILE--
<?php

$a = "test\0test";
$$a;

compact("a\0b");

?>
--EXPECTF--
Warning: Undefined variable $test%0test in %s on line %d

Warning: compact(): Undefined variable $a%0b in %s on line %d
