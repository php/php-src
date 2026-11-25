--TEST--
gzwrite(), fwrite() on compress.zlib://: a write error is reported
--EXTENSIONS--
zlib
--SKIPIF--
<?php
if (!file_exists('/dev/full')) die('skip needs /dev/full');
?>
--FILE--
<?php
$data = random_bytes(300000);
$w = gzopen('/dev/full', 'w');
var_dump(gzwrite($w, $data));
$f = fopen('compress.zlib:///dev/full', 'w');
var_dump(fwrite($f, $data));
?>
--EXPECT--
bool(false)
bool(false)
