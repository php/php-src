--TEST--
Bug #xxxxx (military timezone offset signedness)
--SKIP--
if (!@putenv("TZ=GMT0") || getenv("TZ") != 'GMT0') {
	die("skip unable to change TZ enviroment variable\n");
}
--FILE--
<?php
    putenv("TZ=GMT0");
	echo date("Y-m-d H:i:s\n", strtotime("2003-11-19 12:30:42 Z"));
	echo date("Y-m-d H:i:s\n", strtotime("2003-11-19 12:30:42 T"));
	echo date("Y-m-d H:i:s\n", strtotime("2003-11-19 12:30:42 k"));
?>
--EXPECT--
2003-11-19 12:30:42
2003-11-19 05:30:42
2003-11-19 22:30:42