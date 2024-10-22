--TEST--
ReflectionConstant::getType()
--FILE--
<?php

const C_OBJ = new stdClass;
const C_NULL = null;
const C_BOOL = true;
const C_STRING = "string";
const C_ARRAY = ["a", "b", "c"];
const C_INT = 1234;
const C_FLOAT = 0.1234;
const C_OBJ2 = C_OBJ;

$list = [
    "C_OBJ",
    "C_NULL",
    "C_BOOL",
    "C_STRING",
    "C_ARRAY",
    "C_INT",
    "C_FLOAT",
    "C_OBJ2",
];

foreach ($list as $name) {
    $rc = new ReflectionConstant($name);
    echo $rc->getType(), "\n";
}

?>
--EXPECT--
stdClass
null
true
string
array
int
float
stdClass
