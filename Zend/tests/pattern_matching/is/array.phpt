--TEST--
Array pattern
--FILE--
<?php

var_dump([] is []);
var_dump(42 is []);
var_dump('foo' is []);
var_dump([42] is [42]);
var_dump([42] is []);
var_dump([42] is [43]);
var_dump([42] is ['0' => 42]);
var_dump([42, 43] is [42]);
var_dump([42, 43] is [42, ...]);
var_dump([42] is [$a]);
var_dump($a);

?>
--EXPECT--
bool(true)
bool(false)
bool(false)
bool(true)
bool(false)
bool(false)
bool(true)
bool(false)
bool(true)
bool(true)
int(42)
