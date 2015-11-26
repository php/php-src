--TEST--
Bug #70239 Creating a huge array doesn't result in exhausted, but segfault, var 2
--FILE--
<?php
range(pow(2.0, 100000000), pow(2.0, 100000000) + 1);
?>
===DONE===
--EXPECTF--
Warning: range(): Invalid range supplied: start=inf end=inf in %srange_bug70239_1.php on line %d
===DONE===
