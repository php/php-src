--TEST--
Phar object: getContent() (verify it works with compression)
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
<?php if (!extension_loaded("zlib")) die("skip zlib not available"); ?>
--INI--
phar.readonly=0
--FILE--
<?php
$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.php';
$fname2 = __DIR__ . '/' . basename(__FILE__, '.php') . '.2.phar.php';

$phar = new Phar($fname);
$phar['a'] = 'file contents
this works';
$phar['a']->compress(Phar::GZ);
copy($fname, $fname2);
$phar2 = new Phar($fname2);
var_dump($phar2['a']->isCompressed());
echo $phar2['a']->getContent() . "\n";
?>
--CLEAN--
<?php
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.php');
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.2.phar.php');
__halt_compiler();
?>
--EXPECT--
bool(true)
file contents
this works
