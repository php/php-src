--TEST--
__invoke() can be partially applied
--FILE--
<?php

class C {
    public function __invoke(int $a, object $b): C {
        var_dump($a, $b);
        return $this;
    }
}

$c = new C();
$f = $c(?, ...);

echo (string) new ReflectionFunction($f), "\n";

$f = $c(?, new stdClass);

echo (string) new ReflectionFunction($f), "\n";

$f(1);

?>
--EXPECTF--
Closure [ <user> public method {closure:%s:%d} ] {
  @@ %s.php 11 - 11

  - Parameters [2] {
    Parameter #0 [ <required> int $a ]
    Parameter #1 [ <required> object $b ]
  }
  - Return [ C ]
}

Closure [ <user> public method {closure:%s:%d} ] {
  @@ %s.php 15 - 15

  - Bound Variables [1] {
      Variable #0 [ $b ]
  }

  - Parameters [1] {
    Parameter #0 [ <required> int $a ]
  }
  - Return [ C ]
}

int(1)
object(stdClass)#%d (0) {
}
