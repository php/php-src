--TEST--
Bug #79947: Memory leak on invalid offset type in compound assignment
--FILE--
<?php
$array = [];
$key = [];
try {
    $array[$key] += [$key];
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
var_dump($array);
?>
--EXPECT--
Illegal offset type
array(0) {
}
