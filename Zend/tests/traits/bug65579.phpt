--TEST--
Bug #65579 (Using traits with get_class_methods causes segfault)
--FILE--
<?php
trait ParentTrait {
    public function testMethod() { }
}

trait ChildTrait {
    use ParentTrait {
        testMethod as testMethodFromParentTrait;
    }
    public function testMethod() { }
}

class TestClass {
    use ChildTrait;
}

$obj = new TestClass();
var_dump(get_class_methods($obj));
?>
--EXPECT--
array(2) {
  [0]=>
  string(10) "testMethod"
  [1]=>
  string(25) "testMethodFromParentTrait"
}
