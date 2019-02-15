--TEST--
Phar: addFile/addFromString
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.readonly=0
--FILE--
<?php
$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php';
$pname = 'phar://' . $fname;
$phar = new Phar($fname);
$phar->addFromString('a', 'hi');
echo file_get_contents($pname . '/a') . "\n";
$phar->addFile($pname . '/a', 'b');
echo file_get_contents($pname . '/b') . "\n";
try {
$phar->addFile($pname . '/a');
} catch (Exception $e) {
echo $e->getMessage() . "\n";
}
try {
$phar->addFile($pname . '/a', 'a');
} catch (Exception $e) {
echo $e->getMessage() . "\n";
}
try {
$phar->addFile(dirname(__FILE__) . '/does/not/exist');
} catch (Exception $e) {
echo $e->getMessage() . "\n";
}
try {
$phar->addFile($pname . '/a', '.phar/stub.php');
} catch (Exception $e) {
echo $e->getMessage() . "\n";
}
try {
$phar->addFromString('.phar/stub.php', 'hi');
} catch (Exception $e) {
echo $e->getMessage() . "\n";
}
?>
===DONE===
--CLEAN--
<?php unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
--EXPECTF--
hi
hi
Entry phar://%saddfuncs.phar.php/a does not exist and cannot be created: phar error: invalid path "phar://%saddfuncs.phar.php/a" contains double slash
Entry a does not exist and cannot be created: phar error: file "a" in phar "%saddfuncs.phar.php" cannot be opened for writing, readable file pointers are open
phar error: unable to open file "%s/does/not/exist" to add to phar archive
Cannot create any files in magic ".phar" directory
Cannot create any files in magic ".phar" directory
===DONE===
