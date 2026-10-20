--TEST--
array_sum() packed long overflow continues in double mode
--FILE--
<?php

$tests = [
    [[PHP_INT_MAX, 1, 4096], ((float) PHP_INT_MAX + 1) + 4096],
    [[PHP_INT_MIN, -1, -4096], ((float) PHP_INT_MIN - 1) - 4096],
];

foreach ($tests as [$input, $expected]) {
    $result = array_sum($input);
    var_dump(is_float($result));
    var_dump($result === $expected);
}

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
