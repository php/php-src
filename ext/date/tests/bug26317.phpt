--TEST--
Bug #26317 (military timezone offset signedness)
--FILE--
<?php
    date_default_timezone_set('GMT0');
	echo date("Y-m-d H:i:s\n", strtotime("2003-11-19 16:20:42 Z"));
	echo date("Y-m-d H:i:s\n", strtotime("2003-11-19 09:20:42 T"));
	echo date("Y-m-d H:i:s\n", strtotime("2003-11-19 19:20:42 C"));
?>
--EXPECT--
2003-11-19 16:20:42
2003-11-19 16:20:42
2003-11-19 16:20:42
