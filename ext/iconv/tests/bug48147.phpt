--TEST--
Bug #48147 (iconv with //IGNORE cuts the string)
--EXTENSIONS--
iconv
--SKIPIF--
<?php
/*
 * POSIX 2024 specifies how the "//IGNORE" suffix should behave, but
 * falls short of requiring implementations to support it (iconv_open
 * is allowed to return EINVAL for a suffix it does not recognize).
 *
 * Many implementations still do not support it, which is OK. We
 * whitelist the ones that are known to.
 */
if (ICONV_IMPL != "glibc" && ICONV_IMPL != "libiconv") {
    die("skip iconv implementation may not support //IGNORE");
}
?>
--FILE--
<?php
/*
 * POSIX says that when //IGNORE is specified, invalid bytes followed
 * by valid bytes "shall not be treated as an error." GNU iconv does
 * not follow this convention, but PHP does the right thing. In the
 * examples below, invalid bytes in the middle of the string get
 * dropped, and a string is returned. The two examples where the
 * problem is at the end do not qualify for the "shall not" exception
 * because there are no VALID bytes after the error. So PHP is morally
 * correct in those cases to return an error (false).
 */
$text = "aa\xC3\xC3\xC3\xB8aa";
var_dump(iconv("UTF-8", "UTF-8", $text));
var_dump(urlencode(iconv("UTF-8", "UTF-8//IGNORE", $text)));
// only invalid
var_dump(iconv("UTF-8", "UTF-8//IGNORE", "\xC3"));
// start invalid
var_dump(urlencode(iconv("UTF-8", "UTF-8//IGNORE", "\xC3\xC3\xC3\xB8aa")));
// finish invalid
var_dump(iconv("UTF-8", "UTF-8//IGNORE", "aa\xC3\xC3\xC3"));
?>
--EXPECTF--
Notice: iconv(): Detected an illegal character in input string in %s on line %d
bool(false)
string(10) "aa%C3%B8aa"

Notice: iconv(): Detected an incomplete multibyte character in input string in %s on line %d
bool(false)
string(8) "%C3%B8aa"

Notice: iconv(): Detected an incomplete multibyte character in input string in %s on line %d
bool(false)
