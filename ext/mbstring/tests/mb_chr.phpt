--TEST--
mb_chr()
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php
var_dump(
    "\u{20bb7}" === mb_chr(0x20bb7),
    "\x8f\xa1\xef" === mb_chr(0x50aa, "EUC-JP-2004"),
    false === mb_chr(0xd800),
    false === mb_chr(0x1f600, "EUC-JP-2004")
);

// Invalid
var_dump(
	mb_chr(0xd800, "typo"),
	mb_chr(0xd800, "pass"),
	mb_chr(0xd800, "jis"),
	mb_chr(0xd800, "cp50222"),
	mb_chr(0xd800, "utf-7")
);
?>
--EXPECTF--
bool(true)
bool(true)
bool(true)
bool(true)

Warning: mb_chr(): Unknown encoding "typo" in %s on line %d

Warning: mb_chr(): Unsupported encoding "pass" in %s on line %d

Warning: mb_chr(): Unsupported encoding "jis" in %s on line %d

Warning: mb_chr(): Unsupported encoding "cp50222" in %s on line %d

Warning: mb_chr(): Unsupported encoding "utf-7" in %s on line %d
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
