--TEST--
Bug #70083 (Use after free with assign by ref to overloaded objects)
--FILE--
<?php

class foo {
	private $var;
	function __get($e) {
		return $this;
	}
}

function &noref() { $foo = 1; return $foo; }

$foo = new foo;
try {
    $foo->i = &noref();
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
var_dump($foo);

?>
--EXPECT--
Cannot assign by reference to overloaded object
object(foo)#1 (1) {
  ["var":"foo":private]=>
  NULL
}
