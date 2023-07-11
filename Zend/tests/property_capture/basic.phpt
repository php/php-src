--TEST--
Property capture, with renaming and types
--FILE--
<?php
$a = $b = $c = $d = $e = $f = 42;
$g = $h = new stdClass;

$foo = new class use (
    $a,
    $b as $b_renamed,
    $c as int,
    $d as int $d_renamed,
    $e as string|int,
    $f as string|int $f_renamed,
    $g as stdClass,
    $h as stdClass $h_renamed,
) {};

var_dump($foo);

echo (new ReflectionClass($foo));
?>
--EXPECTF--
object(class@anonymous)#%d (8) {
  ["a"]=>
  int(42)
  ["b_renamed"]=>
  int(42)
  ["c"]=>
  int(42)
  ["d_renamed"]=>
  int(42)
  ["e"]=>
  int(42)
  ["f_renamed"]=>
  int(42)
  ["g"]=>
  object(stdClass)#%d (0) {
  }
  ["h_renamed"]=>
  object(stdClass)#%d (0) {
  }
}
Class [ <user> class class@anonymous ] {
  @@ %s 5-14

  - Constants [0] {
  }

  - Static properties [0] {
  }

  - Static methods [0] {
  }

  - Properties [8] {
    Property [ public $a = NULL ]
    Property [ public $b_renamed = NULL ]
    Property [ public int $c ]
    Property [ public int $d_renamed ]
    Property [ public string|int $e ]
    Property [ public string|int $f_renamed ]
    Property [ public stdClass $g ]
    Property [ public stdClass $h_renamed ]
  }

  - Methods [1] {
    Method [ <user, ctor> public method __construct ] {
      @@ %s 6 - 13

      - Parameters [8] {
        Parameter #0 [ <required> $a ]
        Parameter #1 [ <required> $b_renamed ]
        Parameter #2 [ <required> int $c ]
        Parameter #3 [ <required> int $d_renamed ]
        Parameter #4 [ <required> string|int $e ]
        Parameter #5 [ <required> string|int $f_renamed ]
        Parameter #6 [ <required> stdClass $g ]
        Parameter #7 [ <required> stdClass $h_renamed ]
      }
    }
  }
}
