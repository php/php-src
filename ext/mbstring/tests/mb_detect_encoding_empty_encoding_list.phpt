--TEST--
Test mb_detect_encoding() function : empty encoding list
--EXTENSIONS--
mbstring
--FILE--
<?php

$string = 'Hello';

try {
    var_dump( mb_detect_encoding($string, ''));
} catch (\ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump( mb_detect_encoding($string, []));
} catch (\ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
ValueError: mb_detect_encoding(): Argument #2 ($encodings) must specify at least one encoding
ValueError: mb_detect_encoding(): Argument #2 ($encodings) must specify at least one encoding
