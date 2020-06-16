--TEST--
Using Reflection on promoted properties
--FILE--
<?php

class Test {
    public $z;
    public function __construct(
        public int $x,
        /** @SomeAnnotation() */
        public string $y = "123",
        string $z = "abc",
    ) {}
}

$rc = new ReflectionClass(Test::class);
echo $rc, "\n";

$y = $rc->getProperty('y');
var_dump($y->isPromoted());
var_dump($y->getDocComment());
$z = $rc->getProperty('z');
var_dump($z->isPromoted());

echo "\n";

$rp = new ReflectionParameter([Test::class, '__construct'], 'y');
var_dump($rp->isPromoted());
$rp = new ReflectionParameter([Test::class, '__construct'], 'z');
var_dump($rp->isPromoted());

?>
--EXPECTF--
Class [ <user> class Test ] {
  @@ %s 3-11

  - Constants [0] {
  }

  - Static properties [0] {
  }

  - Static methods [0] {
  }

  - Properties [3] {
    Property [ public $z = NULL ]
    Property [ public int $x ]
    Property [ public string $y ]
  }

  - Methods [1] {
    Method [ <user, ctor> public method __construct ] {
      @@ %s 5 - 10

      - Parameters [3] {
        Parameter #0 [ <required> int $x ]
        Parameter #1 [ <optional> string $y = '123' ]
        Parameter #2 [ <optional> string $z = 'abc' ]
      }
    }
  }
}

bool(true)
string(24) "/** @SomeAnnotation() */"
bool(false)

bool(true)
bool(false)
