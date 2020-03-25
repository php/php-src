--TEST--
Bug #68819 Fileinfo on specific file causes spurious OOM and/or segfault, var 2
--SKIPIF--
<?php require_once(__DIR__ . '/skipif.inc'); ?>
--FILE--
<?php

$string = '';

// These two in any order
$string .= "\r\n";
$string .= "''''";

// Total string length > 8192
$string .= str_repeat("a", 8184);

// Ending in this string
$string .= "say";

$finfo = new finfo();
$type = $finfo->buffer($string);
var_dump($type);

?>
--EXPECT--
string(60) "ASCII text, with very long lines, with CRLF line terminators"
