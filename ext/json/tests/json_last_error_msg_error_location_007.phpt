--TEST--
json_last_error_msg() - Error location with various error types
--FILE--
<?php

require_once("json_validate_requires.inc");

echo "Testing error locations with different error types\n\n";

// State mismatch errors
echo "State mismatch - expected value:\n";
json_validate_trycatchdump('{"key": }');

echo "\nState mismatch - expected key:\n";
json_validate_trycatchdump('{: "value"}');

echo "\nState mismatch - trailing comma in object:\n";
json_validate_trycatchdump('{"a": 1, "b": 2,}');

echo "\nState mismatch - trailing comma in array:\n";
json_validate_trycatchdump('[1, 2, 3,]');

// Number format errors
echo "\nInvalid number format - leading zero:\n";
json_validate_trycatchdump('{"num": 01}');

echo "\nInvalid number format - multiple decimals:\n";
json_validate_trycatchdump('{"num": 1.2.3}');

echo "\nInvalid number format - incomplete exponent:\n";
json_validate_trycatchdump('{"num": 1e}');

echo "\nInvalid number format - double sign:\n";
json_validate_trycatchdump('{"num": --1}');

// String errors
echo "\nUnclosed string:\n";
json_validate_trycatchdump('{"key": "value');

echo "\nInvalid escape sequence:\n";
json_validate_trycatchdump('{"key": "\\q"}');

echo "\nIncomplete unicode escape:\n";
json_validate_trycatchdump('{"key": "\\u123"}');

echo "\nInvalid unicode escape:\n";
json_validate_trycatchdump('{"key": "\\uGGGG"}');

// Keyword errors
echo "\nInvalid true keyword:\n";
json_validate_trycatchdump('{"val": tru}');

echo "\nInvalid false keyword:\n";
json_validate_trycatchdump('{"val": fals}');

echo "\nInvalid null keyword:\n";
json_validate_trycatchdump('{"val": nul}');

// Structure errors
echo "\nMismatched brackets - ] instead of }:\n";
json_validate_trycatchdump('{"key": "val"]');

echo "\nMismatched brackets - } instead of ]:\n";
json_validate_trycatchdump('["val"}');

echo "\nExtra closing bracket:\n";
json_validate_trycatchdump('{"key": "val"}}');

echo "\nMissing comma between elements:\n";
json_validate_trycatchdump('[1 2 3]');

echo "\nMissing comma between object properties:\n";
json_validate_trycatchdump('{"a": 1 "b": 2}');

?>
--EXPECT--
Testing error locations with different error types

State mismatch - expected value:
bool(false)
int(4)
string(30) "Syntax error near location 1:9"

State mismatch - expected key:
bool(false)
int(4)
string(30) "Syntax error near location 1:2"

State mismatch - trailing comma in object:
bool(false)
int(4)
string(31) "Syntax error near location 1:17"

State mismatch - trailing comma in array:
bool(false)
int(4)
string(31) "Syntax error near location 1:10"

Invalid number format - leading zero:
bool(false)
int(4)
string(31) "Syntax error near location 1:10"

Invalid number format - multiple decimals:
bool(false)
int(4)
string(31) "Syntax error near location 1:12"

Invalid number format - incomplete exponent:
bool(false)
int(4)
string(31) "Syntax error near location 1:10"

Invalid number format - double sign:
bool(false)
int(4)
string(30) "Syntax error near location 1:9"

Unclosed string:
bool(false)
int(3)
string(71) "Control character error, possibly incorrectly encoded near location 1:9"

Invalid escape sequence:
bool(false)
int(4)
string(30) "Syntax error near location 1:9"

Incomplete unicode escape:
bool(false)
int(4)
string(30) "Syntax error near location 1:9"

Invalid unicode escape:
bool(false)
int(4)
string(30) "Syntax error near location 1:9"

Invalid true keyword:
bool(false)
int(4)
string(30) "Syntax error near location 1:9"

Invalid false keyword:
bool(false)
int(4)
string(30) "Syntax error near location 1:9"

Invalid null keyword:
bool(false)
int(4)
string(30) "Syntax error near location 1:9"

Mismatched brackets - ] instead of }:
bool(false)
int(2)
string(61) "State mismatch (invalid or malformed JSON) near location 1:14"

Mismatched brackets - } instead of ]:
bool(false)
int(2)
string(60) "State mismatch (invalid or malformed JSON) near location 1:7"

Extra closing bracket:
bool(false)
int(4)
string(31) "Syntax error near location 1:15"

Missing comma between elements:
bool(false)
int(4)
string(30) "Syntax error near location 1:4"

Missing comma between object properties:
bool(false)
int(4)
string(30) "Syntax error near location 1:9"

