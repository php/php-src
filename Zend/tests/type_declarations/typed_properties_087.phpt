--TEST--
Ensure null-initialization of nullable typed static property taken by reference
--FILE--
<?php

class A {
    public static ?int $a;
}

$x =& A::$a;
var_dump($x);

?>
--EXPECT--
NULL
