--TEST--
Observer: Function observing in fibers with unfinished fiber
--EXTENSIONS--
zend_test
--INI--
zend_test.observer.enabled=1
zend_test.observer.observe_all=1
zend_test.observer.fiber_init=1
zend_test.observer.fiber_switch=1
zend_test.observer.fiber_destroy=1
--FILE--
<?php

$fiber = new Fiber(function (): void {
    var_dump(1);
    Fiber::suspend();
    var_dump(2);
});

$fiber->start();

?>
--EXPECTF--
<!-- init '%s' -->
<file '%s'>
  <!-- init Fiber::__construct() -->
  <Fiber::__construct>
  </Fiber::__construct>
  <!-- init Fiber::start() -->
  <Fiber::start>
<!-- alloc: %s -->
<!-- switching from fiber %s to %s -->
<init '%s'>
    <!-- init {closure}() -->
    <{closure}>
      <!-- init var_dump() -->
      <var_dump>
int(1)
      </var_dump>
      <!-- init Fiber::suspend() -->
      <Fiber::suspend>
<!-- switching from fiber %s to %s -->
<suspend '%s'>
      </Fiber::start>
    </file '%s'>
<!-- switching from fiber %s to %s -->
<destroying '%s'>
    <!-- Exception: GracefulExit -->
  </Fiber::suspend>
  <!-- Exception: GracefulExit -->
</{closure}>
<!-- switching from fiber %s to %s -->
<destroyed '%s'>
<!-- destroy: %s -->