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
try {
    var_dump( mb_ord("\u{d800}", "typo") );
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump( mb_ord("\u{d800}", "pass") );
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump( mb_ord("\u{d800}", "jis") );
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump( mb_ord("\u{d800}", "cp50222") );
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump( mb_ord("\u{d800}", "utf-7") );
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump( mb_ord("") );
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

mb_internal_encoding("utf-7");
mb_ord("");
?>
--EXPECTF--
bool(true)
bool(true)
bool(true)
mb_ord(): Argument #2 ($encoding) must be a valid encoding, "typo" given

Warning: mb_ord(): Unsupported encoding "pass" in %s on line %d
bool(false)

Warning: mb_ord(): Unsupported encoding "JIS" in %s on line %d
bool(false)

Warning: mb_ord(): Unsupported encoding "CP50222" in %s on line %d
bool(false)

Warning: mb_ord(): Unsupported encoding "UTF-7" in %s on line %d
bool(false)

Warning: mb_ord(): Empty string in %s on line %d
bool(false)

Warning: mb_ord(): Unsupported encoding "UTF-7" in %s on line %d
