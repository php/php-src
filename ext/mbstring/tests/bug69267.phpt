--TEST--
Bug #69267 (mb_strtolower fails on titlecase characters)
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php
$str = "\u{01c5}";
var_dump(
    "\u{01c6}" === mb_strtolower($str)
);
?>
--EXPECT--
bool(true)