--TEST--
First Class Callable from Closure
--FILE--
<?php
$foo = function(){};
$bar = $foo(...);

if ($foo === $bar) {
    echo "OK";
}
?>
--EXPECT--
OK
