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
} catch (Throwable $ex) {
    echo $ex::class, ': ', $ex->getMessage(), "\n";
}
?>
--EXPECT--
int(1)
bool(true)
Error: Cannot add element to the array as the next element is already occupied
