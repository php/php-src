--TEST--
Partial application variation variadics internal
--FILE--
<?php
$sprintf = sprintf("%d %d %d", 100, ...);

echo (string) new ReflectionFunction($sprintf);

echo $sprintf(1000, 10000);
?>
--EXPECTF--
Function [ <internal:standard> partial function sprintf ] {

  - Parameters [1] {
    Parameter #0 [ <optional> mixed ...$values ]
  }
  - Return [ string ]
}
100 1000 10000
