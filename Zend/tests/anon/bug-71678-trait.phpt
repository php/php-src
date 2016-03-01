--TEST--
Bug #71678 class() extends self/parent/static inside traits
--FILE--
<?php
trait Creation {
  static function newParent() {
    return new class() extends parent { };
  }

  static function newSelf() {
    return new class() extends self { };
  }

  static function newStatic() {
    return new class() extends static { };
  }
}

class Base {
  
}

class Test extends Base {
  use Creation;
}

class Extension extends Test {

}

$parent= Test::newParent();
var_dump($parent, get_parent_class(get_class($parent)));

$self= Test::newSelf();
var_dump($self, get_parent_class(get_class($self)));

$self= Extension::newSelf();
var_dump($self, get_parent_class(get_class($self)));

$ext= Extension::newStatic();
var_dump($ext, get_parent_class(get_class($ext)));
--EXPECTF--
object(class@%s)#%d (0) {
}
string(4) "Base"
object(class@%s)#%d (0) {
}
string(4) "Test"
object(class@%s)#%d (0) {
}
string(4) "Test"
object(class@%s)#%d (0) {
}
string(9) "Extension"
