--TEST--
Bug #36988 (mktime freezes on long numbers)
--FILE--
<?php
date_default_timezone_set('GMT');
$start = microtime(true);
$a = mktime(1, 1, 1, 1, 1, 11111111111);
?>
--EXPECTF--
Warning: mktime() expects parameter 6 to be long, double given in %s on line %d
