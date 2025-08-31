--TEST--
GHSA-jqcx-ccgc-xwhv (Buffer overflow and overread in phar_dir_read())
--EXTENSIONS--
phar
--INI--
phar.readonly=0
--FILE--
<?php
$phar = new Phar(__DIR__. '/GHSA-jqcx-ccgc-xwhv.phar');
$phar->startBuffering();
$phar->addFromString(str_repeat('A', PHP_MAXPATHLEN - 1), 'This is the content of file 1.');
$phar->addFromString(str_repeat('B', PHP_MAXPATHLEN - 1).'C', 'This is the content of file 2.');
$phar->stopBuffering();

$handle = opendir('phar://' . __DIR__ . '/GHSA-jqcx-ccgc-xwhv.phar');
var_dump(strlen(readdir($handle)));
// Must not be a string of length PHP_MAXPATHLEN+1
var_dump(readdir($handle));
closedir($handle);
?>
--CLEAN--
<?php
unlink(__DIR__. '/GHSA-jqcx-ccgc-xwhv.phar');
?>
--EXPECTF--
int(%d)
bool(false)
