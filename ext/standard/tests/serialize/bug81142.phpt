--TEST--
Bug #81142 (memory leak when unserialize()ing associative array)
--FILE--
<?php
$mem0 = memory_get_usage();
$ctr = 0;
unserialize(serialize(["foo_$ctr" => 1]));
$mem1 = memory_get_usage();
var_dump($mem1 - $mem0);
?>
--EXPECT--
int(0)
