--TEST--
If the LHS of ref-assign ERRORs, that takes precedence over the "only variables" notice
--FILE--
<?php

function val() {
    return 42;
}

$var = 24;
$arr = [PHP_INT_MAX => "foo"];
var_dump($arr[] =& $var);
var_dump(count($arr));
var_dump($arr[] =& val());
var_dump(count($arr));

?>
--EXPECTF--
Warning: Cannot add element to the array as the next element is already occupied in %s on line %d
NULL
int(1)

Warning: Cannot add element to the array as the next element is already occupied in %s on line %d
NULL
int(1)
