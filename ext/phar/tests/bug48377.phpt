--TEST--
Phar: PHP bug #48377 "error message unclear on converting phar with existing file"
--EXTENSIONS--
phar
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php
$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar';
$fname2 = __DIR__ . '/' . basename(__FILE__, '.php') . '.zip';

touch($fname2);

$phar = new Phar($fname, 0, 'a.phar');
$phar['x'] = 'hi';
try {
    $phar->convertToData(Phar::ZIP, Phar::NONE, 'zip');
} catch (BadMethodCallException $e) {
    echo $e->getMessage(),"\n";
}
?>
--CLEAN--
<?php unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar');?>
<?php unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.zip');?>
--EXPECTF--
phar "%sbug48377.zip" exists and must be unlinked prior to conversion
