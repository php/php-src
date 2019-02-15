--TEST--
ReflectionObject::export() : very basic test with no dynamic properties
--FILE--
<?php

class Foo  {
	public $bar = 1;
}
$f = new foo;

ReflectionObject::export($f);

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

  - Dynamic properties [0] {
  }

  - Methods [0] {
  }
}
