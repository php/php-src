--TEST--
jdtounix()
--INI--
date.timezone=UTC
--EXTENSIONS--
calendar
--FILE--
<?php
echo date("Y-m-d",jdtounix(2440588)). "\n";
echo date("Y-m-d",jdtounix(2452162)). "\n";
echo date("Y-m-d",jdtounix(2453926)). "\n";
?>
--EXPECT--
1970-01-01
2001-09-09
2006-07-09
