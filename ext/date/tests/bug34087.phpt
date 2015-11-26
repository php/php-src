--TEST--
Bug #34087 (strtotime() does not work with date format "Y/m/d")
--FILE--
<?php
date_default_timezone_set("UTC");
echo "Y/m/d: ", strtotime("2005/8/12"), "\n";
echo "Y-m-d: ", strtotime("2005-8-12"), "\n";

echo date(DATE_ISO8601, strtotime("2005/1/2")), "\n";
echo date(DATE_ISO8601, strtotime("2005/01/02")), "\n";
echo date(DATE_ISO8601, strtotime("2005/01/2")), "\n";
echo date(DATE_ISO8601, strtotime("2005/1/02")), "\n";
?>
--EXPECT--
Y/m/d: 1123804800
Y-m-d: 1123804800
2005-01-02T00:00:00+0000
2005-01-02T00:00:00+0000
2005-01-02T00:00:00+0000
2005-01-02T00:00:00+0000
