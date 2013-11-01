--TEST--
test disabled expectations have no ill side effects
--INI--
zend.expectations=0
--FILE--
<?php
class OdEar extends ExpectationException {}

$variable = 1;
expect true : "constant message";
expect ($variable && $variable) || 
            php_sapi_name() : new OdEar("constant message");
var_dump(true);
?>
--EXPECTF--	
bool(true)
