--TEST--
Bug #68819 Fileinfo on specific file causes spurious OOM and/or segfault, var 2
--EXTENSIONS--
fileinfo
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
string(67) "ASCII text, with very long lines (8191), with CRLF line terminators"
