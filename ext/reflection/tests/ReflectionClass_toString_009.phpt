--TEST--
ReflectionClass::__toString() - readonly property with default
--FILE--
<?php

class Test {
    public readonly int $property = 42;
}

echo new ReflectionClass(Test::class);

?>
--EXPECTF--
Class [ <user> class Test ] {
  @@ %s 3-5

  - Constants [0] {
  }

  - Static properties [0] {
  }

  - Static methods [0] {
  }

  - Properties [1] {
    Property [ public protected(set) readonly int $property = 42 ]
  }

  - Methods [0] {
  }
}
