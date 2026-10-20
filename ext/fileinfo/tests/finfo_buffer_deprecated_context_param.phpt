--TEST--
finfo_buffer() deprecated $context param
--EXTENSIONS--
fileinfo
--FILE--
<?php

$finfo = finfo_open();
$buffer = "Regular string here";

var_dump(finfo_buffer($finfo, $buffer, FILEINFO_NONE, null));
var_dump($finfo->buffer($buffer, FILEINFO_NONE, null));

?>
--EXPECTF--
Deprecated: finfo_buffer(): The $context parameter has no effect for finfo_buffer() in %s on line %d
string(36) "ASCII text, with no line terminators"

Deprecated: finfo::buffer(): The $context parameter has no effect for finfo_buffer() in %s on line %d
string(36) "ASCII text, with no line terminators"
