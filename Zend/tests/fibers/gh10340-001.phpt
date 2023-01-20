--TEST--
Bug GH-10340 001 (Assertion in zend_fiber_object_gc())
--FILE--
<?php
function f() {
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
--EXPECTF--
Warning: Undefined variable $y in %s on line %d
==DONE==
