--TEST--
Pretty printing for switch expression
--FILE--
<?php

assert('foo' switch { default => false });

assert('foo' switch {
    'foo', 'bar' => false,
    'baz' => false,
});

assert((function () {
    switch ('foo') {
        case 'foo', 'bar':
            return false;
        case 'baz':
            return false;
    }
})());

?>
--EXPECTF--
Warning: assert(): assert('foo' switch {
    default => false,
}) failed in %s on line %d

Warning: assert(): assert('foo' switch {
    'foo', 'bar' => false,
    'baz' => false,
}) failed in %s on line %d

Warning: assert(): assert(function () {
    switch ('foo') {
        case 'foo', 'bar':
            return false;
        case 'baz':
            return false;
    }
}()) failed in %s on line %d
