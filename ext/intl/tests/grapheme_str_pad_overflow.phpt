--TEST--
grapheme_str_pad() throws when the result size overflows, like mb_str_pad()
--EXTENSIONS--
intl
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only"); ?>
--FILE--
<?php
// One side alone does not fit
try {
    grapheme_str_pad("a", PHP_INT_MAX, "\u{1F600}");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

// Each side fits, both together do not
try {
    grapheme_str_pad("a", PHP_INT_MAX, "\u{00E9}", STR_PAD_BOTH);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
Error: String size overflow
Error: String size overflow
