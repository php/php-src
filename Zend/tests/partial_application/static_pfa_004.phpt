--TEST--
PFA of non-static closure is not static
--FILE--
<?php

$c = function ($a) {
    try {
        var_dump($this, $this === $a);
    } catch (Error $e) {
        var_dump(null, false);
    }
};

$f = $c(?);
echo new ReflectionFunction($f), "\n";
$f($c);

$c2 = new class {};
$f->bindTo($c2)($c2);

?>
--EXPECTF--
Closure [ <user> function {closure:pfa:%s:11} ] {
  @@ %s 11 - 11

  - Bound Variables [1] {
      Variable #0 [ $fn ]
  }

  - Parameters [1] {
    Parameter #0 [ <required> $a ]
  }
}

NULL
bool(false)
object(class@anonymous)#3 (0) {
}
bool(true)
