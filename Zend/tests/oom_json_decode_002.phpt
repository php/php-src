--TEST--
OOM json_decode() nested arrays
--INI--
memory_limit=10M
--FILE--
<?php

$depth = 1024;
$nested = str_repeat('[', $depth) . str_repeat(']', $depth);
$json = '[' . $nested . str_repeat(','.$nested, 1024) . ']';
var_dump(json_decode($json, depth: $depth+2));

?>
--EXPECTF--
Fatal error: Allowed memory size of 10485760 bytes exhausted %S(tried to allocate %d bytes) in %s on line 6
