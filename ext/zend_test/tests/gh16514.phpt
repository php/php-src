--TEST--
GH-16514: Nested generator in zend_test observer
--EXTENSIONS--
zend_test
--INI--
zend_test.observer.enabled=1
zend_test.observer.show_output=1
zend_test.observer.show_init_backtrace=1
--FILE--
<?php

class Foo {
    public function __destruct() {
        debug_print_backtrace();
    }
}
function bar() {
    yield from foo();
}
function foo() {
    $foo = new Foo();
    yield;
}
$gen = bar();
foreach ($gen as $dummy);

?>
--EXPECTF--
<!-- init '%sgh16514.php' -->
<!--
    {main} %sgh16514.php
-->
<!-- init bar() -->
<!--
    bar()
    {main} %sgh16514.php
-->
<!-- init foo() -->
<!--
    foo()
    bar()
    {main} %sgh16514.php
-->
<!-- init Foo::__destruct() -->
<!--
    Foo::__destruct()
    bar()
    {main} %sgh16514.php
-->
<!-- init debug_print_backtrace() -->
<!--
    debug_print_backtrace()
    Foo::__destruct()
    bar()
    {main} %sgh16514.php
-->
#0 %s(%d): Foo->__destruct()
#1 %s(%d): bar()
