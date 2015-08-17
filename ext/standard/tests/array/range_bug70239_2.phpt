--TEST--
Bug #70239 Creating a huge array doesn't result in exhausted, but segfault, var 3
--FILE--
<?php
var_dump(range(0, PHP_INT_MAX));
?>
===DONE===
--EXPECTF--
Warning: range(): The supplied range exceeds the maximum array size: start=0 end=%d in %srange_bug70239_2.php on line %d
bool(false)
===DONE===
