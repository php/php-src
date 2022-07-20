--TEST--
Observer: Basic fiber switching
--EXTENSIONS--
zend_test
--INI--
zend_test.observer.enabled=1
zend_test.observer.fiber_init=1
zend_test.observer.fiber_switch=1
zend_test.observer.fiber_destroy=1
--FILE--
<?php

$fiber = new Fiber(function (): void {
    Fiber::suspend();
});

$fiber->start();
$fiber->resume();

?>
--EXPECTF--
<!-- init '%s' -->
<!-- init Fiber::__construct() -->
<!-- init Fiber::start() -->
<!-- alloc: %s -->
<!-- switching from fiber %s to %s -->
<init '%s'>
<!-- init {closure}() -->
<!-- init Fiber::suspend() -->
<!-- switching from fiber %s to %s -->
<suspend '%s'>
<!-- init Fiber::resume() -->
<!-- switching from fiber %s to %s -->
<resume '%s'>
<!-- switching from fiber %s to %s -->
<returned '%s'>
<!-- destroy: %s -->
