--TEST--
passing parameter of __get() by ref
--FILE--
<?php

class test {
    function __get(&$name) { }
}

$t = new test;
$name = "prop";
var_dump($t->$name);

echo "Done\n";
?>
--EXPECTF--	
Fatal error: Method test::__get() cannot take arguments by reference in %s on line %d
