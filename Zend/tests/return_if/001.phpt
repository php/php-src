--TEST--
return if in function
--FILE--
<?php

function divide($dividend, $divisor = null) {
    return if ($divisor === null); // return null void
    
    return if ($divisor == 0): 0; // explicit return value
    
    return $dividend / $divisor;
}

var_dump(divide(10));
var_dump(divide(10, 0));
var_dump(divide(10, 5));

?>
--EXPECT--
NULL
int(0)
int(2)
