--TEST--
GH-22658: ReflectionConstant with a string value with a null byte
--FILE--
<?php

define('DEMO', "f\0oo");
$r = new ReflectionConstant('DEMO');
echo $r;
var_dump( $r->getValue() );

?>
--EXPECTF--
Constant [ string DEMO ] { f%0oo }
string(4) "f%0oo"
