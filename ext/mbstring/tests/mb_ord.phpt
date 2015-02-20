--TEST--
mb_ord()
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php
var_dump(
    0x20bb7 === mb_ord("\u{20bb7}"),
    0x3f === mb_ord("\u{d800}"),
    0x8fa1ef === mb_ord("\x8f\xa1\xef", "EUC-JP-2004")
);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)