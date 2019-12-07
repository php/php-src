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
$invalids = [
    "typo",
    "pass",
    "jis",
    "cp50222",
    "utf-7"
];

foreach($invalids as $invalid) {
    try {
	    mb_chr(0xd800, $invalid);
    } catch (\ValueError $e) {
        echo $e->getMessage() . \PHP_EOL;
    }
}

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
Unknown encoding "typo"
Unsupported encoding "pass"
Unsupported encoding "jis"
Unsupported encoding "cp50222"
Unsupported encoding "utf-7"
