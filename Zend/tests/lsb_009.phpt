--TEST--
ZE2 Late Static Binding interface name "static"
--FILE--
<?php
interface static {
}
--EXPECTF--
Parse error: syntax error, unexpected T_STATIC, expecting T_STRING in %slsb_009.php on line 2
