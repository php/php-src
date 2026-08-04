--TEST--
PFA of standalone function is static
--FILE--
<?php

function f($a) {
    var_dump($a);
}

$f = f(?);
echo new ReflectionFunction($f), "\n";
$f(1);

// Warns
var_dump($f->bindTo(new class {}));

?>
--EXPECTF--
Closure [ <user> static function {closure:pfa:%s:7} ] {
  @@ %s 7 - 7

  - Parameters [1] {
    Parameter #0 [ <required> $a ]
  }
}

int(1)

Warning: Cannot bind an instance to a static closure, this will be an error in PHP 9 in %s on line %d
NULL
