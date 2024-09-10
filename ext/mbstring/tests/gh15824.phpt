--TEST--
GH-15824 (ValueError: mb_detect_encoding(): Argument #2 ($encodings) contains invalid encoding "UTF8")
--EXTENSIONS--
mbstring
--FILE--
<?php
echo "== alias name ==\n";
var_dump(mb_detect_encoding('abc', 'UTF8, ASCII'));
var_dump(mb_detect_encoding('こんにちは', 'UTF8, ASCII'));
var_dump(mb_detect_encoding('こんにちは', 'ASCII, UTF8'));
var_dump(mb_detect_encoding("\xC5", 'US-ASCII, LATIN4'));
var_dump(mb_detect_encoding("\xC5", 'US-ASCII, cyrillic'));
/* 0x9D is not located in CP1254 */
var_dump(mb_detect_encoding("\x9D", 'US-ASCII, CP1254, cyrillic'));
var_dump(mb_detect_encoding("\x9D", 'US-ASCII, CP1254, cyrillic', false));

echo "== mime name ==\n";
var_dump(mb_detect_encoding('abc', 'ANSI_X3.4-1968, ISO-8859-1'));
var_dump(mb_detect_encoding('abc', 'CP50220, ANSI_X3.4-1968'));
/* last comma is not mistake, intentionally */
var_dump(mb_detect_encoding(bin2hex('1b24422422242424262428242a1b2842'), 'CP50220, ANSI_X3.4-1968,', false));
var_dump(mb_detect_encoding('😄', 'US-ASCII, UTF-8-Mobile#KDDI-B, UTF-8'));
?>
--EXPECT--
== alias name ==
string(5) "UTF-8"
string(5) "UTF-8"
string(5) "UTF-8"
string(10) "ISO-8859-4"
string(10) "ISO-8859-5"
string(10) "ISO-8859-5"
string(10) "ISO-8859-5"
== mime name ==
string(5) "ASCII"
string(7) "CP50220"
string(7) "CP50220"
string(19) "UTF-8-Mobile#KDDI-B"
