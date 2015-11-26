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

try {
$a = new PharFileInfo(array());
} catch (TypeError $e) {
echo $e->getMessage() . "\n";
}

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
PharFileInfo::__construct() expects parameter 1 to be string, array given
Cannot access phar file entry '%s' in archive '%s'
Cannot call constructor twice
'%s' is not a valid phar archive URL (must have at least phar://filename.phar)
===DONE===
