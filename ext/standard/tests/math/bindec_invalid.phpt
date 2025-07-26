--TEST--
bindec(): basic
--INI--
precision=14
--FILE--
<?php

$values = [
    -0b101010 => '-0b101010',
    0b111111 => '1111112',
    0b111111 => '111111?x',
];

foreach ($values as $nb => $value) {
    echo "bindec('$value') should be $nb is";
    $res = bindec($value);
    var_dump($res);
}

?>
--EXPECTF--
bindec('-0b101010') should be -42 is
Deprecated: Invalid characters passed for attempted conversion, these have been ignored in %s on line %d
int(42)
bindec('111111?x') should be 63 is
Deprecated: Invalid characters passed for attempted conversion, these have been ignored in %s on line %d
int(63)
