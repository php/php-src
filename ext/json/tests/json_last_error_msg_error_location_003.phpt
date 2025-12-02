--TEST--
json_last_error_msg() - Error location reporting with multi-line JSON
--FILE--
<?php

require_once("json_validate_requires.inc");

echo "Testing error locations in multi-line JSON\n\n";

// Error on line 2
echo "Error on line 2, column 13:\n";
$json = '{
    "name": "value
}';
json_validate_trycatchdump($json);

// Error on line 3
echo "\nError on line 3, column 12:\n";
$json = '{
    "key1": "value1",
    "key2" "value2"
}';
json_validate_trycatchdump($json);

// Error on line 5 in nested structure
echo "\nError on line 5, column 26:\n";
$json = '{
    "outer": {
        "inner": {
            "deep": {
                "value": "unclosed
            }
        }
    }
}';
json_validate_trycatchdump($json);

// Error on last line
echo "\nError on line 7, column 1:\n";
$json = '{
    "key1": "value1",
    "key2": "value2",
    "key3": "value3",
    "key4": "value4",
    "key5": "value5"
';
json_validate_trycatchdump($json);

?>
--EXPECT--
Testing error locations in multi-line JSON

Error on line 2, column 13:
bool(false)
int(3)
string(72) "Control character error, possibly incorrectly encoded near location 2:13"

Error on line 3, column 12:
bool(false)
int(4)
string(31) "Syntax error near location 3:12"

Error on line 5, column 26:
bool(false)
int(3)
string(72) "Control character error, possibly incorrectly encoded near location 5:26"

Error on line 7, column 1:
bool(false)
int(4)
string(30) "Syntax error near location 7:1"

