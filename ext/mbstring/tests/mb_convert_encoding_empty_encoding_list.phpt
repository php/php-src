--TEST--
Test mb_convert_encoding() function : empty encoding list
--EXTENSIONS--
mbstring
--FILE--
<?php

$string = 'Hello';

try {
    var_dump( mb_convert_encoding($string, 'UTF-8', ''));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump( mb_convert_encoding($string, 'UTF-8', []));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
mb_convert_encoding(): Argument #3 ($from_encoding) must specify at least one encoding
mb_convert_encoding(): Argument #3 ($from_encoding) must specify at least one encoding
