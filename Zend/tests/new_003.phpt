--TEST--
Create a class instance from an expression
--FILE--
<?php

class Foo {
}

$oo = 'oo';

$foo = new {'f' . $oo};

var_dump(is_object($foo));
echo get_class($foo) . PHP_EOL;

echo 'Done' . PHP_EOL;
?>
--EXPECTF--	
bool(true)
Foo
Done
