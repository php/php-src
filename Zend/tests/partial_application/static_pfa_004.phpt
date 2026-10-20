--TEST--
PFA of non-static closure is not static
--FILE--
<?php

$c = function ($a) {
    try {
        var_dump($this, $this === $a);
    } catch (Error $e) {
        echo $e::class, ": ", $e->getMessage(), "\n";
    }
};

echo "# Original PFA\n";

$f = $c(?);
echo new ReflectionFunction($f), "\n";
$f($c);

echo "# Re-bound PFA\n";

$c2 = new class {};
$f->bindTo($c2)($c2);

?>
--EXPECTF--
# Original PFA
Closure [ <user> function {closure:pfa:%s:13} ] {
  @@ %s 13 - 13

  - Bound Variables [1] {
      Variable #0 [ $fn ]
  }

  - Parameters [1] {
    Parameter #0 [ <required> $a ]
  }
}

Error: Using $this when not in object context
# Re-bound PFA
object(class@anonymous)#3 (0) {
}
bool(true)
