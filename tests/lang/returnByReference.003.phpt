--TEST--
Returning a reference from a function
--FILE--
<?php
function returnConstantByValue() {
    return 100;
}

function &returnConstantByRef() {
    return 100;
}

function &returnVariableByRef() {
    return $GLOBALS['a'];
}

echo "\n---> 1. Trying to assign by reference the return value of a function that returns by value:\n";
unset($a, $b);
$a = 4;
$b = &returnConstantByValue();
$a++;
var_dump($a, $b);

echo "\n---> 2. Trying to assign by reference the return value of a function that returns a constant by ref:\n";
unset($a, $b);
$a = 4;
$b = &returnConstantByRef();
$a++;
var_dump($a, $b);

echo "\n---> 3. Trying to assign by reference the return value of a function that returns by ref:\n";
unset($a, $b);
$a = 4;
$b = &returnVariableByRef();
$a++;
var_dump($a, $b);

?>
--EXPECTF--
---> 1. Trying to assign by reference the return value of a function that returns by value:

Notice: Only variables should be assigned by reference in %s on line 17
int(5)
int(100)

---> 2. Trying to assign by reference the return value of a function that returns a constant by ref:

Notice: Only variable references should be returned by reference in %s on line 7
int(5)
int(100)

---> 3. Trying to assign by reference the return value of a function that returns by ref:
int(5)
int(5)
