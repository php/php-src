--TEST--
Bug #75776 (Flushing streams with compression filter is broken)
--SKIPIF--
<?php
if (!extension_loaded('bz2')) die('skip bz2 extension not available');
?>
--FILE--
<?php
$text = str_repeat('0123456789abcdef', 1000);

$temp = fopen('php://temp', 'r+');
stream_filter_append($temp, 'bzip2.compress', STREAM_FILTER_WRITE);
fwrite($temp, $text);

rewind($temp);

var_dump(bin2hex(stream_get_contents($temp)));
var_dump(ftell($temp));

fclose($temp);
?>
--EXPECT--
string(144) "425a68343141592653599fe7bbbf0001f389007fe03f002000902980026826aa80003ea9061520c6a41954833a9069520d6a41b54837a9071520e6a41d5483ba9079520f6a41f548"
int(72)
