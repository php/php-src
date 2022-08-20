--TEST--
is_json() - Error handling
--FILE--
<?php

require_once("is_json_requires.inc");

echo "*** Flag JSON_THROW_ON_ERROR Off" . PHP_EOL;

is_json_trycatchdump(null);
is_json_trycatchdump("");
is_json_trycatchdump("-");
is_json_trycatchdump("", -1);
is_json_trycatchdump('{"key1":"value1", "key2":"value2"}', 1);
is_json_trycatchdump('{"key1":"value1", "key2":"value2"}', 2);
is_json_trycatchdump("-", 0);
is_json_trycatchdump("-", 512, JSON_BIGINT_AS_STRING);

echo PHP_EOL . "*** Flag JSON_THROW_ON_ERROR On" . PHP_EOL;

is_json_trycatchdump(null, 512, JSON_THROW_ON_ERROR);
is_json_trycatchdump("", 512, JSON_THROW_ON_ERROR);
is_json_trycatchdump("-", 512, JSON_THROW_ON_ERROR);
is_json_trycatchdump("", -1, JSON_THROW_ON_ERROR);
is_json_trycatchdump('{"key1":"value1", "key2":"value2"}', 1, JSON_THROW_ON_ERROR);
is_json_trycatchdump('{"key1":"value1", "key2":"value2"}', 2, JSON_THROW_ON_ERROR);
is_json_trycatchdump("-", 0, JSON_THROW_ON_ERROR);
is_json_trycatchdump("-", 512, JSON_THROW_ON_ERROR | JSON_BIGINT_AS_STRING);

?>
--EXPECTF--
*** Flag JSON_THROW_ON_ERROR Off

Deprecated: is_json(): Passing null to parameter #1 ($json) of type string is deprecated in %s on line %d
bool(false)
int(4)
string(12) "Syntax error"
bool(false)
int(4)
string(12) "Syntax error"
bool(false)
int(4)
string(12) "Syntax error"
bool(false)
int(4)
string(12) "Syntax error"
bool(false)
int(1)
string(28) "Maximum stack depth exceeded"
bool(true)
int(0)
string(8) "No error"
Error: 0 is_json(): Argument #2 ($depth) must be greater than 0
int(0)
string(8) "No error"
Error: 0 is_json(): Argument #3 ($flags) must be a valid flag (JSON_THROW_ON_ERROR, JSON_INVALID_UTF8_IGNORE)
int(0)
string(8) "No error"

*** Flag JSON_THROW_ON_ERROR On

Deprecated: is_json(): Passing null to parameter #1 ($json) of type string is deprecated in %s on line %d
JsonException: 4 Syntax error
int(0)
string(8) "No error"
JsonException: 4 Syntax error
int(0)
string(8) "No error"
JsonException: 4 Syntax error
int(0)
string(8) "No error"
JsonException: 4 Syntax error
int(0)
string(8) "No error"
JsonException: 1 Maximum stack depth exceeded
int(0)
string(8) "No error"
bool(true)
int(0)
string(8) "No error"
Error: 0 is_json(): Argument #2 ($depth) must be greater than 0
int(0)
string(8) "No error"
JsonException: 4 Syntax error
int(0)
string(8) "No error"
