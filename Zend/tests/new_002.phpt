--TEST--
Create a class instance from a variable
--FILE--
<?php

class Foo {
}

$fooClass = 'Foo';

$foo = new $fooClass;

var_dump(is_object($foo));
echo get_class($foo) . PHP_EOL;

echo 'Done' . PHP_EOL;
?>
--EXPECTF--	
bool(true)
Foo
Done
