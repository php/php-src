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
var_dump(gzwrite($w, 'x'));
$f = fopen('compress.zlib:///dev/full', 'w');
var_dump(fwrite($f, $data));
var_dump(file_put_contents('compress.zlib:///dev/full', $data));
?>
--EXPECTF--
Notice: gzwrite(): Write of 300000 bytes failed with errno=%d %s in %s on line %d
bool(false)

Notice: gzwrite(): Write of 1 bytes failed with errno=%d %s in %s on line %d
bool(false)

Notice: fwrite(): Write of 300000 bytes failed with errno=%d %s in %s on line %d
bool(false)

Notice: file_put_contents(): Write of 300000 bytes failed with errno=%d %s in %s on line %d
bool(false)
