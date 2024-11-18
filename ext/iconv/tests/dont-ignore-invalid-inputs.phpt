--TEST--
iconv with //IGNORE should not ignore invalid input sequences
--EXTENSIONS--
iconv
--SKIPIF--
<?php
// POSIX 2024 standardizes "//IGNORE", but some implementations (like musl)
// do not support it yet. If iconv() doesn't understand "//IGNORE", the
// tests below will still fail, but you'll get an error message about
// finding an unexpected string in your encoding name rather than the
// expected illegal-sequence error.
if (ICONV_IMPL == "unknown") {
    die("skip iconv implementation may not understand //IGNORE");
}
?>
--FILE--
<?php
$text = "aa\xC3\xC3\xC3\xB8aa";
var_dump(iconv("UTF-8", "UTF-8", $text));
var_dump(iconv("UTF-8", "UTF-8//IGNORE", $text));
// only invalid
var_dump(iconv("UTF-8", "UTF-8//IGNORE", "\xC3"));
// start invalid
var_dump(iconv("UTF-8", "UTF-8//IGNORE", "\xC3\xC3\xC3\xB8aa"));
// finish invalid
var_dump(iconv("UTF-8", "UTF-8//IGNORE", "aa\xC3\xC3\xC3"));
?>
--EXPECTF--
Notice: iconv(): Detected an illegal character in input string in %s on line %d
bool(false)

Notice: iconv(): Detected an illegal character in input string in %s on line %d
bool(false)

Notice: iconv(): Detected an incomplete multibyte character in input string in %s on line %d
bool(false)

Notice: iconv(): Detected an illegal character in input string in %s on line %d
bool(false)

Notice: iconv(): Detected an incomplete multibyte character in input string in %s on line %d
bool(false)
