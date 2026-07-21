--TEST--
Phar: encrypted zip
--EXTENSIONS--
phar
--FILE--
<?php
try {
    new PharData(__DIR__ . '/files/encrypted.zip');
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECTF--
UnexpectedValueException: phar error: Cannot process encrypted zip files in zip-based phar "%sencrypted.zip"
