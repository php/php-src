--TEST--
json_last_error_msg() - Error location with whitespace variations
--FILE--
<?php

require_once("json_validate_requires.inc");

echo "Testing error locations with various whitespace patterns\n\n";

// Error after spaces
echo "Error after multiple spaces:\n";
json_validate_trycatchdump('     {');

// Error after tabs
echo "\nError after tabs:\n";
json_validate_trycatchdump("\t\t\t{");

// Error after mixed whitespace
echo "\nError after mixed whitespace:\n";
json_validate_trycatchdump("  \t  {");

// Error on new line
echo "\nError on second line:\n";
json_validate_trycatchdump("{\n}x");

// Error with CRLF line endings
echo "\nError with CRLF line endings:\n";
json_validate_trycatchdump("{\r\n\"key\": \"value\r\n}");

// Error with multiple blank lines
echo "\nError after blank lines:\n";
json_validate_trycatchdump("\n\n\n{");

// Whitespace inside strings (should not affect location)
echo "\nError in string with spaces:\n";
json_validate_trycatchdump('{"key": "value with    spaces}');

// Whitespace before and after colons
echo "\nError with whitespace around colon:\n";
json_validate_trycatchdump('{"key"  :  "value}');

// Whitespace before and after commas
echo "\nError with whitespace around comma:\n";
json_validate_trycatchdump('{"a": 1  ,  "b": 2  ,  "c": 3}');

// Pretty printed JSON with error
echo "\nError in pretty printed JSON:\n";
$json = '{
  "name": "John",
  "age": 30,
  "city": "New York
}';
json_validate_trycatchdump($json);

// Heavily indented JSON with error
echo "\nError in heavily indented JSON:\n";
$json = '{
    "level1": {
        "level2": {
            "level3": {
                "value": "test
            }
        }
    }
}';
json_validate_trycatchdump($json);

// Mixed tabs and spaces with error
echo "\nError with mixed tabs and spaces:\n";
$json = "{\n\t  \"key1\": \"val1\",\n  \t\"key2\": \"val2\n}";
json_validate_trycatchdump($json);

// Whitespace-only line before error
echo "\nError after whitespace-only line:\n";
$json = "{\n    \n    \"key\": \"value\n}";
json_validate_trycatchdump($json);

// No whitespace (compact JSON) with error
echo "\nError in compact JSON:\n";
json_validate_trycatchdump('{"a":"b","c":"d","e":"f}');

// Unicode whitespace characters (if supported)
echo "\nError with regular spaces:\n";
json_validate_trycatchdump('{ "key": "value}');

?>
--EXPECT--
Testing error locations with various whitespace patterns

Error after multiple spaces:
bool(false)
int(4)
string(30) "Syntax error near location 1:7"

Error after tabs:
bool(false)
int(4)
string(30) "Syntax error near location 1:5"

Error after mixed whitespace:
bool(false)
int(4)
string(30) "Syntax error near location 1:7"

Error on second line:
bool(false)
int(4)
string(30) "Syntax error near location 2:2"

Error with CRLF line endings:
bool(false)
int(3)
string(71) "Control character error, possibly incorrectly encoded near location 2:8"

Error after blank lines:
bool(false)
int(4)
string(30) "Syntax error near location 4:2"

Error in string with spaces:
bool(false)
int(3)
string(71) "Control character error, possibly incorrectly encoded near location 1:9"

Error with whitespace around colon:
bool(false)
int(3)
string(72) "Control character error, possibly incorrectly encoded near location 1:12"

Error with whitespace around comma:
bool(true)
int(0)
string(8) "No error"

Error in pretty printed JSON:
bool(false)
int(3)
string(72) "Control character error, possibly incorrectly encoded near location 4:11"

Error in heavily indented JSON:
bool(false)
int(3)
string(72) "Control character error, possibly incorrectly encoded near location 5:26"

Error with mixed tabs and spaces:
bool(false)
int(3)
string(72) "Control character error, possibly incorrectly encoded near location 3:12"

Error after whitespace-only line:
bool(false)
int(3)
string(72) "Control character error, possibly incorrectly encoded near location 3:12"

Error in compact JSON:
bool(false)
int(3)
string(72) "Control character error, possibly incorrectly encoded near location 1:22"

Error with regular spaces:
bool(false)
int(3)
string(72) "Control character error, possibly incorrectly encoded near location 1:10"

