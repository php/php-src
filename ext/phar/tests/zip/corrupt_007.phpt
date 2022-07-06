--TEST--
Phar: corrupted zip (truncated filename record)
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--FILE--
<?php
try {
    new PharData(__DIR__ . '/files/truncfilename.zip');
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECTF--
phar error: corrupted central directory entry, no magic signature in zip-based phar "%struncfilename.zip"
