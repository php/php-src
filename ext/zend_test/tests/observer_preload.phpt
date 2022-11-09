--TEST--
Observer: Test with basic preloading
--EXTENSIONS--
zend_test
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/observer_preload.inc
zend_test.observer.enabled=1
zend_test.observer.observe_all=1
zend_test.observer.observe_declaring=1
zend_test.observer.show_return_value=1
--FILE--
<?php

Foo::test();
foo();

echo 'Done' . PHP_EOL;
?>
--EXPECTF--
<!-- declared class 'foo' -->
<!-- init '%sobserver_preload.inc' -->
<file '%sobserver_preload.inc'>
  <!-- declared function 'foo' -->
</file '%sobserver_preload.inc'>
<!-- init '%sobserver_preload.php' -->
<file '%sobserver_preload.php'>
  <!-- init Foo::test() -->
  <Foo::test>
  </Foo::test:'foo::test'>
  <!-- init foo() -->
  <foo>
  </foo:'I should be observable'>
Done
</file '%sobserver_preload.php'>