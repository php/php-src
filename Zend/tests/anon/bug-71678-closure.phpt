--TEST--
Bug #71678 class() extends self/parent inside closures
--FILE--
<?php
class Base {
  
}

class Test extends Base {
  static function newParent() {
    return function() { return new class() extends parent { }; };
  }

  static function newSelf() {
    return function() { return new class() extends self { }; };
  }
}

$parent= Test::newParent()();
var_dump($parent, get_parent_class(get_class($parent)));

$self= Test::newSelf()();
var_dump($self, get_parent_class(get_class($self)));
--EXPECTF--
object(class@%s)#%d (0) {
}
string(4) "Base"
object(class@%s)#%d (0) {
}
string(4) "Test"