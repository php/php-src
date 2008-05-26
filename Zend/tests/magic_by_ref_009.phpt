--TEST--
passing second parameter of __callstatic() by ref
--FILE--
<?php

class test {
    function __callstatic($name, &$args) { }
}

$t = new test;
$func = "foo";
$arg = 1;

$t->$func($arg);

echo "Done\n";
?>
--EXPECTF--
Fatal error: Method test::__callstatic() cannot take arguments by reference in %s on line %d
