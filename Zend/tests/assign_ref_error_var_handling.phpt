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
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
var_dump(count($arr));
try {
    var_dump($arr[] =& val());
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
var_dump(count($arr));

?>
--EXPECT--
Error: Cannot add element to the array as the next element is already occupied
int(1)
Error: Cannot add element to the array as the next element is already occupied
int(1)
