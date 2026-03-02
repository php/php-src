--TEST--
ReflectionClass::__toString() - ensure inherited private props are hidden.
--FILE--
<?php
Class c {
    private $a;
    static private $b;
    public ?int $c = 42;
    public Foo $d;
}

class d extends c {}

echo new ReflectionClass("c"), "\n";
echo new ReflectionClass("d"), "\n";
?>
--EXPECTF--
Class [ <user> class c ] {
  @@ %s 2-7

  - Constants [0] {
  }

  - Static properties [1] {
    Property [ private static $b = NULL ]
  }

  - Static methods [0] {
  }

  - Properties [3] {
    Property [ private $a = NULL ]
    Property [ public ?int $c = 42 ]
    Property [ public Foo $d ]
  }

  - Methods [0] {
  }
}

Class [ <user> class d extends c ] {
  @@ %s 9-9

  - Constants [0] {
  }

  - Static properties [0] {
  }

  - Static methods [0] {
  }

  - Properties [2] {
    Property [ public ?int $c = 42 ]
    Property [ public Foo $d ]
  }

  - Methods [0] {
  }
}
