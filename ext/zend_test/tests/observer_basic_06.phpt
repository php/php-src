--TEST--
Observer: Basic observability of opline only
--EXTENSIONS--
zend_test
--INI--
opcache.enable=0
zend_test.observer.enabled=1
zend_test.observer.observe_oplines=1
--FILE--
<?php
function foo(){}

foo();
?>
--EXPECTF--
<!-- init '%s%eobserver_basic_06.php' -->
lineno: %d
lineno: %d
<!-- init foo() -->
lineno: %d
lineno: %d
