--TEST--
Pretty printing for match expression
--FILE--
<?php

assert((function () {
    match ('foo') {
        'foo', 'bar' => false,
        'baz' => 'a',
        default => 'b',
    };
})());

?>
--EXPECTF--
Warning: assert(): assert(function () {
    match ('foo') {
        'foo', 'bar' => false,
        'baz' => 'a',
        default => 'b',
    };
}()) failed in %s on line %d
