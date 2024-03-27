--TEST--
Observer: Nested fibers
--EXTENSIONS--
zend_test
--INI--
zend_test.observer.enabled=1
zend_test.observer.fiber_switch=1
--FILE--
<?php

$fiber = new Fiber(function (): void {
    Fiber::suspend();
    var_dump(1);

    $fiber = new Fiber(function (): void {
        Fiber::suspend();
        var_dump(3);
        Fiber::suspend();
        var_dump(5);
    });

    $fiber->start();

    Fiber::suspend();
    var_dump(2);

    $fiber->resume();

    Fiber::suspend();
    var_dump(4);

    $fiber->resume();
});

$fiber->start();
$fiber->resume();
$fiber->resume();
$fiber->resume();

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
<!-- init Fiber::resume() -->
<!-- switching from fiber %s to %s -->
<resume '%s'>
<!-- init var_dump() -->
int(1)
<!-- switching from fiber %s to %s -->
<init '%s'>
<!-- init {closure:%s:%d}() -->
<!-- switching from fiber %s to %s -->
<suspend '%s'>
<!-- switching from fiber %s to %s -->
<suspend '%s'>
<!-- switching from fiber %s to %s -->
<resume '%s'>
int(2)
<!-- switching from fiber %s to %s -->
<resume '%s'>
int(3)
<!-- switching from fiber %s to %s -->
<suspend '%s'>
<!-- switching from fiber %s to %s -->
<suspend '%s'>
<!-- switching from fiber %s to %s -->
<resume '%s'>
int(4)
<!-- switching from fiber %s to %s -->
<resume '%s'>
int(5)
<!-- switching from fiber %s to %s -->
<returned '%s'>
<!-- switching from fiber %s to %s -->
<returned '%s'>
