--TEST--
Check max() optimisation for int and float types
--FILE--
<?php

echo "Start as int optimisation:\n";
var_dump(max([
    10, 5, 3, 2
]));
var_dump(max([
    2, 3, 5, 10
]));
var_dump(max([
    10, 5, 3.5, 2
]));
var_dump(max([
    2, 3.5, 5, 10
]));
var_dump(max([
    10, 5, "3", 2
]));
var_dump(max([
    2, "3", 5, 10
]));
var_dump(max([
    2, 3, "15", 10
]));

echo "Start as float optimisation:\n";
var_dump(max([
    10.5, 5.5, 3.5, 2.5
]));
var_dump(max([
    2.5, 3.5, 5.5, 10.5
]));
var_dump(max([
    10.5, 5.5, 3, 2.5
]));
var_dump(max([
    2.5, 3, 5.5, 10.5
]));
var_dump(max([
    10.5, 5.5, "3.5", 2.5
]));
var_dump(max([
    2.5, "3.5", 5.5, 10.5
]));
var_dump(max([
    2.5, 3.5, "15.5", 10.5
]));

?>
--EXPECT--
Start as int optimisation:
int(10)
int(10)
int(10)
int(10)
int(10)
int(10)
string(2) "15"
Start as float optimisation:
float(10.5)
float(10.5)
float(10.5)
float(10.5)
float(10.5)
float(10.5)
string(4) "15.5"
