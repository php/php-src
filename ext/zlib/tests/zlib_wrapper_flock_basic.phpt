--TEST--
Test function stream_get_meta_data on a zlib stream
--EXTENSIONS--
zlib
--FILE--
<?php
$f = __DIR__."/zlib_flock.txt.gz";
$h = gzopen($f,'r');
var_dump(flock($h, LOCK_SH));
gzclose($h);
?>
--EXPECT--
bool(false)
