--TEST--
json_validate() - compare against json_decode() for different types of inputs 
--FILE--
<?php

$inputs = [
    '""',
    '"string"',
    '1234',
    '123.45',
    '-123',
    'null',
    'true',
    'false',
    '{}',
    '[]',
    '-',
    '',
];

foreach ($inputs as $input) {
    var_dump($input, json_decode($input), json_validate($input));
}

?>
--EXPECTF--
string(2) """"
string(0) ""
bool(true)
string(8) ""string""
string(6) "string"
bool(true)
string(4) "1234"
int(1234)
bool(true)
string(6) "123.45"
float(123.45)
bool(true)
string(4) "-123"
int(-123)
bool(true)
string(4) "null"
NULL
bool(true)
string(4) "true"
bool(true)
bool(true)
string(5) "false"
bool(false)
bool(true)
string(2) "{}"
object(stdClass)#1 (0) {
}
bool(true)
string(2) "[]"
array(0) {
}
bool(true)
string(1) "-"
NULL
bool(false)
string(0) ""
NULL
bool(false)
