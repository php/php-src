--TEST--
Property capture visibility modifiers
--FILE--
<?php
$a0 = $b0 = 0;
$a1 = $b1 = $c1 = $d1 = $e1 = $f1 = 1;
$a2 = $b2 = $c2 = $d2 = $e2 = $f2 = 2;
$a3 = $b3 = $c3 = $d3 = $e3 = $f3 = 3;

$eoo = new class use (
    $a0 as readonly int,
    $b0 as readonly int $b0_renamed,

    $a1 as public,
    $b1 as public $b1_renamed,
    $c1 as public int,
    $d1 as public int $d1_renamed,
    $e1 as public readonly int,
    $f1 as public readonly int $f1_renamed,

    $a2 as protected,
    $b2 as protected $b2_renamed,
    $c2 as protected int,
    $d2 as protected int $d2_renamed,
    $e2 as protected readonly int,
    $f2 as protected readonly int $f2_renamed,

    $a3 as private,
    $b3 as private $b3_renamed,
    $c3 as private int,
    $d3 as private int $d3_renamed,
    $e3 as private readonly int,
    $f3 as private readonly int $f3_renamed,
) {};

var_dump($eoo);

echo (new ReflectionClass($eoo));
?>
--EXPECTF--
object(class@anonymous)#1 (20) {
  ["a0"]=>
  int(0)
  ["b0_renamed"]=>
  int(0)
  ["a1"]=>
  int(1)
  ["b1_renamed"]=>
  int(1)
  ["c1"]=>
  int(1)
  ["d1_renamed"]=>
  int(1)
  ["e1"]=>
  int(1)
  ["f1_renamed"]=>
  int(1)
  ["a2":protected]=>
  int(2)
  ["b2_renamed":protected]=>
  int(2)
  ["c2":protected]=>
  int(2)
  ["d2_renamed":protected]=>
  int(2)
  ["e2":protected]=>
  int(2)
  ["f2_renamed":protected]=>
  int(2)
  ["a3":"class@anonymous":private]=>
  int(3)
  ["b3_renamed":"class@anonymous":private]=>
  int(3)
  ["c3":"class@anonymous":private]=>
  int(3)
  ["d3_renamed":"class@anonymous":private]=>
  int(3)
  ["e3":"class@anonymous":private]=>
  int(3)
  ["f3_renamed":"class@anonymous":private]=>
  int(3)
}
Class [ <user> class class@anonymous ] {
  @@ %s 7-31

  - Constants [0] {
  }

  - Static properties [0] {
  }

  - Static methods [0] {
  }

  - Properties [20] {
    Property [ public readonly int $a0 ]
    Property [ public readonly int $b0_renamed ]
    Property [ public $a1 = NULL ]
    Property [ public $b1_renamed = NULL ]
    Property [ public int $c1 ]
    Property [ public int $d1_renamed ]
    Property [ public readonly int $e1 ]
    Property [ public readonly int $f1_renamed ]
    Property [ protected $a2 = NULL ]
    Property [ protected $b2_renamed = NULL ]
    Property [ protected int $c2 ]
    Property [ protected int $d2_renamed ]
    Property [ protected readonly int $e2 ]
    Property [ protected readonly int $f2_renamed ]
    Property [ private $a3 = NULL ]
    Property [ private $b3_renamed = NULL ]
    Property [ private int $c3 ]
    Property [ private int $d3_renamed ]
    Property [ private readonly int $e3 ]
    Property [ private readonly int $f3_renamed ]
  }

  - Methods [1] {
    Method [ <user, ctor> public method __construct ] {
      @@ %s 8 - 30

      - Parameters [20] {
        Parameter #0 [ <required> int $a0 ]
        Parameter #1 [ <required> int $b0_renamed ]
        Parameter #2 [ <required> $a1 ]
        Parameter #3 [ <required> $b1_renamed ]
        Parameter #4 [ <required> int $c1 ]
        Parameter #5 [ <required> int $d1_renamed ]
        Parameter #6 [ <required> int $e1 ]
        Parameter #7 [ <required> int $f1_renamed ]
        Parameter #8 [ <required> $a2 ]
        Parameter #9 [ <required> $b2_renamed ]
        Parameter #10 [ <required> int $c2 ]
        Parameter #11 [ <required> int $d2_renamed ]
        Parameter #12 [ <required> int $e2 ]
        Parameter #13 [ <required> int $f2_renamed ]
        Parameter #14 [ <required> $a3 ]
        Parameter #15 [ <required> $b3_renamed ]
        Parameter #16 [ <required> int $c3 ]
        Parameter #17 [ <required> int $d3_renamed ]
        Parameter #18 [ <required> int $e3 ]
        Parameter #19 [ <required> int $f3_renamed ]
      }
    }
  }
}
