--TEST--
Or pattern
--FILE--
<?php

var_dump(1 is 1|2|3);
var_dump(2 is 1|2|3);
var_dump(3 is 1|2|3);
var_dump(4 is 1|2|3);
var_dump(null is 1|2|3);
var_dump([] is 1|2|3);
var_dump('hello world' is (1|2|3));
var_dump('hello world' is 1|2|3 === false);

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)
bool(true)
