--TEST--
Test is_nan() - basic function test is_nan()
--FILE--
<?php
$values = [
    234,
    -234,
    23.45e1,
    -23.45e1,
    pow(0, -2),
    acos(1.01),
];
foreach ($values as $value) {
    $res = is_nan($value);
    var_dump($res);
}

?>
--EXPECTF--
Deprecated: Power of base 0 and negative exponent is deprecated in %s on line %d
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(true)
