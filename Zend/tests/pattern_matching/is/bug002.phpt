--TEST--
Pattern matching: Bug 002
--FILE--
<?php

var_dump([1, 2, 3] is ([1, 2]));

?>
--EXPECT--
bool(false)
