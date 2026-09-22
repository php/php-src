--TEST--
Test mb_convert_variables() function : empty encoding list
--EXTENSIONS--
mbstring
--FILE--
<?php

$string = 'Hello';

try {
    var_dump( mb_convert_variables('UTF-8', '', $string));
} catch (\ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump( mb_convert_variables('UTF-8', [], $string));
} catch (\ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
ValueError: mb_convert_variables(): Argument #2 ($from_encoding) must specify at least one encoding
ValueError: mb_convert_variables(): Argument #2 ($from_encoding) must specify at least one encoding
