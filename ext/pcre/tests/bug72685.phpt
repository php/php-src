--TEST--
Bug #72685: Same string is UTF-8 validated repeatedly
--SKIPIF--
<?php
if (getenv('SKIP_PERF_SENSITIVE')) die("skip performance sensitive test");
?>
--FILE--
<?php

$input_size = 64 * 1024;
$str = str_repeat('a', $input_size);

$start = microtime(true);
$pos = 0;
while (preg_match('/\G\w/u', $str, $m, 0, $pos)) ++$pos;
$end = microtime(true);
var_dump(($end - $start) < 0.5); // large margin, more like 0.05 in debug build

?>
--EXPECT--
bool(true)
