--TEST--
Bug #74361: Compaction in array_rand() violates COW
--FILE--
<?php

$array = [4 => 4];
var_dump(array_rand($array));

?>
--EXPECT--
int(4)
