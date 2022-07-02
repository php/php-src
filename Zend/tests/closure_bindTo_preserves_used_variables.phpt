--TEST--
Closure::bindTo() should preserve used variables
--FILE--
<?php

$var = 0;
$fn = function() use($var) {
    var_dump($var);
};
$fn();
$fn = $fn->bindTo(null, null);
$fn();

?>
--EXPECT--
int(0)
int(0)
