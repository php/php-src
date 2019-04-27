--TEST--
phar: mkdir/rmdir edge cases
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.readonly=0
--FILE--
<?php
$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.php';
$pname = 'phar://' . $fname;
Phar::interceptFileFuncs();
mkdir('phar://');
mkdir('phar://foo.phar');
$a = new Phar($fname);
$a['a'] = 'hi';
mkdir($pname . '/a');
rmdir('phar://');
rmdir('phar://foo.phar');
rmdir($pname . '/a');
$a->addEmptyDir('bb');
$a->addEmptyDir('bb');
try {
$a->addEmptyDir('.phar');
} catch (Exception $e) {
echo $e->getMessage(),"\n";
}
?>
===DONE===
--CLEAN--
<?php
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.php');
__HALT_COMPILER();
?>
--EXPECTF--
Warning: mkdir(): phar error: cannot create directory "phar://", no phar archive specified in %smkdir.php on line %d

Warning: mkdir(): phar error: cannot create directory "" in phar "foo.phar", phar error: invalid path "" must not be empty in %smkdir.php on line %d

Warning: mkdir(): phar error: cannot create directory "a" in phar "%smkdir.phar.php", phar error: path "a" exists and is a not a directory in %smkdir.php on line %d

Warning: rmdir(): phar error: cannot remove directory "phar://", no phar archive specified, or phar archive does not exist in %smkdir.php on line %d

Warning: rmdir(): phar error: cannot remove directory "" in phar "foo.phar", directory does not exist in %smkdir.php on line %d

Warning: rmdir(): phar error: cannot remove directory "a" in phar "%smkdir.phar.php", phar error: path "a" exists and is a not a directory in %smkdir.php on line %d
Cannot create a directory in magic ".phar" directory
===DONE===
