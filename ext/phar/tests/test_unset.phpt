--TEST--
Phar: ensure unset() works properly on a non-flushed phar archive
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.readonly=0
--FILE--
<?php
$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.php';
$fname2 = __DIR__ . '/' . basename(__FILE__, '.php') . '.2.phar.php';

if (file_exists($fname)) unlink($fname);
if (file_exists($fname2)) unlink($fname2);

$phar = new Phar($fname); // no entries, never flushed
$phar->setAlias('first');
$phar->setMetadata('hi');
unset($phar);

$phar = new Phar($fname2);
$phar['b'] = 'whatever'; // flushed
try {
   $phar->setAlias('first');
} catch(Exception $e) {
   echo $e->getMessage()."\n";
}

$phar = new Phar($fname);
var_dump($phar->getMetadata());
var_dump($phar->getAlias());
var_dump(file_exists($fname));

?>
===DONE===
--CLEAN--
<?php unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
<?php unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.2.phar.php'); ?>
--EXPECTF--
NULL
string(%d) "%stest_unset.phar.php"
bool(false)
===DONE===
