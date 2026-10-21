--TEST--
OOM json_decode() nested objects as arrays
--INI--
memory_limit=10M
--FILE--
<?php

$depth = 1024;
$nested = str_repeat('{"o":', $depth) . '{"o":null}' . str_repeat('}', $depth);
$elems = '"o0":'.$nested;
for ($i = 0; $i < 256; $i++) {
    $elems .= ',"o'.$i.'":'.$nested;
}
$json = '{' . $elems . '}';
var_dump(json_decode($json, associative: true, depth: $depth+3));
var_dump(json_last_error_msg());
?>
--EXPECTF--
Fatal error: Allowed memory size of 10485760 bytes exhausted %S(tried to allocate %d bytes) in %s on line 10
