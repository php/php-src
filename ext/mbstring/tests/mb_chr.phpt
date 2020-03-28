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
try {
    var_dump( mb_chr(0xd800, "typo") );
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump( mb_chr(0xd800, "pass") );
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump( mb_chr(0xd800, "jis") );
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump( mb_chr(0xd800, "cp50222") );
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump( mb_chr(0xd800, "utf-7") );
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECTF--
bool(true)
bool(true)
bool(true)
bool(true)
mb_chr(): Argument #2 ($encoding) must be a valid encoding, "typo" given

Warning: mb_chr(): Unsupported encoding "pass" in %s on line %d
bool(false)

Warning: mb_chr(): Unsupported encoding "jis" in %s on line %d
bool(false)

Warning: mb_chr(): Unsupported encoding "cp50222" in %s on line %d
bool(false)

Warning: mb_chr(): Unsupported encoding "utf-7" in %s on line %d
bool(false)
