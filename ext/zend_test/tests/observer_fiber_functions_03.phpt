--TEST--
Observer: Function observing in fibers with bailout in fiber
--EXTENSIONS--
zend_test
--INI--
zend_test.observer.enabled=1
zend_test.observer.show_output=1
zend_test.observer.observe_all=1
zend_test.observer.fiber_init=1
zend_test.observer.fiber_switch=1
zend_test.observer.fiber_destroy=1
memory_limit=100M
--FILE--
<?php

$notBailedOutFiber = new Fiber(function (): void {
    var_dump(1);
    Fiber::suspend();
});

$notBailedOutFiber->start();

$fiber = new Fiber(function (): void {
    var_dump(2);
    Fiber::suspend();
    str_repeat('A', 200_000_000);
});

$fiber->start();
$fiber->resume();

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
      <Fiber::__construct>
      </Fiber::__construct>
      <Fiber::start>
<!-- alloc: %s -->
<!-- switching from fiber %s to %s -->
<init '%s'>
        <!-- init {closure}() -->
        <{closure}>
          <var_dump>
int(2)
          </var_dump>
          <Fiber::suspend>
<!-- switching from fiber %s to %s -->
<suspend '%s'>
          </Fiber::start>
          <!-- init Fiber::resume() -->
          <Fiber::resume>
<!-- switching from fiber %s to %s -->
<resume '%s'>
          </Fiber::suspend>
          <!-- init str_repeat() -->
          <str_repeat>

Fatal error: Allowed memory size of 104857600 bytes exhausted %s on line %d
          </str_repeat>
        </{closure}>
<!-- switching from fiber %s to %s -->
<returned '%s'>
<!-- destroy: %s -->
      </Fiber::resume>
    </file '%s'>