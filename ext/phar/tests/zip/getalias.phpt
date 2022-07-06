--TEST--
Phar: getAlias() with an existing phar.zip
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.readonly=0
phar.require_hash=0
--FILE--
<?php

$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.zip';

// sanity check with a virgin phar.zip
$phar = new Phar($fname);
var_dump($phar->getAlias());
unset($phar);

copy(__DIR__ . '/files/metadata.phar.zip', $fname);

// existing phar.zip, no alias set
$phar = new Phar($fname);
var_dump($phar->getAlias());

// check that default alias can be overwritten
$phar->setAlias('jiminycricket');
var_dump($phar->getAlias());
unset($phar);

// existing phar.zip, alias set
$phar = new Phar($fname);
var_dump($phar->getAlias());

// check that alias can't be set manually
try {
    $phar['.phar/alias.txt'] = 'pinocchio';
} catch (Exception $e) {
    echo $e->getMessage()."\n";
}
var_dump($phar->getAlias());

// check that user-defined alias can be overwritten
$phar->setAlias('pinocchio');
var_dump($phar->getAlias());

?>
--CLEAN--
<?php
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.zip');
__HALT_COMPILER();
?>
--EXPECTF--
string(%d) "%sgetalias.phar.zip"
string(%d) "%sgetalias.phar.zip"
string(13) "jiminycricket"
string(13) "jiminycricket"
Cannot set alias ".phar/alias.txt" directly in phar "%sgetalias.phar.zip", use setAlias
string(13) "jiminycricket"
string(9) "pinocchio"
