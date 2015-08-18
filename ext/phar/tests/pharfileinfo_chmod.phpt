--TEST--
Phar: PharFileInfo::chmod extra code coverage
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.readonly=0
--FILE--
<?php
$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar';
$pname = 'phar://' . $fname;

$phar = new Phar($fname);

$phar['a/b'] = 'hi there';

$b = $phar['a/b'];
try {
$phar['a']->chmod(066);
} catch (Exception $e) {
echo $e->getMessage(), "\n";
}
$b->chmod(array());
lstat($pname . '/a/b'); // sets BG(CurrentLStatFile)
$b->chmod(0666);
?>
===DONE===
--CLEAN--
<?php unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar'); ?>
--EXPECTF--
Phar entry "a" is a temporary directory (not an actual entry in the archive), cannot chmod

Warning: PharFileInfo::chmod() expects parameter 1 to be integer, array given in %spharfileinfo_chmod.php on line %d
===DONE===