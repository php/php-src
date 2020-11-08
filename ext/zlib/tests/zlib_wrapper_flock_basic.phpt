--TEST--
Test function stream_get_meta_data on a zlib stream
--SKIPIF--
<?php
if (!extension_loaded("zlib")) {
    print "skip - ZLIB extension not loaded";
}
?>
--FILE--
<?php
$f = __DIR__."/004.txt.gz";
$h = gzopen($f,'r');
var_dump(flock($h, LOCK_SH));
gzclose($h);
?>
--EXPECT--
bool(false)
