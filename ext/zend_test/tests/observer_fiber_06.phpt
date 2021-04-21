--TEST--
Observer: Throwing fiber
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

try {
    $fiber->throw(new Exception);
} catch (Exception $exception) {

}

?>
--EXPECTF--
<!-- init '%sobserver_fiber_06.php' -->
<!-- switching from fiber 0 to %x -->
<init '%x'>
<!-- init {closure}() -->
<!-- switching from fiber %x to 0 -->
<suspend '%x'>
<!-- switching from fiber 0 to %x -->
<resume '%x'>
<!-- switching from fiber %x to 0 -->
<threw '%x'>
