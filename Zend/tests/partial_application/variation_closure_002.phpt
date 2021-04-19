--TEST--
Partial application variation closure __invoke
--FILE--
<?php
$closure = function($a, $b) {
    var_dump($a, $b);
};

$function = $closure->__invoke(1, ?);

echo (string) new ReflectionFunction($function);

$function(10);
?>
--EXPECTF--
Method [ <internal, prototype Closure> partial public method __invoke ] {

  - Parameters [1] {
    Parameter #0 [ <required> $b ]
  }
}
int(1)
int(10)
