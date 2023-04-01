--TEST--
Using Reflection on anon class captured properties
--FILE--
<?php

$x = 42;
$y = "123";
$anon = new class use (
    $x as public int,
    $y as public string
) {
    public $z;
};

$rc = new ReflectionClass($anon);
echo $rc, "\n";

$y = $rc->getProperty('y');
var_dump($y->isPromoted());
var_dump($y->isCaptured());
$z = $rc->getProperty('z');
var_dump($z->isPromoted());
var_dump($z->isCaptured());

echo "\n";

$rp = new ReflectionParameter([$anon, '__construct'], 'y');
var_dump($rp->isPromoted());
var_dump($rp->isCaptured());;

?>
--EXPECTF--
Class [ <user> class class@anonymous ] {
  @@ %s 5-10

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
      @@ %s 6 - 7

      - Parameters [2] {
        Parameter #0 [ <required> int $x ]
        Parameter #1 [ <required> string $y ]
      }
    }
  }
}

bool(false)
bool(true)
bool(false)
bool(false)

bool(false)
bool(true)
