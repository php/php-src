--TEST--
Bug #29893 (segfault when using array as index)
--FILE--
<?php
$base = 50;
$base[$base] -= 0; 
?>
===DONE===
--EXPECTF--
Warning: Cannot use a scalar value as an array in %sbug29893.php on line %d
===DONE===
