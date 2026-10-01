--TEST--
mb_ord()
--EXTENSIONS--
mbstring
--FILE--
<?php
var_dump(
    0x20bb7 === mb_ord("\u{20bb7}"),
    false === mb_ord("\u{d800}"),
    0x50aa === mb_ord("\x8f\xa1\xef", "EUC-JP-2004")
);

// Empty string
try {
    var_dump( mb_ord("") );
} catch (\ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

// Invalid
try {
    var_dump( mb_ord("\u{d800}", "typo") );
} catch (\ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump( mb_ord("\u{d800}", "pass") );
} catch (\ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump( mb_ord("\u{d800}", "jis") );
} catch (\ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump( mb_ord("\u{d800}", "cp50222") );
} catch (\ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump( mb_ord("\u{d800}", "utf-7") );
} catch (\ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

mb_internal_encoding("utf-7");
try {
    var_dump( mb_ord("\u{d800}") );
} catch (\ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
ValueError: mb_ord(): Argument #1 ($string) must not be empty
ValueError: mb_ord(): Argument #2 ($encoding) must be a valid encoding, "typo" given
ValueError: mb_ord(): Argument #2 ($encoding) must be a valid encoding, "pass" given
ValueError: mb_ord() does not support the "JIS" encoding
ValueError: mb_ord() does not support the "CP50222" encoding
ValueError: mb_ord() does not support the "UTF-7" encoding
ValueError: mb_ord() does not support the "UTF-7" encoding
