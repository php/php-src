--TEST--
ReflectionClassConstant::isDeprecated() with userland functions.
--FILE--
<?php

#[\Deprecated]
function test() {
}

$r = new ReflectionFunction('test');

var_dump($r->isDeprecated());

?>
--EXPECTF--
bool(true)
