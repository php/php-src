--TEST--
GC of a fiber suspended in a file included from a function
--FILE--
<?php
function run() {
    include __DIR__ . '/gc-include-shared-symbol-table.inc';
}

$fiber = new Fiber(function () { run(); });
$fiber->start();

// The cycle reaches the fiber, so the collector walks its frames.
$holder = new stdClass;
$holder->fiber = $fiber;
$holder->self = $holder;
unset($holder);

var_dump(gc_collect_cycles());
$fiber->resume();
?>
--EXPECT--
int(1)
local kept
