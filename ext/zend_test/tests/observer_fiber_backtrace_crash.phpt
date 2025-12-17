--TEST--
GH-16319 (Fiber backtrace with null filename should not crash)
--EXTENSIONS--
zend_test
--INI--
zend_test.observer.enabled=1
zend_test.observer.show_init_backtrace=1
zend_test.observer.show_output=1
zend_test.observer.observe_all=1
zend_test.observer.show_opcode=0
opcache.jit=0
--FILE--
<?php
$fiber = new Fiber(function() {});
$fiber->start();
echo "Test completed without crash\n";
?>
--EXPECTF--
<!-- init %s -->
<!--
    {main} %s
-->
<file %s>
  <!-- init Fiber::__construct() -->
  <!--
      Fiber::__construct()
      {main} %s
  -->
  <Fiber::__construct>
  </Fiber::__construct>
  <!-- init Fiber::start() -->
  <!--
      Fiber::start()
      {main} %s
  -->
  <Fiber::start>
    <!-- init {closure:%s:%d}() -->
    <!--
        {closure:%s:%d}()
        {main} [no active file]
        Fiber::start()
        {main} %s
    -->
    <{closure:%s:%d}>
    </{closure:%s:%d}>
  </Fiber::start>
Test completed without crash
</file %s>
