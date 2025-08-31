--TEST--
Observer: Unfinished fiber
--EXTENSIONS--
zend_test
--INI--
zend_test.observer.enabled=1
zend_test.observer.show_output=1
zend_test.observer.fiber_switch=1
--FILE--
<?php

$fiber = new Fiber(function (): void {
    Fiber::suspend();
});

$fiber->start();

?>
--EXPECTF--
<!-- init '%s' -->
<!-- init Fiber::__construct() -->
<!-- init Fiber::start() -->
<!-- switching from fiber %s to %s -->
<init '%s'>
<!-- init {closure:%s:%d}() -->
<!-- init Fiber::suspend() -->
<!-- switching from fiber %s to %s -->
<suspend '%s'>
<!-- switching from fiber %s to %s -->
<destroying '%s'>
<!-- switching from fiber %s to %s -->
<destroyed '%s'>
