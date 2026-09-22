--TEST--
PFA AST export
--INI--
assert.exception=1
--FILE--
<?php
try {
    assert(0 && foo(?) && foo(new stdClass, bar: 1, ...));
} catch (Error $ex) {
    printf("%s: %s\n", $ex::class, $ex->getMessage());
}
?>
--EXPECT--
AssertionError: assert(0 && foo(?) && foo(new stdClass(), bar: 1, ...))
