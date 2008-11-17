--TEST--
ReflectionObject::__toString() : very basic test with dynamic properties
--FILE--
<?php

class Foo  {
	public $bar = 1;
}
$f = new foo;
$f->dynProp = 'hello';
$f->dynProp2 = 'hello again';
echo new ReflectionObject($f);

?>
--EXPECTF--
Object of class [ <user> class Foo ] {
  @@ %s 3-5

  - Constants [0] {
  }

  - Static properties [0] {
  }

  - Static methods [0] {
  }

  - Properties [1] {
    Property [ <default> public $bar ]
  }

  - Dynamic properties [2] {
    Property [ <dynamic> public $dynProp ]
    Property [ <dynamic> public $dynProp2 ]
  }

  - Methods [0] {
  }
}