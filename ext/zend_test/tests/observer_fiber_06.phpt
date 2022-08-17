--TEST--
Observer: Throwing fiber
--EXTENSIONS--
zend_test
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
<!-- init '%s' -->
<!-- init Fiber::__construct() -->
<!-- init Fiber::start() -->
<!-- switching from fiber %s to %s -->
<init '%s'>
<!-- init {closure}() -->
<!-- init Fiber::suspend() -->
<!-- switching from fiber %s to %s -->
<suspend '%s'>
<!-- init Exception::__construct() -->
<!-- init Fiber::throw() -->
<!-- switching from fiber %s to %s -->
<resume '%s'>
<!-- switching from fiber %s to %s -->
<threw '%s'>
