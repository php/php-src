--TEST--
GH-22681: null bytes in name truncate ReflectionConstant::__toString()
--FILE--
<?php

define("F\0oo", true);

$r = new ReflectionConstant("F\0oo");
echo $r;
var_dump( $r->getName() );

?>
--EXPECTF--
Constant [ bool F ] { 1 }
string(4) "F%0oo"
