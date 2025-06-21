--TEST--
bindec(): basic
--INI--
precision=14
--FILE--
<?php

$values = [
    0b101010 => '101010',
    0b111111 => '111111',
];

$values_leading = [
    0b101010 => '0101010',
    0b111111 => '0111111',
];

$values_leading_explicit = [
    0b101010 => '0b101010',
    0b111111 => '0b111111',
];

foreach ($values as $nb => $value) {
    echo "bindec('$value') should be $nb is ";
    $res = bindec($value);
    var_dump($res);
}

foreach ($values_leading as $nb => $value) {
    echo "bindec('$value') should be $nb is ";
    $res = bindec($value);
    var_dump($res);
}

foreach ($values_leading_explicit as $nb => $value) {
    echo "bindec('$value') should be $nb is ";
    $res = bindec($value);
    var_dump($res);
}
?>
--EXPECT--
bindec('101010') should be 42 is int(42)
bindec('111111') should be 63 is int(63)
bindec('0101010') should be 42 is int(42)
bindec('0111111') should be 63 is int(63)
bindec('0b101010') should be 42 is int(42)
bindec('0b111111') should be 63 is int(63)
