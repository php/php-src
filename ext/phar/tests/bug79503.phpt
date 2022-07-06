--TEST--
Bug #79503 (Memory leak on duplicate metadata)
--SKIPIF--
<?php
if (!extension_loaded('phar')) die('skip phar extension not available');
?>
--FILE--
<?php
try {
    new Phar(__DIR__ . '/bug79503.phar');
} catch (UnexpectedValueException $ex) {
    echo $ex->getMessage();
}
?>
--EXPECTF--
phar error: tar-based phar "%s%ebug79503.phar" has invalid metadata in magic file ".phar/.metadata.bin"
