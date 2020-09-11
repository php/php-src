--TEST--
Exhaustive test of mUTF-7 (IMAP) encoding verification and conversion
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php
include('encoding_tests.inc');
mb_substitute_character(0x25); // '%'

function utf16BE($utf8) {
	return mb_convert_encoding($utf8, 'UTF-16BE', 'UTF-8');
}

function mBase64($str) {
	return str_replace('=', '', str_replace('/', ',', base64_encode($str)));
}

function testValid($from, $to, $bothWays = true) {
	testValidString($from, $to, 'UTF7-IMAP', 'UTF-8', $bothWays);
}
function testInvalid($from, $to) {
	testInvalidString($from, $to, 'UTF7-IMAP', 'UTF-8');
}

/* An empty string is valid */
testValid("", "");
echo "Identification passes on empty string... good start!\n";

/* Identification and conversion of ASCII characters (minus &) */
for ($i = 0x20; $i <= 0x7E; $i++) {
	if ($i == 0x26) // '&'
		continue;
	testValid(chr($i), chr($i));
}
echo "Testing all valid single-character ASCII strings... check!\n";

/* Identification and conversion of non-ASCII characters */
for ($i = 0; $i < 0x20; $i++)
	testInvalid(chr($i), "%");
for ($i = 0x7F; $i < 256; $i++)
	testInvalid(chr($i), "%");
echo "Non-ASCII characters convert to illegal char marker... yes!\n";

/* Identification of '&' when Base-64 encoded */
testValid("&" . mBase64(utf16BE("&")) . "-", "&", false);
echo "& can be Base64-encoded... yes!\n";

/* Identification of unterminated & section */
identifyInvalidString("&", 'UTF7-IMAP');
identifyInvalidString("abc&", 'UTF7-IMAP');
identifyInvalidString("&" . mBase64(utf16BE("ハムサンドイッチ")), 'UTF7-IMAP');
echo "Testing unterminated & sections... yep!\n";

/* Identification of null shifts (& immediately after -)
 *
 * This is illegal according to the spec for mUTF-7 (IMAP), but currently we are letting
 * it pass... among other things, this makes it possible to concatenate UTF-7-IMAP
 * strings naively without the concatenated strings being treated as 'invalid'
 *
 * If ever we want to enforce this part of the spec, uncomment the following test */
/*
identifyInvalidString("&" . mBase64(utf16BE("肉包子")) . "-&" . mBase64(utf16BE("冰淇淋")) . "-", 'UTF7-IMAP');
echo "Testing consecutive & sections which should have been merged... yep!\n";
*/

/* Conversion of Base64-encoded ASCII characters (excluding &)
 * These should be treated as erroneous and mb_substitute_character should apply */
for ($i = 0x20; $i <= 0x7E; $i++) {
	if ($i == 0x26) // '&'
		continue;
	testInvalid("&" . mBase64(utf16BE(chr($i))) . "-", "%");
}
echo "Testing ASCII characters which are Base64-encoded... great!\n";

/* Conversion of & encoded as &- */
testValid("&-", "&");
testValid("abc&-", "abc&");
testValid("&-.&-", "&.&");
echo "Testing valid strings which use '&-' for '&'... good!\n";

/* Identification of & sections containing non-Base64 */

/* We'll use 6 character strings as a test, since 6 UTF-16 characters is just enough
 * to fit perfectly in Base64 encoding, with no padding */
$testString = mBase64(utf16BE("我是打酱油的"));
if (strlen($testString) != 16)
	die("Erk!!");
for ($i = 0; $i < 256; $i++) {
	if ($i >= 0x30 && $i <= 0x39) // '0'..'9'
		continue;
	if ($i >= 0x41 && $i <= 0x5A) // 'A'..'Z'
		continue;
	if ($i >= 0x61 && $i <= 0x7A) // 'a'..'z'
		continue;
	if ($i == 0x2B || $i == 0x2C) // '+' or ','
		continue;
	if ($i == 0x2D) // '-'... this will be interpreted as ending the Base64 section
		continue;
	identifyInvalidString("&" . substr($testString, 0, 11) . chr($i) . "-", 'UTF7-IMAP');
}
echo "Identification fails when Base64 sections contain non-Base64 bytes... right!\n";

/* Tell me, please, how many ways can UTF-16BE text get messed up?
 * Why, that's elementary... */

/* 1. The second half of a surrogate pair could come first, */
$testString = mb_convert_encoding("\x00\x01\x04\x00", 'UTF-16BE', 'UTF-32BE');
if (strlen($testString) != 4)
	die("Ouch!");
$testString = substr($testString, 2, 2) . substr($testString, 0, 2);
identifyInvalidString("&" . mBase64($testString) . "-", 'UTF7-IMAP');

/* ...and we should detect this wherever it occurs */
$singleChar = mb_convert_encoding("１", 'UTF-16BE', 'ASCII');
$doubleChar = mb_convert_encoding("\x00\x01\x04\x01", 'UTF-16BE', 'UTF-32BE');
if (strlen($doubleChar) != 4)
	die("That was supposed to be a surrogate pair");
identifyInvalidString("&" . mBase64($singleChar . $testString) . "-", 'UTF7-IMAP');
identifyInvalidString("&" . mBase64($singleChar . $singleChar . $testString) . "-", 'UTF7-IMAP');
identifyInvalidString("&" . mBase64($singleChar . $singleChar . $singleChar . $testString) . "-", 'UTF7-IMAP');
identifyInvalidString("&" . mBase64($doubleChar . $testString) . "-", 'UTF7-IMAP');
identifyInvalidString("&" . mBase64($singleChar . $doubleChar . $testString) . "-", 'UTF7-IMAP');
identifyInvalidString("&" . mBase64($singleChar . $singleChar . $doubleChar . $testString) . "-", 'UTF7-IMAP');

/* 2. The first half of a surrogate pair might be followed by an invalid 2nd part, */
$testString = mb_convert_encoding("\x00\x01\x04\x00", 'UTF-16BE', 'UTF-32BE');
$testString = substr($testString, 0, 2) . mb_convert_encoding("a", 'UTF-16BE', 'ASCII');
identifyInvalidString("&" . mBase64($testString) . "-", 'UTF7-IMAP');

/* ...and we should also detect that wherever it occurs... */
identifyInvalidString("&" . mBase64($singleChar . $testString) . "-", 'UTF7-IMAP');
identifyInvalidString("&" . mBase64($singleChar . $singleChar . $testString) . "-", 'UTF7-IMAP');
identifyInvalidString("&" . mBase64($doubleChar . $testString) . "-", 'UTF7-IMAP');

/* 3. The first half of a surrogate pair could come at the end of the string, */
$testString = mb_convert_encoding("\x00\x01\x04\x00", 'UTF-16BE', 'UTF-32BE');
identifyInvalidString("&" . mBase64(substr($testString, 0, 2)) . "-", 'UTF7-IMAP');
identifyInvalidString("&" . mBase64($singleChar . substr($testString, 0, 2)) . "-", 'UTF7-IMAP');
identifyInvalidString("&" . mBase64($singleChar . $singleChar . substr($testString, 0, 2)) . "-", 'UTF7-IMAP');

/* 4. Or, it could have an odd number of bytes in it! */
$testString = utf16BE("ドーナツ");
$testString = substr($testString, 0, strlen($testString) - 1);
identifyInvalidString("&" . mBase64($testString) . "-", 'UTF7-IMAP');

/* And there is one bonus way to discombobulate your UTF-16BE when it is Base64-encoded...
 * The Base64 might not decode to an integral number of bytes
 * Or, equivalently... it might not be padded with zeroes (as the RFC requires) */
$testString = utf16BE("☺⛑");
if (strlen($testString) != 4)
	die("No good");
$encoded = mBase64($testString);
if (strlen($encoded) != 6)
	die("Don't like that");
/* Mess up the padding by replacing the last Base64 character with ',',
 * which represents 63 (a number with a 1 in the last bit) */
identifyInvalidString("&" . substr($encoded, 0, strlen($encoded) - 1) . ",-", 'UTF7-IMAP');

echo "Identification fails when UTF-16 text is invalid... no sweat!\n";

/* OK, let's try valid Base64-encoded text now */

/* 2-byte char */
testValid("&" . mBase64(utf16BE("☺")) . "-", "☺");
/* 2 + 2 */
testValid("&" . mBase64(utf16BE("饺子")) . "-", "饺子");
/* 2 + 2 + 2 */
testValid("&" . mBase64(utf16BE("１２３")) . "-", "１２３");
/* 2 + 2 + 2 + 2 */
testValid("&" . mBase64(utf16BE("ﾰﾱﾲﾳ")) . "-", "ﾰﾱﾲﾳ");
/* 4 */
$longChar1 = mb_convert_encoding("\x00\x01\x04\x01", 'UTF-16BE', 'UTF-32BE');
$longChar2 = mb_convert_encoding("\x00\x01\x04\x01", 'UTF-8', 'UTF-32BE');
testValid("&" . mBase64($longChar1) . "-", $longChar2);
/* 2 + 4 */
testValid("&" . mBase64(utf16BE("饼") . $longChar1) . "-", "饼" . $longChar2);
/* 4 + 2 */
testValid("&" . mBase64($longChar1 . utf16BE("饼")) . "-", $longChar2 . "饼");
/* 2 + 4 + 2 */
testValid("&" . mBase64(utf16BE("☺") . $longChar1 . utf16BE("饼")) . "-", "☺" . $longChar2 . "饼");
/* 2 + 2 + 4 */
testValid("&" . mBase64(utf16BE("西瓜") . $longChar1) . "-", "西瓜" . $longChar2);
/* 2 + 2 + 4 + 2 */
testValid("&" . mBase64(utf16BE("西瓜") . $longChar1 . utf16BE("☺")) . "-", "西瓜" . $longChar2 . "☺");
/* 2 + 2 + 4 + 4 */
testValid("&" . mBase64(utf16BE("西瓜") . $longChar1 . $longChar1) . "-", "西瓜" . $longChar2 . $longChar2);
/* 2 + 2 + 2 + 4 */
testValid("&" . mBase64(utf16BE("西红柿") . $longChar1) . "-", "西红柿" . $longChar2);

/* Multiple sections of valid ASCII _and_ Base64-encoded text */
testValid("123&" . mBase64(utf16BE("１２３")) . "-abc&" . mBase64(utf16BE("☺")) . "-.", "123１２３abc☺.");

echo "Identification and conversion of valid text is working... perfect!\n";

?>
--EXPECT--
Identification passes on empty string... good start!
Testing all valid single-character ASCII strings... check!
Non-ASCII characters convert to illegal char marker... yes!
& can be Base64-encoded... yes!
Testing unterminated & sections... yep!
Testing ASCII characters which are Base64-encoded... great!
Testing valid strings which use '&-' for '&'... good!
Identification fails when Base64 sections contain non-Base64 bytes... right!
Identification fails when UTF-16 text is invalid... no sweat!
Identification and conversion of valid text is working... perfect!
