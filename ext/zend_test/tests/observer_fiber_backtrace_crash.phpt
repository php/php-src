--TEST--
GH-16319 (Fiber backtrace with null filename should not crash)
--EXTENSIONS--
zend_test
--INI--
zend_test.observer.enabled=1
zend_test.observer.show_init_backtrace=1
--FILE--
<?php
$fiber = new Fiber(function() {});
$fiber->start();
echo "Test completed without crash\n";
?>
--EXPECTF--
<!-- init '%s' -->
<!--
    {main} %s
-->
<!-- init Fiber::__construct() -->
<!--
    Fiber::__construct()
    {main} %s
-->
<!-- init Fiber::start() -->
<!--
    Fiber::start()
    {main} %s
-->
<!-- init {closure}() -->
<!--
    {closure}()
    {main} [no active file]
    Fiber::start()
    {main} %s
-->
Test completed without crash
