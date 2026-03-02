--TEST--
Using ReflectionClass::__toString() with typed class constants
--FILE--
<?php

class Foo {
    const ?int CONST1 = 1;
}

echo new ReflectionClass(Foo::class);

?>
--EXPECTF--
Class [ <user> class Foo ] {
  @@ %s 3-5

  - Constants [1] {
    Constant [ public ?int CONST1 ] { 1 }
  }

  - Static properties [0] {
  }

  - Static methods [0] {
  }

  - Properties [0] {
  }

  - Methods [0] {
  }
}
