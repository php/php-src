--TEST--
ReflectionProperty::getHooks()
--FILE--
<?php

class Test {
    public $prop1;
    public $prop2 { get {} set {} }
    public $prop3 { get {} }
    public $prop4 { set {} }
}

for ($i = 1; $i <= 4; $i++) {
  $rp = new ReflectionProperty(Test::class, 'prop' . $i);
  var_dump($rp->hasHooks());
  var_dump($rp->getHooks());
}

?>
--EXPECT--
bool(false)
array(0) {
}
bool(true)
array(2) {
  ["get"]=>
  object(ReflectionMethod)#1 (2) {
    ["name"]=>
    string(11) "$prop2::get"
    ["class"]=>
    string(4) "Test"
  }
  ["set"]=>
  object(ReflectionMethod)#3 (2) {
    ["name"]=>
    string(11) "$prop2::set"
    ["class"]=>
    string(4) "Test"
  }
}
bool(true)
array(1) {
  ["get"]=>
  object(ReflectionMethod)#2 (2) {
    ["name"]=>
    string(11) "$prop3::get"
    ["class"]=>
    string(4) "Test"
  }
}
bool(true)
array(1) {
  ["set"]=>
  object(ReflectionMethod)#3 (2) {
    ["name"]=>
    string(11) "$prop4::set"
    ["class"]=>
    string(4) "Test"
  }
}
