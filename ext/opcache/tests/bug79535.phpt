--TEST--
Bug #79535: PHP crashes with specific opcache.optimization_level
--INI--
opcache.optimization_level=0x000000a0
--FILE--
<?php
function create() {
    $name = stdClass::class;
    return new $name;
}
var_dump(create());
?>
--EXPECT--
object(stdClass)#1 (0) {
}
