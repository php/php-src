--TEST--
Using ReflectionClass::__toString() with friends
--FILE--
<?php

use Demo\Qux;

class Foo1 {
	friend Bar;
	friend Demo\Bar;
	friend Baz;
	friend Qux;
}

enum Foo2 {
	friend Bar;
	friend Demo\Bar;
	friend Baz;
	friend Qux;
}

echo new ReflectionClass( Foo1::class );
echo new ReflectionClass( Foo2::class );
?>
--EXPECTF--
Class [ <user> class Foo1 ] {
  @@ %s %d-%d

  - Constants [0] {
  }

  - Friends [4] {
     friend Bar
     friend Demo\Bar
     friend Baz
     friend Demo\Qux
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
Enum [ <user> enum Foo2 implements UnitEnum ] {
  @@ %s %d-%d

  - Constants [0] {
  }

  - Friends [4] {
     friend Bar
     friend Demo\Bar
     friend Baz
     friend Demo\Qux
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
