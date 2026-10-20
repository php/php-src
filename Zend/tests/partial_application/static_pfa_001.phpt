--TEST--
PFA of static method is static
--FILE--
<?php

class C {
    public static function f($a) {
        var_dump($a);
    }
}

$f = C::f(?);
echo new ReflectionFunction($f), "\n";
$f(1);

$f = (new C)->f(?);
echo new ReflectionFunction($f), "\n";
$f(1);

// Warns
var_dump($f->bindTo(new C));

?>
--EXPECTF--
Closure [ <user> static public method {closure:pfa:%s:9} ] {
  @@ %s 9 - 9

  - Parameters [1] {
    Parameter #0 [ <required> $a ]
  }
}

int(1)
Closure [ <user> static public method {closure:pfa:%s:13} ] {
  @@ %s 13 - 13

  - Parameters [1] {
    Parameter #0 [ <required> $a ]
  }
}

int(1)

Warning: Cannot bind an instance to a static closure, this will be an error in PHP 9 in %s on line %d
NULL
