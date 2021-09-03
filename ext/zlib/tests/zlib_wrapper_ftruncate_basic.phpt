--TEST--
Test function ftruncate() on zlib wrapper by calling it with its expected arguments
--EXTENSIONS--
zlib
--FILE--
<?php
$f = __DIR__."/004.txt.gz";
$f2 = "zlib_wrapper_ftruncate_basic.txt.gz";
copy($f, $f2);

$h = gzopen($f2, "r");
ftruncate($h, 20);
fclose($h);
unlink($f2);

$h = gzopen($f2, "w");
ftruncate($h, 20);
fclose($h);
unlink($f2);

?>
--EXPECTF--
Warning: ftruncate(): Can't truncate this stream! in %s on line %d

Warning: ftruncate(): Can't truncate this stream! in %s on line %d
