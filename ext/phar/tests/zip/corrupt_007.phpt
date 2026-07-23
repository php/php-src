--TEST--
Phar: corrupted zip (truncated filename record)
--EXTENSIONS--
phar
--FILE--
<?php
try {
    new PharData(__DIR__ . '/files/truncfilename.zip');
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECTF--
UnexpectedValueException: phar error: corrupted central directory entry, no magic signature in zip-based phar "%struncfilename.zip"
