--TEST--
mb_ord()
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php
var_dump(
    0x20bb7 === mb_ord("\u{20bb7}"),
    false === mb_ord("\u{d800}"),
    0x50aa === mb_ord("\x8f\xa1\xef", "EUC-JP-2004")
);

// Invalid
var_dump(
	mb_ord("\u{d800}", "typo"),
	mb_ord("\u{d800}", "pass"),
	mb_ord("\u{d800}", "jis"),
	mb_ord("\u{d800}", "cp50222"),
	mb_ord("\u{d800}", "utf-7"),
	mb_ord("")
);

mb_internal_encoding("utf-7");
mb_ord("");
?>
--EXPECTF--
bool(true)
bool(true)
bool(true)

Warning: mb_ord(): Unknown encoding "typo" %s 10

Warning: mb_ord(): Unsupported encoding "pass" %s 11

Warning: mb_ord(): Unsupported encoding "JIS" in %s on line %d

Warning: mb_ord(): Unsupported encoding "CP50222" in %s on line %d

Warning: mb_ord(): Unsupported encoding "UTF-7" in %s on line %d

Warning: mb_ord(): Empty string in %s on line %d
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)

Warning: mb_ord(): Unsupported encoding "UTF-7" in %s on line %d
