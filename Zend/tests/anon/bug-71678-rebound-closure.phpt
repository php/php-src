--TEST--
Bug #71678 class() extends self/parent/static inside closures
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

  static function newStatic() {
    return function() { return new class() extends static { }; };
  }
}

class Extension extends Test {

}

$ext= new Extension();

$parent= Test::newParent()->bindTo($ext, $ext)();
var_dump($parent, get_parent_class(get_class($parent)));

$self= Test::newSelf()->bindTo($ext, $ext)();
var_dump($self, get_parent_class(get_class($self)));

$self= Test::newStatic()->bindTo($ext, $ext)();
var_dump($self, get_parent_class(get_class($self)));
--EXPECTF--
object(class@%s)#%d (0) {
}
string(4) "Test"
object(class@%s)#%d (0) {
}
string(9) "Extension"
object(class@%s)#%d (0) {
}
string(9) "Extension"