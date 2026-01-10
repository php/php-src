--TEST--
Test base64_decode() function : basic functionality - strict vs non-strict with non-base64 chars.
--FILE--
<?php
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

echo "\nTests on long string for SIMD\n";
$noWhiteSpace = "UEhQIGlzIGEgcG9wdWxhciBnZW5lcmFsLXB1cnBvc2Ugc2NyaXB0aW5nIGxhbmd1YWdlIHRoYXQgaXMgZXNwZWNpYWxseSBzdWl0ZWQgdG8gd2ViIGRldmVsb3BtZW50";
var_dump(base64_decode($noWhiteSpace));
var_dump(base64_decode($noWhiteSpace, false));
var_dump(base64_decode($noWhiteSpace, true));
$withWhiteSpace = "UEhQIGlzIGE gcG9wdWxhciBnZW5lcmFsLXB1cnBvc2Ugc2NyaXB0aW5nIGxhbmd1YWdl IHRoYXQga
                        XMgZXNwZWNpYWxseSBzdWl0ZWQgdG8gd2ViIGRldmVsb3BtZW50";
var_dump(base64_decode($withWhiteSpace));
var_dump(base64_decode($withWhiteSpace, false));
var_dump(base64_decode($withWhiteSpace, true));
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

Tests on long string for SIMD
string(96) "PHP is a popular general-purpose scripting language that is especially suited to web development"
string(96) "PHP is a popular general-purpose scripting language that is especially suited to web development"
string(96) "PHP is a popular general-purpose scripting language that is especially suited to web development"
string(96) "PHP is a popular general-purpose scripting language that is especially suited to web development"
string(96) "PHP is a popular general-purpose scripting language that is especially suited to web development"
string(96) "PHP is a popular general-purpose scripting language that is especially suited to web development"
string(96) "PHP is a popular general-purpose scripting language that is especially suited to web development"
string(96) "PHP is a popular general-purpose scripting language that is especially suited to web development"
bool(false)
Done
