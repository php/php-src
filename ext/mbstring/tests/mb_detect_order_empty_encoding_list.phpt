--TEST--
Test mb_detect_order() function : empty encoding list
--EXTENSIONS--
mbstring
--FILE--
<?php

try {
    var_dump( mb_detect_order(''));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump( mb_detect_order([]));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
mb_detect_order(): Argument #1 ($encoding) must specify at least one encoding
mb_detect_order(): Argument #1 ($encoding) must specify at least one encoding
