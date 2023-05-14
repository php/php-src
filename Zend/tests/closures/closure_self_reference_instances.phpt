--TEST--
Testing Closure self-reference functionality: Closure instances
--FILE--
<?php

$map = new WeakMap();

$fn = function() as $fn use ($map) {
    var_dump(isset($map[$fn]));
};
$map[$fn] = 1;
$fn();
$fn->call(new class {});
$fn();

var_dump(count($map));
unset($fn);
var_dump(count($map));

?>
--EXPECT--
bool(true)
bool(false)
bool(true)
int(1)
int(0)
