--TEST--
ReflectionClass::hasMethod()
--CREDITS--
Marc Veldman <marc@ibuildings.nl>
#testfest roosendaal on 2008-05-10
--FILE--
<?php
//New instance of class C - defined below
$rc = new ReflectionClass("C");

//Check if C has public method publicFoo
var_dump($rc->hasMethod('publicFoo'));

//Check if C has protected method protectedFoo
var_dump($rc->hasMethod('protectedFoo'));

//Check if C has private method privateFoo
var_dump($rc->hasMethod('privateFoo'));

//Check if C has static method staticFoo
var_dump($rc->hasMethod('staticFoo'));

//C should not have method bar
var_dump($rc->hasMethod('bar'));

//Method names are case insensitive
var_dump($rc->hasMethod('PUBLICfOO'));

Class C {
  public function publicFoo()
  {
    return true;
  }

  protected function protectedFoo()
  {
    return true;
  }

  private function privateFoo()
  {
    return true;
  }

  static function staticFoo()
  {
    return true;
  }
}
?>
--EXPECTF--
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
bool(true)
