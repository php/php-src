--TEST--
json_last_error_msg() - Error location with mixed UTF-8 multi-byte characters
--FILE--
<?php

require_once("json_validate_requires.inc");

echo "Testing error locations with various UTF-8 multi-byte character widths\n\n";

// 2-byte UTF-8 characters (Latin Extended)
echo "Error with 2-byte UTF-8 (Latin Extended):\n";
json_validate_trycatchdump('{"café": "value}');

echo "\nError with 2-byte UTF-8 (Greek):\n";
json_validate_trycatchdump('{"Ελληνικά": "value}');

echo "\nError with 2-byte UTF-8 (Cyrillic):\n";
json_validate_trycatchdump('{"Привет": "мир}');

// 3-byte UTF-8 characters (CJK)
echo "\nError with 3-byte UTF-8 (Chinese):\n";
json_validate_trycatchdump('{"中文": "测试}');

echo "\nError with 3-byte UTF-8 (Japanese Hiragana):\n";
json_validate_trycatchdump('{"ひらがな": "テスト}');

echo "\nError with 3-byte UTF-8 (Japanese Katakana):\n";
json_validate_trycatchdump('{"カタカナ": "テスト}');

echo "\nError with 3-byte UTF-8 (Korean):\n";
json_validate_trycatchdump('{"한글": "테스트}');

// 4-byte UTF-8 characters (Emoji and special symbols)
echo "\nError with 4-byte UTF-8 (Emoji faces):\n";
json_validate_trycatchdump('{"emoji": "😀😃😄😁}');

echo "\nError with 4-byte UTF-8 (Emoji objects):\n";
json_validate_trycatchdump('{"objects": "🎉🎊🎁🎈}');

echo "\nError with 4-byte UTF-8 (Mathematical symbols):\n";
json_validate_trycatchdump('{"math": "𝕒𝕓𝕔𝕕}');

// Mixed byte-width characters
echo "\nError with mixed 1-2-3 byte UTF-8:\n";
json_validate_trycatchdump('{"mix": "Aéø中文}');

echo "\nError with mixed 2-3-4 byte UTF-8:\n";
json_validate_trycatchdump('{"mix": "Привет中文😀}');

echo "\nError with all byte widths:\n";
json_validate_trycatchdump('{"all": "Aéø中文😀}');

// UTF-8 in keys at different positions
echo "\nError with UTF-8 key at start:\n";
json_validate_trycatchdump('{"🔑": "value}');

echo "\nError with multiple UTF-8 keys:\n";
json_validate_trycatchdump('{"キー1": "値1", "キー2": "値2", "キー3": "値3}');

// Array with mixed UTF-8
echo "\nError in array with mixed UTF-8:\n";
json_validate_trycatchdump('["ASCII", "café", "中文", "😀}');

// Complex nested structure with UTF-8
echo "\nError in nested structure with various UTF-8:\n";
json_validate_trycatchdump('{"外层": {"中层": {"内层": "值}}}');

// UTF-8 with special combining characters
echo "\nError with combining diacritical marks:\n";
json_validate_trycatchdump('{"tëst": "vâlue}');

// Right-to-left languages
echo "\nError with Hebrew:\n";
json_validate_trycatchdump('{"שלום": "עולם}');

echo "\nError with Arabic with diacritics:\n";
json_validate_trycatchdump('{"مرحبا": "عالم}');

?>
--EXPECT--
Testing error locations with various UTF-8 multi-byte character widths

Error with 2-byte UTF-8 (Latin Extended):
bool(false)
int(3)
string(72) "Control character error, possibly incorrectly encoded near location 1:10"

Error with 2-byte UTF-8 (Greek):
bool(false)
int(3)
string(72) "Control character error, possibly incorrectly encoded near location 1:14"

Error with 2-byte UTF-8 (Cyrillic):
bool(false)
int(3)
string(72) "Control character error, possibly incorrectly encoded near location 1:12"

Error with 3-byte UTF-8 (Chinese):
bool(false)
int(3)
string(71) "Control character error, possibly incorrectly encoded near location 1:8"

Error with 3-byte UTF-8 (Japanese Hiragana):
bool(false)
int(3)
string(72) "Control character error, possibly incorrectly encoded near location 1:10"

Error with 3-byte UTF-8 (Japanese Katakana):
bool(false)
int(3)
string(72) "Control character error, possibly incorrectly encoded near location 1:10"

Error with 3-byte UTF-8 (Korean):
bool(false)
int(3)
string(71) "Control character error, possibly incorrectly encoded near location 1:8"

Error with 4-byte UTF-8 (Emoji faces):
bool(false)
int(3)
string(72) "Control character error, possibly incorrectly encoded near location 1:11"

Error with 4-byte UTF-8 (Emoji objects):
bool(false)
int(3)
string(72) "Control character error, possibly incorrectly encoded near location 1:13"

Error with 4-byte UTF-8 (Mathematical symbols):
bool(false)
int(3)
string(72) "Control character error, possibly incorrectly encoded near location 1:10"

Error with mixed 1-2-3 byte UTF-8:
bool(false)
int(3)
string(71) "Control character error, possibly incorrectly encoded near location 1:9"

Error with mixed 2-3-4 byte UTF-8:
bool(false)
int(3)
string(71) "Control character error, possibly incorrectly encoded near location 1:9"

Error with all byte widths:
bool(false)
int(3)
string(71) "Control character error, possibly incorrectly encoded near location 1:9"

Error with UTF-8 key at start:
bool(false)
int(3)
string(71) "Control character error, possibly incorrectly encoded near location 1:7"

Error with multiple UTF-8 keys:
bool(false)
int(3)
string(72) "Control character error, possibly incorrectly encoded near location 1:35"

Error in array with mixed UTF-8:
bool(false)
int(3)
string(72) "Control character error, possibly incorrectly encoded near location 1:25"

Error in nested structure with various UTF-8:
bool(false)
int(3)
string(72) "Control character error, possibly incorrectly encoded near location 1:22"

Error with combining diacritical marks:
bool(false)
int(3)
string(72) "Control character error, possibly incorrectly encoded near location 1:10"

Error with Hebrew:
bool(false)
int(3)
string(72) "Control character error, possibly incorrectly encoded near location 1:10"

Error with Arabic with diacritics:
bool(false)
int(3)
string(72) "Control character error, possibly incorrectly encoded near location 1:11"

