--TEST--
Partial application ast export
--INI--
assert.exception=1
--FILE--
<?php
try {
    assert(0 && foo(?) && foo(...));
} catch (Error $ex) {
    printf("%s\n", $ex->getMessage());
}
?>
--EXPECT--
assert(0 && foo(?) && foo(...))
