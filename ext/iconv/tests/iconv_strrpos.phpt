--TEST--
iconv_strrpos()
--SKIPIF--
<?php extension_loaded('iconv') or die('skip iconv extension is not available'); ?>
--INI--
iconv.internal_charset=ISO-8859-1
--FILE--
<?php
function my_error_handler($errno, $errmsg, $filename, $linenum, $vars)
{
	echo "$errno: $errmsg\n";
}
set_error_handler('my_error_handler');
function foo($haystk, $needle, $to_charset = false, $from_charset = false)
{
	if ($from_charset !== false) {
		$haystk = iconv($from_charset, $to_charset, $haystk);
	}
	if ($to_charset !== false) {
		var_dump(iconv_strlen($haystk, $to_charset));
		var_dump(iconv_strrpos($haystk, $needle, $to_charset));
	} else {
		var_dump(iconv_strlen($haystk));
		var_dump(iconv_strrpos($haystk, $needle));
	}
}
foo("abecdbcdabcdef", "bcd");
foo(str_repeat("abcab", 60)."abcdb".str_repeat("adabc", 60), "abcd");
foo(str_repeat("あいうえお", 30)."いうおえあ".str_repeat("あいえおう", 30), "うお", "EUC-JP");

for ($i = 0; $i <=6; ++$i) {
	$str = str_repeat("あいうえお", 60).str_repeat('$', $i).str_repeat("あいえおう", 60);
	foo($str, '$', "ISO-2022-JP", "EUC-JP");
}

var_dump(iconv_strrpos("string", ""));
var_dump(iconv_strrpos("", "string"));

?>
--EXPECT--
int(14)
int(9)
int(605)
int(300)
int(305)
int(151)
int(600)
bool(false)
int(601)
int(300)
int(602)
int(301)
int(603)
int(302)
int(604)
int(303)
int(605)
int(304)
int(606)
int(305)
bool(false)
bool(false)
