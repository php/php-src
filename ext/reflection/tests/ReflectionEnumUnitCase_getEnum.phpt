--TEST--
ReflectionEnumUnitCase::getEnum()
--FILE--
<?php

enum Foo {
    case Bar;
}

echo (new ReflectionEnumUnitCase(Foo::class, 'Bar'))->getEnum();

?>
--EXPECTF--
Enum [ <user> enum Foo implements UnitEnum ] {
  @@ %sReflectionEnumUnitCase_getEnum.php 3-5

  - Enum cases [1] {
    Case Bar
  }

  - Constants [0] {
  }

  - Static properties [0] {
  }

  - Static methods [1] {
    Method [ <internal, prototype UnitEnum> static public method cases ] {

      - Parameters [0] {
      }
      - Return [ array ]
    }
  }

  - Properties [1] {
    Property [ public protected(set) readonly string $name ]
  }

  - Methods [0] {
  }
}
