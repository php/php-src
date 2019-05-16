--TEST--
Bug #29210 (Function is_callable does not support private and protected methods)
--FILE--
<?php
class test_class {
   private function test_func1() {
   	 echo "test_func1\n";
   }
   protected function test_func2() {
   	 echo "test_func2\n";
   }
   static private function test_func3() {
   	 echo "test_func3\n";
   }
   static protected function test_func4() {
   	 echo "test_func4\n";
   }
   function test() {
     if (is_callable(array($this,'test_func1'))) {
	     $this->test_func1();
     } else {
       echo "test_func1 isn't callable from inside\n";
     }
     if (is_callable(array($this,'test_func2'))) {
	     $this->test_func2();
     } else {
       echo "test_func2 isn't callable from inside\n";
     }
     if (is_callable(array('test_class','test_func3'))) {
	     test_class::test_func3();
     } else {
       echo "test_func3 isn't callable from inside\n";
     }
     if (is_callable(array('test_class','test_func4'))) {
	     test_class::test_func4();
     } else {
       echo "test_func4 isn't callable from inside\n";
     }
   }
}

class foo extends test_class {
   function test() {
     if (is_callable(array($this,'test_func1'))) {
	     $this->test_func1();
     } else {
       echo "test_func1 isn't callable from child\n";
     }
     if (is_callable(array($this,'test_func2'))) {
	     $this->test_func2();
     } else {
       echo "test_func2 isn't callable from child\n";
     }
     if (is_callable(array('test_class','test_func3'))) {
	     test_class::test_func3();
     } else {
       echo "test_func3 isn't callable from child\n";
     }
     if (is_callable(array('test_class','test_func4'))) {
	     test_class::test_func4();
     } else {
       echo "test_func4 isn't callable from child\n";
     }
   }
}

$object = new test_class;
$object->test();
if (is_callable(array($object,'test_func1'))) {
	$object->test_func1();
} else {
  echo "test_func1 isn't callable from outside\n";
}
if (is_callable(array($object,'test_func2'))) {
	$object->test_func2();
} else {
  echo "test_func2 isn't callable from outside\n";
}
if (is_callable(array('test_class','test_func3'))) {
  test_class::test_func3();
} else {
  echo "test_func3 isn't callable from outside\n";
}
if (is_callable(array('test_class','test_func4'))) {
  test_class::test_func4();
} else {
  echo "test_func4 isn't callable from outside\n";
}
$object = new foo();
$object->test();
?>
--EXPECT--
test_func1
test_func2
test_func3
test_func4
test_func1 isn't callable from outside
test_func2 isn't callable from outside
test_func3 isn't callable from outside
test_func4 isn't callable from outside
test_func1 isn't callable from child
test_func2
test_func3 isn't callable from child
test_func4
