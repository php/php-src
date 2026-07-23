--TEST--
Phar: corrupted zip (central directory offset incorrect)
--EXTENSIONS--
phar
--FILE--
<?php
try {
    new PharData(__DIR__ . '/files/cdir_offset.zip');
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECTF--
UnexpectedValueException: phar error: corrupted central directory entry, no magic signature in zip-based phar "%scdir_offset.zip"
