--TEST--
Bug #69579 (Internal trait double-free)
--EXTENSIONS--
zend_test
--FILE--
<?php

class Bar{
  use _ZendTestTrait;
}

$bar = new Bar();
var_dump($bar->testMethod());
// destruction causes a double-free and explodes

?>
--EXPECT--
bool(true)
