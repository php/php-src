--TEST--
Bug #71527 Buffer over-write in finfo_open with malformed magic file
--SKIPIF--
<?php
if (!class_exists('finfo'))
    die('skip no fileinfo extension');
--ENV--
USE_ZEND_ALLOC=0
--FILE--
<?php
    $finfo = finfo_open(FILEINFO_NONE, __DIR__ . DIRECTORY_SEPARATOR . "bug71527.magic");
    var_dump($finfo);
?>
--EXPECTF--
Warning: finfo_open(): Failed to load magic database at "%sbug71527.magic" in %sbug71527.php on line %d
bool(false)
