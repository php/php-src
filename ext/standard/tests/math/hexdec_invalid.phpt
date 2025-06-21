--TEST--
hexdec(): with invalid chars
--INI--
precision=14
--FILE--
<?php

$values = [
    0xABCD => 'ABCDg',
    0x1777 => '1777?!+',
    -0xABCD => '-ABCD',
];

foreach ($values as $nb => $value) {
    echo "hexdec('$value') should be $nb is";
    $res = hexdec($value);
    var_dump($res);
}

?>
--EXPECTF--
hexdec('ABCDg') should be 43981 is
Deprecated: Invalid characters passed for attempted conversion, these have been ignored in %s on line %d
int(43981)
hexdec('1777?!+') should be 6007 is
Deprecated: Invalid characters passed for attempted conversion, these have been ignored in %s on line %d
int(6007)
hexdec('-ABCD') should be -43981 is
Deprecated: Invalid characters passed for attempted conversion, these have been ignored in %s on line %d
int(43981)
