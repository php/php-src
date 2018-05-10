--TEST--
Weakref no unserialization
--FILE--
<?php
$wrs = 'O:7:"WeakRef":0:{}';

var_dump(@unserialize($wrs));
?>
--EXPECT--
bool(false)
