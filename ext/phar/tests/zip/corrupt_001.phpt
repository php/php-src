--TEST--
Phar: corrupted zip (count mismatch)
--EXTENSIONS--
phar
--FILE--
<?php
try {
    new PharData(__DIR__ . '/files/count1.zip');
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    new PharData(__DIR__ . '/files/count2.zip');
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECTF--
UnexpectedValueException: phar error: corrupt zip archive, conflicting file count in end of central directory record in zip-based phar "%scount1.zip"
UnexpectedValueException: phar error: corrupt zip archive, conflicting file count in end of central directory record in zip-based phar "%scount2.zip"
