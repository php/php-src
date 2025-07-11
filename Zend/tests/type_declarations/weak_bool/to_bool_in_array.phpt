--TEST--
Check deprecation to bool type in weak mode for specialized in_array() opcode
--FILE--
<?php

$a = [
    5 => 5,
    6 => 6,
    'five' => '5',
];


echo "int:\n";
var_dump(in_array(5, $a, 2));
var_dump(in_array(5, $a, 0));
var_dump(in_array('5', $a, 2));
var_dump(in_array('5', $a, 0));
var_dump(in_array(6, $a, 2));
var_dump(in_array(6, $a, 0));

echo "float:\n";
var_dump(in_array(5, $a, 52.45));
var_dump(in_array(5, $a, -0.0));
var_dump(in_array('5', $a, 52.45));
var_dump(in_array('5', $a, -0.0));
var_dump(in_array(6, $a, 52.45));
var_dump(in_array(6, $a, -0.0));

echo "string:\n";
var_dump(in_array(5, $a, 'hello'));
var_dump(in_array(5, $a, ''));
var_dump(in_array('5', $a, 'hello'));
var_dump(in_array('5', $a, ''));
var_dump(in_array(6, $a, 'hello'));
var_dump(in_array(6, $a, ''));
?>
--EXPECTF--
int:

Deprecated: in_array(): Passing int to parameter #3 ($strict) of type bool is deprecated in %s on line %d
bool(true)

Deprecated: in_array(): Passing int to parameter #3 ($strict) of type bool is deprecated in %s on line %d
bool(true)

Deprecated: in_array(): Passing int to parameter #3 ($strict) of type bool is deprecated in %s on line %d
bool(true)

Deprecated: in_array(): Passing int to parameter #3 ($strict) of type bool is deprecated in %s on line %d
bool(true)

Deprecated: in_array(): Passing int to parameter #3 ($strict) of type bool is deprecated in %s on line %d
bool(true)

Deprecated: in_array(): Passing int to parameter #3 ($strict) of type bool is deprecated in %s on line %d
bool(true)
float:

Deprecated: in_array(): Passing float to parameter #3 ($strict) of type bool is deprecated in %s on line %d
bool(true)

Deprecated: in_array(): Passing float to parameter #3 ($strict) of type bool is deprecated in %s on line %d
bool(true)

Deprecated: in_array(): Passing float to parameter #3 ($strict) of type bool is deprecated in %s on line %d
bool(true)

Deprecated: in_array(): Passing float to parameter #3 ($strict) of type bool is deprecated in %s on line %d
bool(true)

Deprecated: in_array(): Passing float to parameter #3 ($strict) of type bool is deprecated in %s on line %d
bool(true)

Deprecated: in_array(): Passing float to parameter #3 ($strict) of type bool is deprecated in %s on line %d
bool(true)
string:

Deprecated: in_array(): Passing string to parameter #3 ($strict) of type bool is deprecated in %s on line %d
bool(true)

Deprecated: in_array(): Passing string to parameter #3 ($strict) of type bool is deprecated in %s on line %d
bool(true)

Deprecated: in_array(): Passing string to parameter #3 ($strict) of type bool is deprecated in %s on line %d
bool(true)

Deprecated: in_array(): Passing string to parameter #3 ($strict) of type bool is deprecated in %s on line %d
bool(true)

Deprecated: in_array(): Passing string to parameter #3 ($strict) of type bool is deprecated in %s on line %d
bool(true)

Deprecated: in_array(): Passing string to parameter #3 ($strict) of type bool is deprecated in %s on line %d
bool(true)
