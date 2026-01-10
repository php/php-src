--TEST--
Fix for #77627 method_exists on Closure::__invoke without object returns false
--FILE--
<?php
var_dump(method_exists(Closure::class, "__invoke"));
var_dump(method_exists(Closure::class, "__INVOKE"));

$closure = function(){};

var_dump(method_exists($closure, "__INVOKE"));
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
