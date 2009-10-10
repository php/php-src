--TEST--
Bug #29150 (Roman number format for months)
--FILE--
<?php
	date_default_timezone_set('GMT');
	echo gmdate("Y-m-d H:i:s", strtotime("20 VI. 2005"));
?>
--EXPECT--
2005-06-20 00:00:00
