--TEST--
First Class Callable from User
--FILE--
<?php
function foo() {
    return "OK";
}

$foo = foo(...);

echo $foo();
?>
--EXPECT--
OK
