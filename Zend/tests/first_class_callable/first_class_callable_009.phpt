--TEST--
First Class Callable from Closure::__invoke
--FILE--
<?php
$closure = function() {
    return "OK";
};

$foo = $closure->__invoke(...);

echo $foo();
?>
--EXPECT--
OK
