--TEST--
DateTime::createFromFormat() with UTC offset of 100 hours or more
--FILE--
<?php
var_dump(DateTime::createFromFormat('O', '+9999')->getTimezone()->getName());
?>
--EXPECT--
string(7) "+100:39"
