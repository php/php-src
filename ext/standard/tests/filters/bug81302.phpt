--TEST--
Bug #81302 (Stream position after stream filter removed)
--SKIPIF--
<?php
if (!extension_loaded('zlib')) die("skip zlib extension not available");
?>
--FILE--
<?php
$f = fopen("php://memory", "w+b");
$z = stream_filter_append($f, "zlib.deflate", STREAM_FILTER_WRITE, 6);
fwrite($f, "Testing");
stream_filter_remove($z);
$pos = ftell($f);
fseek($f, 0);
$count = strlen(fread($f, 1024));
fclose($f);
var_dump($count === $pos);
?>
--EXPECT--
bool(true)
