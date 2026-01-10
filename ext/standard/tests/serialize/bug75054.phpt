--TEST--
Bug #75054: A Denial of Service Vulnerability was found when performing deserialization
--FILE--
<?php
$poc = 'a:9:{i:0;s:4:"0000";i:0;s:4:"0000";i:0;R:2;s:4:"5003";R:2;s:4:"0000";R:2;s:4:"0000";R:2;s:4:"';
$poc .= "\x06";
$poc .= '000";R:2;s:4:"0000";d:0;s:4:"0000";a:9:{s:4:"0000";';
var_dump(unserialize($poc));
?>
--EXPECTF--
Warning: unserialize(): Error at offset 43 of 145 bytes in %s on line %d
bool(false)
