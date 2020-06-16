--TEST--
Phar: corrupted zip (count mismatch)
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--FILE--
<?php
try {
    new PharData(__DIR__ . '/files/count1.zip');
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}
try {
    new PharData(__DIR__ . '/files/count2.zip');
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECTF--
phar error: corrupt zip archive, conflicting file count in end of central directory record in zip-based phar "%scount1.zip"
phar error: corrupt zip archive, conflicting file count in end of central directory record in zip-based phar "%scount2.zip"
