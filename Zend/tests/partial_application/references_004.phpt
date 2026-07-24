--TEST--
PFA receives variadic param by ref if the actual function does
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.file_update_protection=0
--FILE--
<?php

function foo($a, &...$args) {
    foreach ($args as &$arg) {
        $arg = -$arg;
    }
}

$b = 2;
$c = 3;
$d = 4;

$foo = foo(1, $b, ?, ...);

echo (string) new ReflectionFunction($foo), "\n";

$foo($c, $d);

var_dump($b, $c, $d);

?>
--EXPECTF--
Closure [ <user> static function {closure:%s:%d} ] {
  @@ %sreferences_004.php 13 - 13

  - Bound Variables [1] {
      Variable #0 [ $args2 ]
  }

  - Parameters [2] {
    Parameter #0 [ <required> &$args0 ]
    Parameter #1 [ <optional> &...$args ]
  }
}

int(-2)
int(-3)
int(-4)
