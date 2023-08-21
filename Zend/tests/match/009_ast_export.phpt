--TEST--
Pretty printing for match expression
--INI--
zend.assertions=1
--FILE--
<?php

try {
assert((function () {
    match ('foo') {
        'foo', 'bar' => false,
        'baz' => 'a',
        default => 'b',
    };
})());
} catch (AssertionError $e) {
    echo 'assert(): ', $e->getMessage(), ' failed', PHP_EOL;
}

?>
--EXPECTF--
assert(): assert(function () {
    match ('foo') {
        'foo', 'bar' => false,
        'baz' => 'a',
        default => 'b',
    };
}()) failed
