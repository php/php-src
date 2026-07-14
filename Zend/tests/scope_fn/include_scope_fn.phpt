--TEST--
Scope function declared in an included file is bound to the include's frame
--FILE--
<?php
// 1. No escape: scope_fn lives only in the shared symbol table; those
//    refs are owned by the include's scope and don't count as escape.
include __DIR__ . '/include_no_escape.inc';
echo "include 1 returned\n";
var_dump(isset($fn));

// 2. Escape via include's return value (return slot is outside the
//    include's symbol table).
try {
    $fn = include __DIR__ . '/include_escape_via_return.inc';
    echo "MUST NOT REACH (return)\n";
} catch (Error $e) {
    echo "return: ", $e->getMessage(), "\n";
}

// 3. Escape via storage outside the include's symbol table (a class
//    static property survives the include's frame teardown).
try {
    include __DIR__ . '/include_escape_via_static.inc';
    echo "MUST NOT REACH (static)\n";
} catch (Error $e) {
    echo "static: ", $e->getMessage(), "\n";
}
?>
--EXPECT--
in include, fn() = 7
include 1 returned
bool(false)
return: Scope function closure must not outlive the declaring scope
static: Scope function closure must not outlive the declaring scope
