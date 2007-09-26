--TEST--
ZE2 Late Static Binding class name "static"
--FILE--
<?php
class static {
}
--EXPECTF--
Parse error: syntax error, unexpected T_STATIC, expecting T_STRING in %slsb_008.php on line 2
