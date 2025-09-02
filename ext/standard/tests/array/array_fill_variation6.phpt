--TEST--
array_fill(): last element
--FILE--
<?php
$a = array_fill(PHP_INT_MAX, 1, "foo");
var_dump(
    count($a),
    array_key_exists(PHP_INT_MAX, $a),
);
try {
    $a[] = "bar";
} catch (Error $ex) {
    echo $ex->getMessage(), PHP_EOL;
}
?>
--EXPECT--
int(1)
bool(true)
Cannot add element to the array as the next element is already occupied
