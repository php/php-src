--TEST--
Observer: Basic fiber switching
--SKIPIF--
<?php if (!extension_loaded('zend_test')) die('skip: zend_test extension required'); ?>
--INI--
zend_test.observer.enabled=1
zend_test.observer.fiber_switch=1
--FILE--
<?php

$fiber = new Fiber(function (): void {
    Fiber::suspend();
});

$fiber->start();
$fiber->resume();

?>
--EXPECTF--
<!-- init '%sobserver_fiber_01.php' -->
<!-- switching from fiber 0 to %s -->
<init '%s'>
<!-- init {closure}() -->
<!-- switching from fiber %s to 0 -->
<suspend '%s'>
<!-- switching from fiber 0 to %s -->
<resume '%s'>
<!-- switching from fiber %s to 0 -->
<returned '%s'>
