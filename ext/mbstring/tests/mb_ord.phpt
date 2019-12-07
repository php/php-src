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
$invalids = [
    "typo",
    "pass",
    "jis",
    "cp50222",
    "utf-7"
];

foreach($invalids as $invalid) {
    try {
	    mb_ord("\u{d800}", $invalid);
    } catch (\ValueError $e) {
        echo $e->getMessage() . \PHP_EOL;
    }
}

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
Unknown encoding "typo"
Unsupported encoding "pass"
Unsupported encoding "jis"
Unsupported encoding "cp50222"
Unsupported encoding "utf-7"
