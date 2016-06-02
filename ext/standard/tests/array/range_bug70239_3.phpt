--TEST--
Bug #70239 Creating a huge array doesn't result in exhausted, but segfault, var 4
--FILE--
<?php
var_dump(range(PHP_INT_MIN, 0));
?>
===DONE===
--EXPECTF--
Warning: range(): The supplied range exceeds the maximum array size: start=-%d end=0 in %srange_bug70239_3.php on line %d
bool(false)
===DONE===
