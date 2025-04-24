--TEST--
GH-11108: Incorrect CG(memoize_mode) state after bailout in ??=
--FILE--
<?php
register_shutdown_function(function() {
    include __DIR__ . '/gh11108_shutdown.inc';
});
include __DIR__ . '/gh11108_test.inc';
?>
--EXPECTF--
Fatal error: Cannot use [] for reading in %s on line %d
