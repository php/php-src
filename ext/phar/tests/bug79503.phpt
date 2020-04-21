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
phar error: "%s%ebug79503.phar" is a corrupted tar file (checksum mismatch of file "")
