--TEST--
uniqid() must preserve NUL bytes in the prefix
--FILE--
<?php

$prefix = "a\0b";

$id = uniqid($prefix);
var_dump(str_starts_with($id, $prefix));
var_dump(strlen($id));

$id = uniqid($prefix, true);
var_dump(str_starts_with($id, $prefix));
var_dump(strlen($id));

?>
--EXPECT--
bool(true)
int(16)
bool(true)
int(26)
