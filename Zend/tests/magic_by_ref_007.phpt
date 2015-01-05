--TEST--
passing second parameter of __call() by ref
--FILE--
<?php

class test {
    function __call($name, &$args) { }
}

$t = new test;
$func = "foo";
$arg = 1;

$t->$func($arg);

echo "Done\n";
?>
--EXPECTF--	
Fatal error: Method test::__call() cannot take arguments by reference in %s on line %d
