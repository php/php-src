--TEST--
Phar: corrupted zip (truncated file comment)
--EXTENSIONS--
phar
--FILE--
<?php
try {
    new PharData(__DIR__ . '/files/filecomment.zip');
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECTF--
UnexpectedValueException: phar error: end of central directory not found in zip-based phar "%sfilecomment.zip"
