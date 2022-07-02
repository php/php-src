--TEST--
Phar: corrupted zip (truncated filename record)
--EXTENSIONS--
phar
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
