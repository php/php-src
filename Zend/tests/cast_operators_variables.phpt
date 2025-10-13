--TEST--
Cast operators with variables
--FILE--
<?php

$nullVar = null;
$intVar = 42;
$stringVar = "123";
$invalidStringVar = "abc";

var_dump((?int) $nullVar);
var_dump((?int) $intVar);
var_dump((?int) $stringVar);

try {
    var_dump((!int) $nullVar);
} catch (TypeError $e) {
    echo "Caught TypeError for null\n";
}

var_dump((!int) $intVar);
var_dump((!int) $stringVar);

try {
    var_dump((!int) $invalidStringVar);
} catch (TypeError $e) {
    echo "Caught TypeError for invalid string\n";
}

?>
--EXPECT--
NULL
int(42)
int(123)
Caught TypeError for null
int(42)
int(123)
Caught TypeError for invalid string
