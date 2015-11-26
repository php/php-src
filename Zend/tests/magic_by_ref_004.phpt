--TEST--
passing parameter of __unset() by ref
--FILE--
<?php

class test {
    function __unset(&$name) { }
}

$t = new test;
$name = "prop";

var_dump($t->$name);

echo "Done\n";
?>
--EXPECTF--	
Fatal error: Method test::__unset() cannot take arguments by reference in %s on line %d
