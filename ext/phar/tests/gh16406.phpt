--TEST--
GH-16406 (Assertion failure in ext/phar/phar.c:2808)
--EXTENSIONS--
phar
zlib
--INI--
phar.readonly=0
phar.require_hash=0
--FILE--
<?php
$fname = __DIR__ . '/gh16406.phar';
@unlink($fname . '.tar');
@unlink($fname . '.gz');
@unlink($fname);
$file = '<?php __HALT_COMPILER(); ?>';
$files['b'] = 'b';
$files['c'] = 'c';
include __DIR__.'/files/phar_test.inc';
$phar = new Phar($fname);
$phar->compressFiles(Phar::GZ);
$phar = $phar->convertToExecutable(Phar::TAR);
$phar = $phar->convertToExecutable(Phar::PHAR, Phar::GZ);
var_dump($phar['b']->openFile()->fread(4096));
var_dump($phar['c']->openFile()->fread(4096));
?>
--CLEAN--
<?php
$fname = __DIR__ . '/gh16406.phar';
@unlink($fname . '.tar');
@unlink($fname . '.gz');
@unlink($fname);
?>
--EXPECT--
string(1) "b"
string(1) "c"
