--TEST--
json_last_error_msg() - Error location reporting with deeply nested structures
--FILE--
<?php

require_once("json_validate_requires.inc");

echo "Testing error locations in deeply nested structures\n\n";

// Error in deeply nested object
echo "Error in deeply nested object:\n";
$json = '{"a": {"b": {"c": {"d": {"e": "value}}}}}';
json_validate_trycatchdump($json);

// Error in deeply nested array
echo "\nError in deeply nested array:\n";
$json = '[[[[[1, 2, 3, 4, 5]]]]]';
json_validate_trycatchdump($json);

// Error in mixed nested structures
echo "\nError in mixed nested structures:\n";
$json = '{"arr": [{"obj": [{"key": "val"}]}]}';
json_validate_trycatchdump($json);

// Error at the end of deep nesting
echo "\nError at end of deep nesting:\n";
$json = '{"level1": {"level2": {"level3": {"level4": "value"}}}}';
json_validate_trycatchdump($json);

// Error in middle of deep nesting
echo "\nError in middle of deep nesting:\n";
$json = '{"l1": {"l2": {"l3" {"l4": "val"}}}}';
json_validate_trycatchdump($json);

// Complex structure with error
echo "\nError in complex structure:\n";
$json = '{"users": [{"name": "John", "age": 30}, {"name": "Jane", "age": 25}], "count": 2, "status": "active}';
json_validate_trycatchdump($json);

// Array of objects with error
echo "\nError in array of objects:\n";
$json = '[{"id": 1, "name": "A"}, {"id": 2, "name": "B"}, {"id": 3, "name": "C}]';
json_validate_trycatchdump($json);

// Object with array values error
echo "\nError in object with array values:\n";
$json = '{"numbers": [1, 2, 3], "strings": ["a", "b", "c"], "booleans": [true, false, true}';
json_validate_trycatchdump($json);

?>
--EXPECT--
Testing error locations in deeply nested structures

Error in deeply nested object:
bool(false)
int(3)
string(72) "Control character error, possibly incorrectly encoded near location 1:31"

Error in deeply nested array:
bool(true)
int(0)
string(8) "No error"

Error in mixed nested structures:
bool(true)
int(0)
string(8) "No error"

Error at end of deep nesting:
bool(true)
int(0)
string(8) "No error"

Error in middle of deep nesting:
bool(false)
int(4)
string(31) "Syntax error near location 1:21"

Error in complex structure:
bool(false)
int(3)
string(72) "Control character error, possibly incorrectly encoded near location 1:93"

Error in array of objects:
bool(false)
int(3)
string(72) "Control character error, possibly incorrectly encoded near location 1:68"

Error in object with array values:
bool(false)
int(2)
string(61) "State mismatch (invalid or malformed JSON) near location 1:82"

