--TEST--
Phar: PharFileInfo::__construct
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.readonly=0
--FILE--
<?php
$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar';
$pname = 'phar://' . $fname;

try {
file_put_contents($fname, 'blah');
$a = new PharFileInfo($pname . '/oops');
} catch (Exception $e) {
echo $e->getMessage() . "\n";
unlink($fname);
}

$a = new PharFileInfo(array());

$a = new Phar($fname);
$a['a'] = 'hi';
$b = $a['a'];

try {
$a = new PharFileInfo($pname . '/oops/I/do/not/exist');
} catch (Exception $e) {
echo $e->getMessage() . "\n";
}

try {
$b->__construct('oops');
} catch (Exception $e) {
echo $e->getMessage() . "\n";
}

try {
$a = new PharFileInfo(__FILE__);
} catch (Exception $e) {
echo $e->getMessage() . "\n";
}
?>
===DONE===
--CLEAN--
<?php unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar'); ?>
--EXPECTF--
Cannot open phar file 'phar://%spharfileinfo_construct.phar/oops': internal corruption of phar "%spharfileinfo_construct.phar" (truncated entry)

Warning: PharFileInfo::__construct() expects parameter 1 to be a valid path, array given in %spharfileinfo_construct.php on line %d
Cannot access phar file entry '/oops/I/do/not/exist' in archive '%spharfileinfo_construct.phar'
Cannot call constructor twice
'%spharfileinfo_construct.php' is not a valid phar archive URL (must have at least phar://filename.phar)
===DONE===