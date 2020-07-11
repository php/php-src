--TEST--
Phar: corrupted zip (central directory offset incorrect)
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--FILE--
<?php
try {
    new PharData(__DIR__ . '/files/cdir_offset.zip');
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECTF--
phar error: corrupted central directory entry, no magic signature in zip-based phar "%scdir_offset.zip"
