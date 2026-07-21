--TEST--
Phar: corrupted zip (no end of zip record)
--EXTENSIONS--
phar
--FILE--
<?php
try {
    new PharData(__DIR__ . '/files/nozipend.zip');
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECTF--
UnexpectedValueException: phar error: end of central directory not found in zip-based phar "%snozipend.zip"
