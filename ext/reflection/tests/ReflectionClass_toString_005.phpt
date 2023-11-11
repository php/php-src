--TEST--
Using ReflectionClass::__toString() on readonly classes
--FILE--
<?php

readonly class Foo {
    public int $bar;
    public readonly int $baz;
}

echo new ReflectionClass(Foo::class);

?>
--EXPECTF--
Class [ <user> readonly class Foo ] {
  @@ %s 3-6

  - Constants [0] {
  }

  - Static properties [0] {
  }

  - Static methods [0] {
  }

  - Properties [2] {
    Property [ public readonly int $bar ]
    Property [ public readonly int $baz ]
  }

  - Methods [0] {
  }
}
