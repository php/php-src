--TEST--
Bug #62083: grapheme_extract() leaks memory
--EXTENSIONS--
intl
--FILE--
<?php
$arr1 = array();
try {
    grapheme_extract(-1, -1, -1,-1, $arr1);
} catch (ValueError $exception) {
    echo $exception::class, ': ', $exception->getMessage(), PHP_EOL;
}
?>
--EXPECT--
ValueError: grapheme_extract(): Argument #3 ($type) must be one of GRAPHEME_EXTR_COUNT, GRAPHEME_EXTR_MAXBYTES, or GRAPHEME_EXTR_MAXCHARS
