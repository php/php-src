--TEST--
Phar: test for the odd case where we intend to remove an archive from memory
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.readonly=0
--FILE--
<?php
$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php';
$pname = 'phar://' . $fname;
$fname2 = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.2.phar.php';
$pname2 = 'phar://' . $fname2;

$phar = new Phar($fname);
$phar->setAlias('first');
$phar['file1.txt'] = 'hi';
unset($phar);

$phar2 = new Phar($fname2);
$phar2->setAlias('first'); // this works because there are no references to $fname open
$phar2['file1.txt'] = 'hi';
unset($phar2);

$a = fopen($pname . '/file1.txt', 'r'); // this works because there are no references to $fname2 open
try {
$phar2 = new Phar($fname2); // fails because references open to $fname
} catch (Exception $e) {
echo $e->getMessage(),"\n";
}
fclose($a);
$phar2 = new Phar($fname2); // succeeds because all refs are closed
var_dump($phar2->getAlias());

$a = file_get_contents($pname . '/file1.txt'); // this fails because $fname2 ref exists
?>
===DONE===
--CLEAN--
<?php unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
<?php unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.2.phar.php'); ?>
--EXPECTF--
Cannot open archive "%stest_alias_unset.2.phar.php", alias is already in use by existing archive
string(5) "first"

Warning: file_get_contents(phar://%sfile1.txt): failed to open stream: Cannot open archive "%stest_alias_unset.phar.php", alias is already in use by existing archive in %stest_alias_unset.php on line %d
===DONE===
