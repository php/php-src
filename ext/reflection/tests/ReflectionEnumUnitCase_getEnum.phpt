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
Class [ <user> final class Foo implements UnitEnum ] {
  @@ %sReflectionEnumUnitCase_getEnum.php 3-5

  - Constants [1] {
    Constant [ public Foo Bar ] { Object }
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
    Property [ public readonly string $name ]
  }

  - Methods [0] {
  }
}
