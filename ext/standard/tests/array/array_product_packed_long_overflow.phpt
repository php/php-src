--TEST--
array_product() packed long overflow continues in double mode
--FILE--
<?php

$tests = [
    [[PHP_INT_MAX, 2, 3], ((float) PHP_INT_MAX * 2) * 3],
    [[PHP_INT_MIN, -1, 2], ((float) PHP_INT_MIN * -1) * 2],
];

foreach ($tests as [$input, $expected]) {
    $result = array_product($input);
    var_dump(is_float($result));
    var_dump($result === $expected);
}

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
