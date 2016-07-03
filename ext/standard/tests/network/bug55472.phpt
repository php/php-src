--TEST--
Bug #55472 (ip2long(integer) returns integer)
--FILE--
<?php
var_dump(ip2long(26));
?>
--EXPECT--
bool(false)
