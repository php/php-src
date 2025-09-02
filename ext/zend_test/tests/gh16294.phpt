--TEST--
GH-16294: Segfault in test observer on zend_pass_function
--EXTENSIONS--
zend_test
--INI--
zend_test.observer.execute_internal=1
--FILE--
<?php

class Foo {};
new Foo([]);

?>
===DONE===
--EXPECT--
===DONE===
