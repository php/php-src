--TEST--
ReflectionClass::getMethods()
--CREDITS--
Robin Fernandes <robinf@php.net>
Steve Seear <stevseea@php.net>
--FILE--
<?php
class C {
	public function pubf1() {}
	public function pubf2() {}
	private function privf1() {}
	private function privf2() {}
	static public function pubsf1() {}
	static public function pubsf2() {}
	static private function privsf1() {}
	static private function privsf2() {}
}

$rc = new ReflectionClass("C");
$StaticFlag = 0x01;
$pubFlag =  0x100;
$privFlag = 0x400;

echo "No methods:";
var_dump($rc->getMethods(0));

echo "Public methods:";
var_dump($rc->getMethods($pubFlag));

echo "Private methods:";
var_dump($rc->getMethods($privFlag));

echo "Public or static methods:";
var_dump($rc->getMethods($StaticFlag | $pubFlag));

echo "Private or static methods:";
var_dump($rc->getMethods($StaticFlag | $privFlag));


?>
--EXPECTF--
No methods:array(0) {
}
Public methods:array(4) {
  [0]=>
  object(ReflectionMethod)#%d (2) {
    ["name"]=>
    string(5) "pubf1"
    ["class"]=>
    string(1) "C"
  }
  [1]=>
  object(ReflectionMethod)#%d (2) {
    ["name"]=>
    string(5) "pubf2"
    ["class"]=>
    string(1) "C"
  }
  [2]=>
  object(ReflectionMethod)#%d (2) {
    ["name"]=>
    string(6) "pubsf1"
    ["class"]=>
    string(1) "C"
  }
  [3]=>
  object(ReflectionMethod)#%d (2) {
    ["name"]=>
    string(6) "pubsf2"
    ["class"]=>
    string(1) "C"
  }
}
Private methods:array(4) {
  [0]=>
  object(ReflectionMethod)#%d (2) {
    ["name"]=>
    string(6) "privf1"
    ["class"]=>
    string(1) "C"
  }
  [1]=>
  object(ReflectionMethod)#%d (2) {
    ["name"]=>
    string(6) "privf2"
    ["class"]=>
    string(1) "C"
  }
  [2]=>
  object(ReflectionMethod)#%d (2) {
    ["name"]=>
    string(7) "privsf1"
    ["class"]=>
    string(1) "C"
  }
  [3]=>
  object(ReflectionMethod)#%d (2) {
    ["name"]=>
    string(7) "privsf2"
    ["class"]=>
    string(1) "C"
  }
}
Public or static methods:array(6) {
  [0]=>
  object(ReflectionMethod)#%d (2) {
    ["name"]=>
    string(5) "pubf1"
    ["class"]=>
    string(1) "C"
  }
  [1]=>
  object(ReflectionMethod)#%d (2) {
    ["name"]=>
    string(5) "pubf2"
    ["class"]=>
    string(1) "C"
  }
  [2]=>
  object(ReflectionMethod)#%d (2) {
    ["name"]=>
    string(6) "pubsf1"
    ["class"]=>
    string(1) "C"
  }
  [3]=>
  object(ReflectionMethod)#%d (2) {
    ["name"]=>
    string(6) "pubsf2"
    ["class"]=>
    string(1) "C"
  }
  [4]=>
  object(ReflectionMethod)#%d (2) {
    ["name"]=>
    string(7) "privsf1"
    ["class"]=>
    string(1) "C"
  }
  [5]=>
  object(ReflectionMethod)#%d (2) {
    ["name"]=>
    string(7) "privsf2"
    ["class"]=>
    string(1) "C"
  }
}
Private or static methods:array(6) {
  [0]=>
  object(ReflectionMethod)#%d (2) {
    ["name"]=>
    string(6) "privf1"
    ["class"]=>
    string(1) "C"
  }
  [1]=>
  object(ReflectionMethod)#%d (2) {
    ["name"]=>
    string(6) "privf2"
    ["class"]=>
    string(1) "C"
  }
  [2]=>
  object(ReflectionMethod)#%d (2) {
    ["name"]=>
    string(6) "pubsf1"
    ["class"]=>
    string(1) "C"
  }
  [3]=>
  object(ReflectionMethod)#%d (2) {
    ["name"]=>
    string(6) "pubsf2"
    ["class"]=>
    string(1) "C"
  }
  [4]=>
  object(ReflectionMethod)#%d (2) {
    ["name"]=>
    string(7) "privsf1"
    ["class"]=>
    string(1) "C"
  }
  [5]=>
  object(ReflectionMethod)#%d (2) {
    ["name"]=>
    string(7) "privsf2"
    ["class"]=>
    string(1) "C"
  }
}
