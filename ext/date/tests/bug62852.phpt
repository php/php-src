--TEST--
Bug #62852 (Unserialize invalid DateTime causes crash)
--INI--
date.timezone=GMT
--FILE--
<?php
try {
	$datetime = unserialize('O:8:"DateTime":3:{s:4:"date";s:20:"10007-06-07 03:51:49";s:13:"timezone_type";i:3;s:8:"timezone";s:3:"UTC";}');
	var_dump($datetime);	
} catch (Exception $e) {
    var_dump($e->getMessage());
}
?>
--EXPECTF--
string(%d) "DateTime::__wakeup(): Failed to parse time string (%s) at position 12 (0): Double time specification"
