--TEST--
json_last_error_msg() - Error location reporting with UTF-16 surrogate pairs
--FILE--
<?php

require_once("json_validate_requires.inc");

echo "Testing error locations with UTF-16 surrogate pairs and escape sequences\n\n";

// Error after UTF-16 escaped characters
echo "Error after UTF-16 escaped emoji:\n";
json_validate_trycatchdump('{"emoji": "\uD83D\uDE00}');

// Error after multiple UTF-16 pairs
echo "\nError after multiple UTF-16 pairs:\n";
json_validate_trycatchdump('{"test": "\uD83D\uDE00\uD83C\uDF89}');

// Mixed UTF-8 and UTF-16 escapes
echo "\nError with mixed UTF-8 and UTF-16:\n";
json_validate_trycatchdump('{"mixed": "Hello \u4E16\u754C world}');

// UTF-16 in key and value
echo "\nError with UTF-16 in key:\n";
json_validate_trycatchdump('{"\u30D7\u30EC\u30B9": "value}');

// Multiple keys with UTF-16
echo "\nError with multiple UTF-16 keys:\n";
json_validate_trycatchdump('{"\u0041\u0042": "val1", "\u0043\u0044": "val2}');

// BMP characters (Basic Multilingual Plane)
echo "\nError with BMP characters:\n";
json_validate_trycatchdump('{"test": "\u0048\u0065\u006C\u006C\u006F}');

// Supplementary plane characters (beyond BMP)
echo "\nError with supplementary plane:\n";
json_validate_trycatchdump('{"music": "\uD834\uDD1E}');

// Array with UTF-16 escaped strings
echo "\nError in array with UTF-16:\n";
json_validate_trycatchdump('["\u0031", "\u0032", "\u0033}');

// Nested with UTF-16
echo "\nError in nested structure with UTF-16:\n";
json_validate_trycatchdump('{"outer": {"\u6D4B\u8BD5": "value}');

// UTF-16 with control characters
echo "\nError with UTF-16 and control chars:\n";
json_validate_trycatchdump('{"data": "\u0009\u000A\u000D}');

?>
--EXPECT--
Testing error locations with UTF-16 surrogate pairs and escape sequences

Error after UTF-16 escaped emoji:
bool(false)
int(3)
string(72) "Control character error, possibly incorrectly encoded near location 1:11"

Error after multiple UTF-16 pairs:
bool(false)
int(3)
string(72) "Control character error, possibly incorrectly encoded near location 1:10"

Error with mixed UTF-8 and UTF-16:
bool(false)
int(3)
string(72) "Control character error, possibly incorrectly encoded near location 1:11"

Error with UTF-16 in key:
bool(false)
int(3)
string(71) "Control character error, possibly incorrectly encoded near location 1:9"

Error with multiple UTF-16 keys:
bool(false)
int(3)
string(72) "Control character error, possibly incorrectly encoded near location 1:22"

Error with BMP characters:
bool(false)
int(3)
string(72) "Control character error, possibly incorrectly encoded near location 1:10"

Error with supplementary plane:
bool(false)
int(3)
string(72) "Control character error, possibly incorrectly encoded near location 1:11"

Error in array with UTF-16:
bool(false)
int(3)
string(72) "Control character error, possibly incorrectly encoded near location 1:12"

Error in nested structure with UTF-16:
bool(false)
int(3)
string(72) "Control character error, possibly incorrectly encoded near location 1:18"

Error with UTF-16 and control chars:
bool(false)
int(3)
string(72) "Control character error, possibly incorrectly encoded near location 1:10"

