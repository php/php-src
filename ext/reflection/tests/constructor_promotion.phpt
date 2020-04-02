--TEST--
Using Reflection on promoted properties
--FILE--
<?php

class Test {
    public function __construct(
        public int $x,
        /** @SomeAnnotation() */
        public string $y = "123"
    ) {}
}

$rc = new ReflectionClass(Test::class);
echo $rc, "\n";

$y = $rc->getProperty('y');
var_dump($y->getDocComment());

?>
--EXPECTF--
Class [ <user> class Test ] {
  @@ %s 3-9

  - Constants [0] {
  }

  - Static properties [0] {
  }

  - Static methods [0] {
  }

  - Properties [2] {
    Property [ public int $x ]
    Property [ public string $y ]
  }

  - Methods [1] {
    Method [ <user, ctor> public method __construct ] {
      @@ %s 4 - 8

      - Parameters [2] {
        Parameter #0 [ <required> int $x ]
        Parameter #1 [ <optional> string $y = '123' ]
      }
    }
  }
}

bool(false)
