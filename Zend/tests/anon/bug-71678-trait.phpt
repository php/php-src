--TEST--
Bug #71678 class() extends self/parent inside traits
--FILE--
<?php
trait Creation {
  static function newParent() {
    return new class() extends parent { };
  }

  static function newSelf() {
    return new class() extends self { };
  }
}

class Base {
  
}

class Test extends Base {
  use Creation;
}

$parent= Test::newParent();
var_dump($parent, get_parent_class(get_class($parent)));

$self= Test::newSelf();
var_dump($self, get_parent_class(get_class($self)));
--EXPECTF--
object(class@%s)#%d (0) {
}
string(4) "Base"
object(class@%s)#%d (0) {
}
string(4) "Test"
