--TEST--
GH-18076 (Since PHP 8, date_sun_info() returns inaccurate sunrise and sunset times)
--FILE--
<?php
date_default_timezone_set("UTC");
$sun_info = date_sun_info(strtotime("2025-03-21"), 51.48, 0.0);
echo date("H:i:s\n", $sun_info['sunrise']);
echo date("H:i:s\n", $sun_info['sunset']);
?>
--EXPECT--
05:59:21
18:14:48
