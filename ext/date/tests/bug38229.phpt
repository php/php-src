--TEST--
Bug #38229 (strtotime() does not parse YYYY-MM)
--FILE--
<?php
date_default_timezone_set("GMT");
echo date("Y-m", strtotime('2006-1'))."\n";
echo date("Y-m", strtotime('2006-03'))."\n";
echo date("Y-m", strtotime('2006-12'))."\n";
?>
--EXPECT--
2006-01
2006-03
2006-12
