--TEST--
iconv_strpos()
--EXTENSIONS--
iconv
--INI--
iconv.internal_charset=ISO-8859-1
--FILE--
<?php
function foo($haystk, $needle, $offset, $to_charset = false, $from_charset = false)
{
    if ($from_charset !== false) {
        $haystk = iconv($from_charset, $to_charset, $haystk);
    }
    try {
        var_dump(strpos($haystk, $needle, $offset));
    } catch (ValueError $exception) {
        echo $exception->getMessage() . "\n";
    }
    try {
        if ($to_charset !== false) {
            var_dump(iconv_strpos($haystk, $needle, $offset, $to_charset));
        } else {
            var_dump(iconv_strpos($haystk, $needle, $offset));
        }
    } catch (ValueError $exception) {
        echo $exception->getMessage() . "\n";
    }
}
foo("abecdbcdabef", "bcd", -1);
foo("abecdbcdabef", "bcd", -7);
foo("abecdbcdabef", "bcd", 12);
foo("abecdbcdabef", "bcd", 100000);
foo("abcabcabcdabcababcdabc", "bcd", 0);
foo("abcabcabcdabcababcdabc", "bcd", 10);
foo(str_repeat("abcab", 60)."abcdb".str_repeat("adabc", 60), "abcd", 0);
foo(str_repeat("あいうえお", 30)."いうおえあ".str_repeat("あいえおう", 30), "うお", 0, "EUC-JP");
$str = str_repeat("あいうえお", 60).'$'.str_repeat("あいえおう", 60);
foo($str, '$', 0, "ISO-2022-JP", "EUC-JP");

var_dump(iconv_strpos("string", ""));
var_dump(iconv_strpos("", "string"));

?>
--EXPECT--
bool(false)
bool(false)
int(5)
int(5)
bool(false)
bool(false)
strpos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
iconv_strpos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
int(7)
int(7)
int(16)
int(16)
int(300)
int(300)
int(302)
int(151)
int(1)
int(300)
bool(false)
bool(false)
