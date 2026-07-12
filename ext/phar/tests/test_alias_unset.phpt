--TEST--
Phar: test for the odd case where we intend to remove an archive from memory
--EXTENSIONS--
phar
--INI--
phar.readonly=0
--FILE--
<?php
$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.php';
$pname = 'phar://' . $fname;
$fname2 = __DIR__ . '/' . basename(__FILE__, '.php') . '.2.phar.php';
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
--CLEAN--
<?php unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
<?php unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.2.phar.php'); ?>
--EXPECTF--
Warning: fopen(phar://%stest_alias_unset.phar.php/file1.txt): Failed to open stream: Cannot open archive "%stest_alias_unset.phar.php", alias is already in use by existing archive in %stest_alias_unset.php on line %d

Fatal error: Uncaught TypeError: fclose(): Argument #1 ($stream) must be of type resource, false given in %stest_alias_unset.php:%d
Stack trace:
#0 %stest_alias_unset.php(%d): fclose(false)
#1 {main}
  thrown in %stest_alias_unset.php on line %d
