--TEST--
Tests passing default to an internal function parameter
--FILE--
<?php

json_encode([], 0, $D = default);
var_dump($D);
?>
--EXPECT--
int(512)
