--TEST--
Bug #79947: Memory leak on invalid offset type in compound assignment
--FILE--
<?php
$array = [];
$key = [];
try {
    $array[$key] += [$key];
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
var_dump($array);
?>
--EXPECT--
TypeError: Cannot access offset of type array on array
array(0) {
}
