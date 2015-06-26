--TEST--
Bug #61660: bin2hex(hex2bin($data)) != $data
--FILE--
<?php

var_dump(bin2hex(hex2bin('123')));

?>
--EXPECTF--
string(4) "0123"
