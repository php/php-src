--TEST--
Observer: Nested fibers with unfinished fiber
--SKIPIF--
<?php if (!extension_loaded('zend_test')) die('skip: zend_test extension required'); ?>
--INI--
zend_test.observer.enabled=1
zend_test.observer.fiber_switch=1
--FILE--
<?php

$fiber = new Fiber(function (): void {
    Fiber::suspend();

    $fiber = new Fiber(function (): void {
        Fiber::suspend();
    });

    $fiber->start();

    Fiber::suspend();
});

$fiber->start();
$fiber->resume();
$fiber->resume();

?>
--EXPECTF--
<!-- init '%sobserver_fiber_04.php' -->
<!-- switching from fiber 0 to %x -->
<init '%x'>
<!-- init {closure}() -->
<!-- switching from fiber %x to 0 -->
<suspend '%x'>
<!-- switching from fiber 0 to %x -->
<resume '%x'>
<!-- switching from fiber %x to %x -->
<init '%x'>
<!-- init {closure}() -->
<!-- switching from fiber %x to %x -->
<suspend '%x'>
<!-- switching from fiber %x to 0 -->
<suspend '%x'>
<!-- switching from fiber 0 to %x -->
<resume '%x'>
<!-- switching from fiber %x to %x -->
<destroying '%x'>
<!-- switching from fiber %x to %x -->
<destroyed '%x'>
<!-- switching from fiber %x to 0 -->
<returned '%x'>
