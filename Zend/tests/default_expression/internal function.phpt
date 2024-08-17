--TEST--
Tests passing default to an internal function parameter
--FILE--
<?php

json_encode([], depth: $D = default);
var_dump($D);
?>
--EXPECT--
int(512)
