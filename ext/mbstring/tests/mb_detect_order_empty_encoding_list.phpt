--TEST--
Test mb_detect_order() function : empty encoding list
--EXTENSIONS--
mbstring
--FILE--
<?php

try {
    var_dump( mb_detect_order(''));
} catch (\ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump( mb_detect_order([]));
} catch (\ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
ValueError: mb_detect_order(): Argument #1 ($encoding) must specify at least one encoding
ValueError: mb_detect_order(): Argument #1 ($encoding) must specify at least one encoding
