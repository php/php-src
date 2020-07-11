--TEST--
If the LHS of ref-assign ERRORs, that takes precedence over the "only variables" notice
--FILE--
<?php

function val() {
    return 42;
}

$var = 24;
$arr = [PHP_INT_MAX => "foo"];
try {
    var_dump($arr[] =& $var);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
var_dump(count($arr));
try {
    var_dump($arr[] =& val());
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
var_dump(count($arr));

?>
--EXPECT--
Cannot add element to the array as the next element is already occupied
int(1)
Cannot add element to the array as the next element is already occupied
int(1)
