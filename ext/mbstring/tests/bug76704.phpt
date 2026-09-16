--TEST--
Bug #76704 (mb_detect_order return value varies based on argument type)
--EXTENSIONS--
mbstring
--FILE--
<?php
try {
    var_dump(mb_detect_order('Foo, UTF-8'));
} catch (\ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump(mb_detect_order(['Foo', 'UTF-8']));
} catch (\ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
ValueError: mb_detect_order(): Argument #1 ($encoding) contains invalid encoding "Foo"
ValueError: mb_detect_order(): Argument #1 ($encoding) contains invalid encoding "Foo"
