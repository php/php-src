--TEST--
Observer: Nested fibers
--SKIPIF--
<?php if (!extension_loaded('zend_test')) die('skip: zend_test extension required'); ?>
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
<!-- init '%sobserver_fiber_03.php' -->
<!-- switching from fiber 0 to %s -->
<init '%s'>
<!-- init {closure}() -->
<!-- switching from fiber %s to 0 -->
<suspend '%s'>
<!-- switching from fiber 0 to %s -->
<resume '%s'>
int(1)
<!-- switching from fiber %s to %s -->
<init '%s'>
<!-- init {closure}() -->
<!-- switching from fiber %s to %s -->
<suspend '%s'>
<!-- switching from fiber %s to 0 -->
<suspend '%s'>
<!-- switching from fiber 0 to %s -->
<resume '%s'>
int(2)
<!-- switching from fiber %s to %s -->
<resume '%s'>
int(3)
<!-- switching from fiber %s to %s -->
<suspend '%s'>
<!-- switching from fiber %s to 0 -->
<suspend '%s'>
<!-- switching from fiber 0 to %s -->
<resume '%s'>
int(4)
<!-- switching from fiber %s to %s -->
<resume '%s'>
int(5)
<!-- switching from fiber %s to %s -->
<returned '%s'>
<!-- switching from fiber %s to 0 -->
<returned '%s'>
