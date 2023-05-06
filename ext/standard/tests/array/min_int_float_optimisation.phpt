--TEST--
Check min() optimisation for int and float types
--FILE--
<?php

echo "Start as int optimisation:\n";
var_dump(min([
    10, 5, 3, 2
]));
var_dump(min([
    2, 3, 5, 10
]));
var_dump(min([
    10, 5, 3.5, 2
]));
var_dump(min([
    2, 3.5, 5, 10
]));
var_dump(min([
    10, 5, "3", 2
]));
var_dump(min([
    2, "3", 5, 10
]));
var_dump(min([
    2, 3, "1", 10
]));

echo "Start as float optimisation:\n";
var_dump(min([
    10.5, 5.5, 3.5, 2.5
]));
var_dump(min([
    2.5, 3.5, 5.5, 10.5
]));
var_dump(min([
    10.5, 5.5, 3, 2.5
]));
var_dump(min([
    2.5, 3, 5.5, 10.5
]));
var_dump(min([
    10.5, 5.5, "3.5", 2.5
]));
var_dump(min([
    2.5, "3.5", 5.5, 10.5
]));
var_dump(min([
    2.5, 3.5, "1.5", 10.5
]));

?>
--EXPECT--
Start as int optimisation:
int(2)
int(2)
int(2)
int(2)
int(2)
int(2)
string(1) "1"
Start as float optimisation:
float(2.5)
float(2.5)
float(2.5)
float(2.5)
float(2.5)
float(2.5)
string(3) "1.5"
