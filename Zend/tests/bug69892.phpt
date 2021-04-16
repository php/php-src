--TEST--
Bug #69892: Different arrays compare identical due to integer key truncation
--PLATFORM--
bits: 64
--FILE--
<?php
var_dump([0 => 0] === [0x100000000 => 0]);
?>
--EXPECT--
bool(false)
