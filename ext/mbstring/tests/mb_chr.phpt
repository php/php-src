--TEST--
mb_chr()
--EXTENSIONS--
mbstring
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
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump( mb_chr(0xd800, "pass") );
} catch (\ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump( mb_chr(0xd800, "jis") );
} catch (\ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump( mb_chr(0xd800, "cp50222") );
} catch (\ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump( mb_chr(0xd800, "utf-7") );
} catch (\ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

mb_internal_encoding("utf-7");
try {
    var_dump( mb_chr(0xd800) );
} catch (\ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
ValueError: mb_chr(): Argument #2 ($encoding) must be a valid encoding, "typo" given
ValueError: mb_chr(): Argument #2 ($encoding) must be a valid encoding, "pass" given
ValueError: mb_chr() does not support the "JIS" encoding
ValueError: mb_chr() does not support the "CP50222" encoding
ValueError: mb_chr() does not support the "UTF-7" encoding
ValueError: mb_chr() does not support the "UTF-7" encoding
