--TEST--
Preload: Unresolved property type
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/preload_unresolved_prop_type.inc
--EXTENSIONS--
opcache
--FILE--
<?php
class Unknown {
}
$x = new Test;
$x->prop = new Unknown;
?>
===DONE===
--EXPECT--
===DONE===
