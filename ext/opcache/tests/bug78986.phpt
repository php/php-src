--TEST--
Bug #78986: Opcache segfaults when inheriting ctor from immutable into mutable class
--EXTENSIONS--
opcache
--FILE--
<?php

define('TEST_TEST', 1);

class TestClass2 {
    function __construct() {}
}

class TestClass extends TestClass2 {
    var $test = [
        TEST_TEST => 'test'
    ];
}

var_dump(new TestClass());

?>
--EXPECT--
object(TestClass)#1 (1) {
  ["test"]=>
  array(1) {
    [1]=>
    string(4) "test"
  }
}
