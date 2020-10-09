--TEST--
Fetching default value of an internal trampoline function with userland arginfo
--FILE--
<?php
$closure = function ($b = 0) {};
$ro = new ReflectionObject($closure);
$rm = $ro->getMethod('__invoke');
echo $rm, "\n";
?>
--EXPECT--
Method [ <internal> public method __invoke ] {

  - Parameters [1] {
    Parameter #0 [ <optional> $b = <default> ]
  }
}
