--TEST--
Bug #48147 (iconv with //IGNORE cuts the string)
--EXTENSIONS--
iconv
--SKIPIF--
<?php
// POSIX 2024 standardizes "//IGNORE", but does NOT say that
// it should ignore invalid input sequences, only untranslatable
// ones. The GNU implementations have however historically skipped
// invalid input sequences when it is used.
if (ICONV_IMPL != "libiconv" && ICONV_IMPL != "glibc") {
    die("skip iconv will not //IGNORE invalid input sequences");
}
?>
--FILE--
<?php
$text = "aa\xC3\xC3\xC3\xB8aa";
var_dump(iconv("UTF-8", "UTF-8", $text));
var_dump(urlencode(iconv("UTF-8", "UTF-8//IGNORE", $text)));
// only invalid
var_dump(urlencode(iconv("UTF-8", "UTF-8//IGNORE", "\xC3")));
// start invalid
var_dump(urlencode(iconv("UTF-8", "UTF-8//IGNORE", "\xC3\xC3\xC3\xB8aa")));
// finish invalid
var_dump(urlencode(iconv("UTF-8", "UTF-8//IGNORE", "aa\xC3\xC3\xC3")));
?>
--EXPECTF--
Notice: iconv(): Detected an illegal character in input string in %s on line %d
bool(false)
string(10) "aa%C3%B8aa"

Notice: iconv(): Detected an incomplete multibyte character in input string in %s on line %d
string(0) ""
string(8) "%C3%B8aa"

Notice: iconv(): Detected an incomplete multibyte character in input string in %s on line %d
string(0) ""
