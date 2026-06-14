--TEST--
json_last_error_msg() - Error location with depth errors and complex nesting
--FILE--
<?php

require_once("json_validate_requires.inc");

echo "Testing error locations with depth-related issues\n\n";

// Test max depth error with location
echo "Max depth error at specific location:\n";
$json = '{"a":{"b":{"c":{"d":{"e":{"f":{"g":{"h":{"i":{"j":"val"}}}}}}}}}}';
json_validate_trycatchdump($json, 5);

echo "\nMax depth error in array:\n";
$json = '[[[[[[[[[[10]]]]]]]]]]';
json_validate_trycatchdump($json, 5);

echo "\nMax depth error with mixed structures:\n";
$json = '[{"a":[{"b":[{"c":"val"}]}]}]';
json_validate_trycatchdump($json, 3);

// Syntax error at deep level
echo "\nSyntax error at deep nesting level:\n";
$json = '{"l1":{"l2":{"l3":{"l4":{"l5":"unclosed}}}}}';
json_validate_trycatchdump($json);

echo "\nSyntax error in deep array:\n";
$json = '[[[[["value]]]]]';
json_validate_trycatchdump($json);

// Valid deep structure followed by error
echo "\nError after valid deep structure:\n";
$json = '{"valid":{"nested":{"structure":"ok"}},"error":"unclosed}';
json_validate_trycatchdump($json);

// Multiple nested objects with error in middle
echo "\nError in middle of nested structure:\n";
$json = '{"outer":{"middle":{"inner" "value"}}}';
json_validate_trycatchdump($json);

// Array with nested objects and error
echo "\nError in array with nested objects:\n";
$json = '[{"a":{"b":"val"}},{"c":{"d":"val}}}]';
json_validate_trycatchdump($json);

// Deep structure with UTF-8 and error
echo "\nError in deep UTF-8 structure:\n";
$json = '{"外":{"中":{"内":"値}}}';
json_validate_trycatchdump($json);

// Very deep valid structure (should pass)
echo "\nValid deep structure within limit:\n";
$json = '{"a":{"b":{"c":{"d":"val"}}}}';
json_validate_trycatchdump($json, 10);

?>
--EXPECT--
Testing error locations with depth-related issues

Max depth error at specific location:
bool(false)
int(1)
string(47) "Maximum stack depth exceeded near location 1:21"

Max depth error in array:
bool(false)
int(1)
string(46) "Maximum stack depth exceeded near location 1:5"

Max depth error with mixed structures:
bool(false)
int(1)
string(46) "Maximum stack depth exceeded near location 1:7"

Syntax error at deep nesting level:
bool(false)
int(3)
string(72) "Control character error, possibly incorrectly encoded near location 1:31"

Syntax error in deep array:
bool(false)
int(3)
string(71) "Control character error, possibly incorrectly encoded near location 1:6"

Error after valid deep structure:
bool(false)
int(3)
string(72) "Control character error, possibly incorrectly encoded near location 1:48"

Error in middle of nested structure:
bool(false)
int(4)
string(31) "Syntax error near location 1:29"

Error in array with nested objects:
bool(false)
int(3)
string(72) "Control character error, possibly incorrectly encoded near location 1:30"

Error in deep UTF-8 structure:
bool(false)
int(3)
string(72) "Control character error, possibly incorrectly encoded near location 1:16"

Valid deep structure within limit:
bool(true)
int(0)
string(8) "No error"

