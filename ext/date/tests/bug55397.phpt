--TEST--
Bug #55397 (comparison of incomplete DateTime causes SIGSEGV)
--INI--
--FILE--
<?php
date_default_timezone_set('Europe/Prague');
var_dump(unserialize('O:8:"DateTime":0:{}') == new DateTime);
?>
--EXPECTF--
Fatal error: Invalid serialization data for DateTime object in %sbug55397.php on line %d
