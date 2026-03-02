--TEST--
Acceptance of whitespace in numeric strings
--FILE--
<?php

$strings = [
    "123",
    "123   ",
    "123 \t\n\r\v\f",
    "   123",
    " \t\n\r\v\f123",
    "   123   ",
    " \t\n\r\v\f123 \t\n\r\v\f",
    "123.0",
    "123.0   ",
    "123.0 \t\n\r\v\f",
    "   123.0",
    " \t\n\r\v\f123.0",
    "   123.0   ",
    " \t\n\r\v\f123 \t\n\r\v\f",
    "123e0",
    "123e0   ",
    "123e0 \t\n\r\v\f",
    "   123e0",
    " \t\n\r\v\f123e0",
    "   123e0   ",
    " \t\n\r\v\f123e0 \t\n\r\v\f"
];

function takes_integer(int $i) {
    \assert($i === 123);
}
function takes_float(float $f) {
    \assert($f === 123.0);
}

foreach ($strings as $string) {
    \assert($string == 123);
    $num = +$string;
    \assert($num == 123);
    takes_integer($string);
    takes_float($string);
    \assert(\intdiv($string, 1) === 123);
    \assert(\is_numeric($string));
    $incremented = $string;
    ++$incremented;
    \assert(\is_int($incremented) || \is_float($incremented));
    \assert($incremented == 124);
    $decremented = $string;
    --$decremented;
    \assert(\is_int($decremented) || \is_float($decremented));
    \assert($decremented == 122);
}

echo "OK!", PHP_EOL;

?>
--EXPECT--
OK!
