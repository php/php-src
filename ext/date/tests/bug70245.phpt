--TEST--
Bug #70245 (strtotime does not emit warning when 2nd parameter is object or string)
--FILE--
<?php
$d = new DateTime('2011-01-15 00:00:00');
var_dump(strtotime('-1 month', $d));
?>
--EXPECTF--
Warning: strtotime() expects parameter 2 to be int, object given in %sbug70245.php on line %d
bool(false)
