--TEST--
json_last_error_msg() - Error location reporting edge cases
--FILE--
<?php

require_once("json_validate_requires.inc");

echo "Testing error location edge cases\n\n";

// Error at very start
echo "Error at position 1:1:\n";
json_validate_trycatchdump('');

// Error at position 1:1 with single character
echo "\nError at position 1:1 with invalid char:\n";
json_validate_trycatchdump('x');

// Error after whitespace
echo "\nError after leading whitespace:\n";
json_validate_trycatchdump('   {');

// Error with only whitespace
echo "\nError with tabs and spaces:\n";
json_validate_trycatchdump("  \t  \n  ");

// Error after newlines
echo "\nError after multiple newlines:\n";
json_validate_trycatchdump("\n\n\n{");

// Long string with error at end
echo "\nError at end of long string:\n";
$longString = '{"key": "' . str_repeat('a', 1000) . '"}';
// Remove closing brace to create error
$longString = substr($longString, 0, -1);
json_validate_trycatchdump($longString);

// Error with very long key name
echo "\nError with very long key:\n";
$longKey = '{"' . str_repeat('x', 500) . '": "value}';
json_validate_trycatchdump($longKey);

// Empty object followed by error
echo "\nError after empty object:\n";
json_validate_trycatchdump('{}x');

// Empty array followed by error
echo "\nError after empty array:\n";
json_validate_trycatchdump('[]x');

// Multiple values (invalid JSON)
echo "\nError with multiple root values:\n";
json_validate_trycatchdump('{}{}');

// Number followed by invalid content
echo "\nError after valid number:\n";
json_validate_trycatchdump('123x');

// Boolean followed by invalid content
echo "\nError after valid boolean:\n";
json_validate_trycatchdump('truex');

// Null followed by invalid content
echo "\nError after valid null:\n";
json_validate_trycatchdump('nullx');

// String followed by invalid content
echo "\nError after valid string:\n";
json_validate_trycatchdump('"test"x');

// Error with mixed whitespace
echo "\nError with mixed whitespace:\n";
json_validate_trycatchdump("{\n\t\"key\": \n\t\"value\n}");

?>
--EXPECT--
Testing error location edge cases

Error at position 1:1:
bool(false)
int(4)
string(12) "Syntax error"

Error at position 1:1 with invalid char:
bool(false)
int(4)
string(30) "Syntax error near location 1:1"

Error after leading whitespace:
bool(false)
int(4)
string(30) "Syntax error near location 1:5"

Error with tabs and spaces:
bool(false)
int(4)
string(30) "Syntax error near location 2:3"

Error after multiple newlines:
bool(false)
int(4)
string(30) "Syntax error near location 4:2"

Error at end of long string:
bool(false)
int(4)
string(33) "Syntax error near location 1:1011"

Error with very long key:
bool(false)
int(3)
string(73) "Control character error, possibly incorrectly encoded near location 1:506"

Error after empty object:
bool(false)
int(4)
string(30) "Syntax error near location 1:3"

Error after empty array:
bool(false)
int(4)
string(30) "Syntax error near location 1:3"

Error with multiple root values:
bool(false)
int(4)
string(30) "Syntax error near location 1:3"

Error after valid number:
bool(false)
int(4)
string(30) "Syntax error near location 1:4"

Error after valid boolean:
bool(false)
int(4)
string(30) "Syntax error near location 1:5"

Error after valid null:
bool(false)
int(4)
string(30) "Syntax error near location 1:5"

Error after valid string:
bool(false)
int(4)
string(30) "Syntax error near location 1:7"

Error with mixed whitespace:
bool(false)
int(3)
string(71) "Control character error, possibly incorrectly encoded near location 3:2"

