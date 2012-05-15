--TEST--
ReflectionClass::getMethods() with trait
--FILE--
<?php

class Class1 {
	use T {
		func as newFunc;
	}

	public function func() {
		echo "From Class1::func\n";
	}
}

class Class2 {
	use T {
		func as newFunc2;
	}
}

class Class3 {
	use T;
}

trait T {
	public function func() {
		echo "From trait T\n";
	}
}

echo "Class1:\n";
$rc1 = new ReflectionClass("Class1");
var_dump($rc1->getMethods());

echo "Class2:\n";
$rc2 = new ReflectionClass("Class2");
var_dump($rc2->getMethods());

echo "Class3:\n";
$rc3 = new ReflectionClass("Class3");
var_dump($rc3->getMethods());
--EXPECTF--
Class1:
array(2) {
  [0]=>
  &object(ReflectionMethod)#%d (2) {
    ["name"]=>
    string(4) "func"
    ["class"]=>
    string(6) "Class1"
  }
  [1]=>
  &object(ReflectionMethod)#%d (2) {
    ["name"]=>
    string(7) "newFunc"
    ["class"]=>
    string(6) "Class1"
  }
}
Class2:
array(2) {
  [0]=>
  &object(ReflectionMethod)#%d (2) {
    ["name"]=>
    string(8) "newFunc2"
    ["class"]=>
    string(6) "Class2"
  }
  [1]=>
  &object(ReflectionMethod)#%d (2) {
    ["name"]=>
    string(4) "func"
    ["class"]=>
    string(6) "Class2"
  }
}
Class3:
array(1) {
  [0]=>
  &object(ReflectionMethod)#%d (2) {
    ["name"]=>
    string(4) "func"
    ["class"]=>
    string(6) "Class3"
  }
}
