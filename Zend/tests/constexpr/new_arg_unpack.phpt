--TEST--
Argument unpacking in new arguments in static variable
--FILE--
<?php

static $x = new stdClass(...[0]);

var_dump($x);

?>
--EXPECT--
object(stdClass)#1 (0) {
}
