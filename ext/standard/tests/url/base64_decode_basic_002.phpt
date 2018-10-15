--TEST--
Test base64_decode() function : basic functionality - strict vs non-strict with non-base64 chars.
--FILE--
<?php
/* Prototype  : proto string base64_decode(string str[, bool strict])
 * Description: Decodes string using MIME base64 algorithm
 * Source code: ext/standard/base64.c
 * Alias to functions:
 */

echo "Decode 'hello world!':\n";
$noWhiteSpace = "aGVsbG8gd29ybGQh";
var_dump(base64_decode($noWhiteSpace));
var_dump(base64_decode($noWhiteSpace, false));
var_dump(base64_decode($noWhiteSpace, true));

echo "\nWhitespace does not affect base64_decode, even with \$strict===true:\n";
$withWhiteSpace = "a GVs   bG8gd2
		 				9ybGQh";
var_dump(base64_decode($withWhiteSpace));
var_dump(base64_decode($withWhiteSpace, false));
var_dump(base64_decode($withWhiteSpace, true));

echo "\nOther chars outside the base64 alphabet are ignored when \$strict===false, but cause failure with \$strict===true:\n";
$badChars = $noWhiteSpace . '*';
var_dump(base64_decode($badChars));
var_dump(base64_decode($badChars, false));
var_dump(base64_decode($badChars, true));

echo "Done";
?>
--EXPECT--
Decode 'hello world!':
string(12) "hello world!"
string(12) "hello world!"
string(12) "hello world!"

Whitespace does not affect base64_decode, even with $strict===true:
string(12) "hello world!"
string(12) "hello world!"
string(12) "hello world!"

Other chars outside the base64 alphabet are ignored when $strict===false, but cause failure with $strict===true:
string(12) "hello world!"
string(12) "hello world!"
bool(false)
Done
