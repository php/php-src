--TEST--
Bug #26320 (strtotime handling of XML Schema/ISO 8601 format)
--FILE--
<?php
    date_default_timezone_set('GMT0');
	echo date("Y-m-d H:i:s\n", strtotime("2003-11-19T12:30:42"));
	echo date("Y-m-d H:i:s\n", strtotime("2003-11-19T12:30:42Z"));
?>
--EXPECT--
2003-11-19 12:30:42
2003-11-19 12:30:42
