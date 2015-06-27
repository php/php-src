--TEST--
Bug #69947: hex2bin() with odd-length input
--FILE--
<?php

var_dump(bin2hex(hex2bin('123')));

?>
--EXPECTF--
string(4) "0123"
