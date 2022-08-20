--TEST--
is_json() - Invalid UTF-8's
--FILE--
<?php

require_once("is_json_requires.inc");

echo "Testing Invalid UTF-8" . PHP_EOL;


is_json_trycatchdump("\"a\xb0b\"");
is_json_trycatchdump("\"a\xd0\xf2b\"");
is_json_trycatchdump("\"\x61\xf0\x80\x80\x41\"");
is_json_trycatchdump("[\"\xc1\xc1\",\"a\"]");

is_json_trycatchdump("\"a\xb0b\"", 512, JSON_THROW_ON_ERROR);
is_json_trycatchdump("\"a\xd0\xf2b\"", 512, JSON_THROW_ON_ERROR);
is_json_trycatchdump("\"\x61\xf0\x80\x80\x41\"", 512, JSON_THROW_ON_ERROR);
is_json_trycatchdump("[\"\xc1\xc1\",\"a\"]", 512, JSON_THROW_ON_ERROR);

is_json_trycatchdump("\"a\xb0b\"", 512, JSON_INVALID_UTF8_IGNORE);
is_json_trycatchdump("\"a\xd0\xf2b\"", 512, JSON_INVALID_UTF8_IGNORE);
is_json_trycatchdump("\"\x61\xf0\x80\x80\x41\"", 512, JSON_INVALID_UTF8_IGNORE);
is_json_trycatchdump("[\"\xc1\xc1\",\"a\"]", 512, JSON_INVALID_UTF8_IGNORE);

is_json_trycatchdump("\"a\xb0b\"", 512, JSON_THROW_ON_ERROR | JSON_INVALID_UTF8_IGNORE);
is_json_trycatchdump("\"a\xd0\xf2b\"", 512, JSON_THROW_ON_ERROR | JSON_INVALID_UTF8_IGNORE);
is_json_trycatchdump("\"\x61\xf0\x80\x80\x41\"", 512, JSON_THROW_ON_ERROR | JSON_INVALID_UTF8_IGNORE);
is_json_trycatchdump("[\"\xc1\xc1\",\"a\"]", 512, JSON_THROW_ON_ERROR | JSON_INVALID_UTF8_IGNORE);

?>
--EXPECTF--
Testing Invalid UTF-8
bool(false)
int(5)
string(56) "Malformed UTF-8 characters, possibly incorrectly encoded"
bool(false)
int(5)
string(56) "Malformed UTF-8 characters, possibly incorrectly encoded"
bool(false)
int(5)
string(56) "Malformed UTF-8 characters, possibly incorrectly encoded"
bool(false)
int(5)
string(56) "Malformed UTF-8 characters, possibly incorrectly encoded"
JsonException: 5 Malformed UTF-8 characters, possibly incorrectly encoded
int(5)
string(56) "Malformed UTF-8 characters, possibly incorrectly encoded"
JsonException: 5 Malformed UTF-8 characters, possibly incorrectly encoded
int(5)
string(56) "Malformed UTF-8 characters, possibly incorrectly encoded"
JsonException: 5 Malformed UTF-8 characters, possibly incorrectly encoded
int(5)
string(56) "Malformed UTF-8 characters, possibly incorrectly encoded"
JsonException: 5 Malformed UTF-8 characters, possibly incorrectly encoded
int(5)
string(56) "Malformed UTF-8 characters, possibly incorrectly encoded"
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
