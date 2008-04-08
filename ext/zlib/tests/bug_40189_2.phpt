--TEST--
Bug #40189 (test for truncated deflate, also part of erroneous fix for #40189)
--SKIPIF--
<?php if (!extension_loaded("zlib")) print "skip"; ?>
--FILE--
<?php
$a = fopen(dirname(__FILE__). '/bug.tar', 'rb');
stream_filter_append($a, 'zlib.deflate', STREAM_FILTER_READ, array('window' => 15+16));
$b = fread($a, 4716032);
var_dump(strlen($b));
// when broken, this outputs "int(686904)"
?>
--EXPECT--
int(1676116)