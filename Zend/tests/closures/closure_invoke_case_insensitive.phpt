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
--EXPECTF--
Fatal error: Uncaught Error: Call to undefined method Closure::__INVOKE() in %s:%d
Stack trace:
#%d {main}
  thrown in %s on line %d
