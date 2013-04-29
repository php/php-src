--TEST--
Bug #55397 (comparsion of incomplete DateTime causes SIGSEGV)
--INI--
--FILE--
<?php
date_default_timezone_set('Europe/Prague');
var_dump(unserialize('O:8:"DateTime":0:{}') == new DateTime);
?>
--EXPECTF--
Warning: %s: Trying to compare an incomplete DateTime object in %s on line %d
bool(false)
