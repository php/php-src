--TEST--
Readonly property hook validation
--FILE--
<?php

readonly class PositivePoint
{
    public function __construct(
        public int $x { set => $value > 0 ? $value : throw new \Error('Value must be greater 0'); },
        public int $y { set => $value > 0 ? $value : throw new \Error('Value must be greater 0'); },
    ) {}
}

$one = new PositivePoint(1,1);
var_dump($one);

try {
   $two = new PositivePoint(0,1);
} catch (Error $e) {
   echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}


?>
--EXPECTF--
object(PositivePoint)#1 (2) {
  ["x"]=>
  int(1)
  ["y"]=>
  int(1)
}
Error: Value must be greater 0
