--TEST--
GH-13142 (Undefined variable name is shortened when contains \0)
--FILE--
<?php

$a = "test\0test";
$$a;
$a = "\0test";
$$a;
$a = "test\0";
$$a;

compact("a\0b");
compact("\0ab");
compact("ab\0");

?>
--EXPECTF--
Warning: Undefined variable $test%0test in %s on line %d

Warning: Undefined variable $%0test in %s on line %d

Warning: Undefined variable $test%0 in %s on line %d

Warning: compact(): Undefined variable $a%0b in %s on line %d

Warning: compact(): Undefined variable $%0ab in %s on line %d

Warning: compact(): Undefined variable $ab%0 in %s on line %d
