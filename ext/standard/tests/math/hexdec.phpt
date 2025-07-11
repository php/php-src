--TEST--
Basic hexdec() test
--INI--
precision=14
--FILE--
<?php

$values = [
    0xABCD => 'ABCD',
    0x1777 => '1777',
    0x7FFFFFFF => '7FFFFFFF',
    0x80000000 => '80000000',
];

$values_leading = [
    0xABCD => '0ABCD',
    0x1777 => '01777',
    0x7FFFFFFF => '07FFFFFFF',
    0x80000000 => '080000000',
];

$values_leading_explicit = [
    0xABCD => '0xABCD',
    0x1777 => '0x1777',
    0x7FFFFFFF => '0x7FFFFFFF',
    0x80000000 => '0x80000000',
];

foreach ($values as $nb => $value) {
    echo "hexdec('$value') should be $nb is ";
    $res = hexdec($value);
    var_dump($res);
}

foreach ($values_leading as $nb => $value) {
    echo "hexdec('$value') should be $nb is ";
    $res = hexdec($value);
    var_dump($res);
}

foreach ($values_leading_explicit as $nb => $value) {
    echo "hexdec('$value') should be $nb is ";
    $res = hexdec($value);
    var_dump($res);
}

?>
--EXPECT--
hexdec('ABCD') should be 43981 is int(43981)
hexdec('1777') should be 6007 is int(6007)
hexdec('7FFFFFFF') should be 2147483647 is int(2147483647)
hexdec('80000000') should be 2147483648 is int(2147483648)
hexdec('0ABCD') should be 43981 is int(43981)
hexdec('01777') should be 6007 is int(6007)
hexdec('07FFFFFFF') should be 2147483647 is int(2147483647)
hexdec('080000000') should be 2147483648 is int(2147483648)
hexdec('0xABCD') should be 43981 is int(43981)
hexdec('0x1777') should be 6007 is int(6007)
hexdec('0x7FFFFFFF') should be 2147483647 is int(2147483647)
hexdec('0x80000000') should be 2147483648 is int(2147483648)
