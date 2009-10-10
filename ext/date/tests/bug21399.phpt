--TEST--
Bug #21399 (strtotime() request for "YYYYMMDDhhmmss [ZZZ]")
--FILE--
<?php
	date_default_timezone_set('GMT');
	echo gmdate("Y-m-d H:i:s", strtotime("20050620091407 GMT"));
?>
--EXPECT--
2005-06-20 09:14:07
