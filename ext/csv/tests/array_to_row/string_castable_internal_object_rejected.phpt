--TEST--
Test Csv\array_to_row() and Csv\collection_to_buffer() reject internal objects with a string cast handler (GMP)
--EXTENSIONS--
csv
gmp
--FILE--
<?php

$number = gmp_init('123456789012345678901234567890');
var_dump((string) $number);

try {
    Csv\array_to_row([$number, 'x']);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    Csv\collection_to_buffer([[$number]]);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
string(30) "123456789012345678901234567890"
Field 0 must be of type null|bool|int|float|string|BackedEnum, GMP given
Field 0 of element 1 of the collection must be of type null|bool|int|float|string|BackedEnum, GMP given
