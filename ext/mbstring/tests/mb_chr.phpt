--TEST--
mb_chr()
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php
var_dump(
    "\u{20bb7}" === mb_chr(0x20bb7),
    "?" === mb_chr(0xd800),
    "\x8f\xa1\xef" === mb_chr(0x8fa1ef, "EUC-JP-2004")
);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)