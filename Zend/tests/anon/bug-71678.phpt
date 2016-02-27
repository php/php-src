--TEST--
Bug #71678 class() extends self/parent
--FILE--
<?php
class Base {
  
}

class T extends Base {
  static function newParent() {
    return new class() extends parent { };
  }

  static function newSelf() {
    return new class() extends self { };
  }
}

$parent= T::newParent();
var_dump($parent, get_parent_class(get_class($parent)));

$self= T::newSelf();
var_dump($self, get_parent_class(get_class($self)));
--EXPECTF--
object(class@%s)#%d (0) {
}
string(4) "Base"
object(class@%s)#%d (0) {
}
string(1) "T"
