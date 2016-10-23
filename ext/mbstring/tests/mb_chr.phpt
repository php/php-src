--TEST--
mb_chr()
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php
var_dump(
    "\u{20bb7}" === mb_chr(0x20bb7),
    "\x8f\xa1\xef" === mb_chr(0x8fa1ef, "EUC-JP-2004"),
    "?" === mb_chr(0xd800)
);

mb_internal_encoding("UCS-4BE");
mb_substitute_character(0xfffd);
var_dump(
    "\u{fffd}" === mb_chr(0xd800, "UTF-8")
);
mb_substitute_character(0xd800);
var_dump(
    "?" === mb_chr(0xd800, "UTF-8")
);

mb_internal_encoding("EUC-JP");
mb_substitute_character(0xa4a2);
var_dump(
    "?" === mb_chr(0xd800, "UTF-8")
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
bool(true)
bool(true)

Warning: mb_chr(): Unknown encoding "typo" in %s on line 26

Warning: mb_chr(): Unsupported encoding "pass" in %s on line 27

Warning: mb_chr(): Unsupported encoding "jis" in %s on line 28

Warning: mb_chr(): Unsupported encoding "cp50222" in %s on line 29

Warning: mb_chr(): Unsupported encoding "utf-7" in %s on line 30
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
