--TEST--
json_last_error_msg() - Error location reporting with Unicode UTF-8 characters
--FILE--
<?php

require_once("json_validate_requires.inc");

echo "Testing error locations with Unicode UTF-8 characters\n\n";

// Error after UTF-8 characters (Japanese)
echo "Error after Japanese characters:\n";
json_validate_trycatchdump('{"プレスキット": "value}');

// Error after UTF-8 characters (Russian)
echo "\nError after Russian characters:\n";
json_validate_trycatchdump('{"имя": "значение}');

// Error after UTF-8 characters (Chinese)
echo "\nError after Chinese characters:\n";
json_validate_trycatchdump('{"名字": "值}');

// Error after UTF-8 characters (Arabic)
echo "\nError after Arabic characters:\n";
json_validate_trycatchdump('{"اسم": "قيمة}');

// Error after UTF-8 characters (Emoji)
echo "\nError after Emoji:\n";
json_validate_trycatchdump('{"emoji": "😀🎉}');

// Mixed ASCII and UTF-8
echo "\nError in mixed ASCII and UTF-8:\n";
json_validate_trycatchdump('{"name": "John", "город": "Москва}');

// UTF-8 with escaped sequences
echo "\nError with UTF-8 escaped sequences:\n";
json_validate_trycatchdump('{"test": "\u30d7\u30ec\u30b9}');

// Multiple UTF-8 keys with error
echo "\nError in object with multiple UTF-8 keys:\n";
json_validate_trycatchdump('{"キー1": "値1", "キー2": "値2}');

// Array with UTF-8 and error
echo "\nError in array with UTF-8 strings:\n";
json_validate_trycatchdump('["文字列1", "文字列2", "文字列3}');

// Nested object with UTF-8 and error
echo "\nError in nested object with UTF-8:\n";
json_validate_trycatchdump('{"外部": {"内部": "値}');

?>
--EXPECT--
Testing error locations with Unicode UTF-8 characters

Error after Japanese characters:
bool(false)
int(3)
string(72) "Control character error, possibly incorrectly encoded near location 1:12"

Error after Russian characters:
bool(false)
int(3)
string(71) "Control character error, possibly incorrectly encoded near location 1:9"

Error after Chinese characters:
bool(false)
int(3)
string(71) "Control character error, possibly incorrectly encoded near location 1:8"

Error after Arabic characters:
bool(false)
int(3)
string(71) "Control character error, possibly incorrectly encoded near location 1:9"

Error after Emoji:
bool(false)
int(3)
string(72) "Control character error, possibly incorrectly encoded near location 1:11"

Error in mixed ASCII and UTF-8:
bool(false)
int(3)
string(72) "Control character error, possibly incorrectly encoded near location 1:27"

Error with UTF-8 escaped sequences:
bool(false)
int(3)
string(72) "Control character error, possibly incorrectly encoded near location 1:10"

Error in object with multiple UTF-8 keys:
bool(false)
int(3)
string(72) "Control character error, possibly incorrectly encoded near location 1:22"

Error in array with UTF-8 strings:
bool(false)
int(3)
string(72) "Control character error, possibly incorrectly encoded near location 1:18"

Error in nested object with UTF-8:
bool(false)
int(3)
string(72) "Control character error, possibly incorrectly encoded near location 1:15"

