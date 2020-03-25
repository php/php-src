--TEST--
Bug #77669: Crash in extract() when overwriting extracted array
--FILE--
<?php

function test($mode) {
    $foo = [];
    $foo["foo"] = 42;
    $foo["bar"] = 24;
    extract($foo, $mode, "");
    $prefix_foo = [];
    $prefix_foo["foo"] = 42;
    $prefix_foo["bar"] = 24;
    extract($prefix_foo, $mode, "prefix");
}

test(EXTR_OVERWRITE);
test(EXTR_SKIP);
test(EXTR_IF_EXISTS);
test(EXTR_PREFIX_SAME);
test(EXTR_PREFIX_ALL);
test(EXTR_PREFIX_INVALID);
test(EXTR_PREFIX_IF_EXISTS);
test(EXTR_REFS | EXTR_OVERWRITE);
test(EXTR_REFS | EXTR_SKIP);
test(EXTR_REFS | EXTR_IF_EXISTS);
test(EXTR_REFS | EXTR_PREFIX_SAME);
test(EXTR_REFS | EXTR_PREFIX_ALL);
test(EXTR_REFS | EXTR_PREFIX_INVALID);
test(EXTR_REFS | EXTR_PREFIX_IF_EXISTS);

?>
===DONE===
--EXPECT--
===DONE===
