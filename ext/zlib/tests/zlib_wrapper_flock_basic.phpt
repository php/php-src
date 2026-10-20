--TEST--
Test function flock on a zlib stream
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Solaris') {
    die('skip Solaris: fcntl-based flock requires write access for LOCK_EX with read-only zlib stream');
}
?>
--EXTENSIONS--
zlib
--FILE--
<?php
$f = __DIR__."/data/zlib_flock.txt.gz";
$h = gzopen($f,'r');
var_dump(flock($h, LOCK_EX));
gzclose($h);
?>
--EXPECT--
bool(true)
