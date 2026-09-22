--TEST--
Phar object: add file
--EXTENSIONS--
phar
--INI--
phar.readonly=0
phar.require_hash=0
--FILE--
<?php

$pharconfig = 0;

require_once 'files/phar_oo_test.inc';

$phar = new Phar($fname);

$phar['hi/f.php'] = 'hi';
var_dump(isset($phar['hi']));
var_dump($phar['hi']);
var_dump(isset($phar['hi/f.php']));
echo $phar['hi/f.php'];
echo "\n";

$phar->setInfoClass('SplFileObject');
$phar['hi/f.php'] = 'hi';
var_dump(isset($phar['hi']));
try {
    var_dump($phar['hi']);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
var_dump(isset($phar['hi/f.php']));
echo $phar['hi/f.php'];
echo "\n";

?>
--CLEAN--
<?php
unlink(__DIR__ . '/files/phar_oo_011.phar.php');
__halt_compiler();
?>
--EXPECTF--
bool(true)
object(PharFileInfo)#%d (2) {
  ["pathName":"SplFileInfo":private]=>
  string(%d) "phar://%s/phar_oo_011.phar.php/hi"
  ["fileName":"SplFileInfo":private]=>
  string(2) "hi"
}
bool(true)
phar://%s/phar_oo_011.phar.php/hi/f.php
bool(false)
LogicException: Cannot use SplFileObject with directories
bool(true)
hi
