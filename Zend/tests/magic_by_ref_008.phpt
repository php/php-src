--TEST--
passing first parameter of __callstatic() by ref
--FILE--
<?php

class test {
    static function __callstatic(&$name, $args) { }
}

$t = new test;
$func = "foo";

$t->$func();

echo "Done\n";
?>
--EXPECTF--
Fatal error: Method test::__callstatic() cannot take arguments by reference in %s on line %d
