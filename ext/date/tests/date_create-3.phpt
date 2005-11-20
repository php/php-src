--TEST--
date_create() function [3]
--SKIPIF--
<?php if (!function_exists('date_create')) echo "SKIP"; ?>
--FILE--
<?php
date_default_timezone_set("GMT");
echo date_format(date_create("Dec31 2005 +0100"), DATE_ISO8601), "\n";
echo date(DATE_ISO8601, strtotime("Dec31 2005 +0100")), "\n";
?>
--EXPECT--
2005-12-31T00:00:00+0100
2005-12-31T00:00:00+0100
