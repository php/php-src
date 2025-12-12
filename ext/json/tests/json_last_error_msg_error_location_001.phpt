--TEST--
json_last_error_msg() - Error location reporting with ASCII characters
--FILE--
<?php

require_once("json_validate_requires.inc");

echo "Testing errors at various locations with ASCII characters\n\n";

// Error at start
echo "Error at position 1:1:\n";
json_validate_trycatchdump("-");

// Error in middle of simple object
echo "\nError at position 1:10:\n";
json_validate_trycatchdump('{"name": "value}');

// Missing colon
echo "\nError at position 1:9:\n";
json_validate_trycatchdump('{"name" "value"}');

// Extra comma
echo "\nError at position 1:16:\n";
json_validate_trycatchdump('{"name": "val",}');

// Missing closing bracket in array
echo "\nError at position 1:15:\n";
json_validate_trycatchdump('[1, 2, 3, 4, 5');

// Invalid value
echo "\nError at position 1:10:\n";
json_validate_trycatchdump('{"test": undefined}');

// Trailing comma in array
echo "\nError at position 1:7:\n";
json_validate_trycatchdump('[1, 2,]');

// Single quote instead of double quote
echo "\nError at position 1:2:\n";
json_validate_trycatchdump("{'key': 'value'}");

// Missing closing brace
echo "\nError at position 1:16:\n";
json_validate_trycatchdump('{"key": "value"');

// Double comma
echo "\nError at position 1:4:\n";
json_validate_trycatchdump('[1,, 2]');

// Invalid escape sequence
echo "\nError at position 1:10:\n";
json_validate_trycatchdump('{"test": "\x"}');

// Unescaped control character
echo "\nError at position 1:10:\n";
json_validate_trycatchdump('{"test": "' . "\n" . '"}');

?>
--EXPECT--
Testing errors at various locations with ASCII characters

Error at position 1:1:
bool(false)
int(4)
string(30) "Syntax error near location 1:1"

Error at position 1:10:
bool(false)
int(3)
string(72) "Control character error, possibly incorrectly encoded near location 1:10"

Error at position 1:9:
bool(false)
int(4)
string(30) "Syntax error near location 1:9"

Error at position 1:16:
bool(false)
int(4)
string(31) "Syntax error near location 1:16"

Error at position 1:15:
bool(false)
int(4)
string(31) "Syntax error near location 1:15"

Error at position 1:10:
bool(false)
int(4)
string(31) "Syntax error near location 1:10"

Error at position 1:7:
bool(false)
int(4)
string(30) "Syntax error near location 1:7"

Error at position 1:2:
bool(false)
int(4)
string(30) "Syntax error near location 1:2"

Error at position 1:16:
bool(false)
int(4)
string(31) "Syntax error near location 1:16"

Error at position 1:4:
bool(false)
int(4)
string(30) "Syntax error near location 1:4"

Error at position 1:10:
bool(false)
int(4)
string(31) "Syntax error near location 1:10"

Error at position 1:10:
bool(false)
int(3)
string(72) "Control character error, possibly incorrectly encoded near location 1:10"

