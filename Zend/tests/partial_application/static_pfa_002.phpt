--TEST--
PFA of instance method is not static
--FILE--
<?php

class C {
    public function f($a) {
        var_dump($this, $this === $a);
    }
}

$c = new C();
$f = $c->f(?);
echo new ReflectionFunction($f), "\n";
$f($c);

$c2 = new C();
$f->bindTo($c2)($c2);

?>
--EXPECTF--
Closure [ <user> public method {closure:pfa:%s:10} ] {
  @@ %s 10 - 10

  - Parameters [1] {
    Parameter #0 [ <required> $a ]
  }
}

object(C)#%d (0) {
}
bool(true)
object(C)#%d (0) {
}
bool(true)
