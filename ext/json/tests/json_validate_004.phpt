--TEST--
json_validate() - Invalid UTF-8's
--FILE--
<?php

require_once("json_validate_requires.inc");

echo "Testing Invalid UTF-8" . PHP_EOL;


json_validate_trycatchdump("\"a\xb0b\"");
json_validate_trycatchdump("\"a\xd0\xf2b\"");
json_validate_trycatchdump("\"\x61\xf0\x80\x80\x41\"");
json_validate_trycatchdump("[\"\xc1\xc1\",\"a\"]");

json_validate_trycatchdump("\"a\xb0b\"", 512, JSON_INVALID_UTF8_IGNORE);
json_validate_trycatchdump("\"a\xd0\xf2b\"", 512, JSON_INVALID_UTF8_IGNORE);
json_validate_trycatchdump("\"\x61\xf0\x80\x80\x41\"", 512, JSON_INVALID_UTF8_IGNORE);
json_validate_trycatchdump("[\"\xc1\xc1\",\"a\"]", 512, JSON_INVALID_UTF8_IGNORE);

?>
--EXPECT--
Testing Invalid UTF-8
bool(false)
int(5)
string(73) "Malformed UTF-8 characters, possibly incorrectly encoded near character 1"
bool(false)
int(5)
string(73) "Malformed UTF-8 characters, possibly incorrectly encoded near character 1"
bool(false)
int(5)
string(73) "Malformed UTF-8 characters, possibly incorrectly encoded near character 1"
bool(false)
int(5)
string(73) "Malformed UTF-8 characters, possibly incorrectly encoded near character 2"
bool(true)
int(0)
string(8) "No error"
bool(true)
int(0)
string(8) "No error"
bool(true)
int(0)
string(8) "No error"
bool(true)
int(0)
string(8) "No error"
