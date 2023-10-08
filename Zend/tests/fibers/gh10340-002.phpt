--TEST--
Bug GH-10340 002 (Assertion in zend_fiber_object_gc())
--FILE--
<?php
function f() {
    $y = 'a';
    $$y = Fiber::getCurrent();
    Fiber::suspend();
}
$fiber = new Fiber(function() {
    get_defined_vars();
    f();
});
$fiber->start();
gc_collect_cycles();
?>
==DONE==
--EXPECT--
==DONE==
