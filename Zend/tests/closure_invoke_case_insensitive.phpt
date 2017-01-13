--TEST--
Closure::__invoke() is case insensitive
--FILE--
<?php

$inc = function(&$n) {
    $n++;
};

$n = 1;
$inc->__INVOKE($n);
var_dump($n);

?>
--EXPECT--
int(2)
