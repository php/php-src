--TEST--
Test Csv\array_to_row() accepts internal objects convertible to string that are not Stringable (GMP)
--EXTENSIONS--
csv
gmp
--FILE--
<?php

$number = gmp_init('123456789012345678901234567890');
var_dump($number instanceof Stringable);
var_dump(Csv\array_to_row([$number, 'x'], eolSequence: "\n"));
var_dump(Csv\collection_to_buffer([[$number]], eolSequence: "\n"));

?>
--EXPECT--
bool(false)
string(33) "123456789012345678901234567890,x
"
string(31) "123456789012345678901234567890
"
