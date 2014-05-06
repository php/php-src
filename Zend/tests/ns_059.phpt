--TEST--
059: Constant arrays
--XFAIL--
Actually it's hard to test where the array comes from (property, constant, ...)
--FILE--
<?php
const C = array();
--EXPECTF--
Fatal error: Arrays are not allowed as constants in %sns_059.php on line 2

