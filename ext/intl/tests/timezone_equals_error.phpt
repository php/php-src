--TEST--
IntlTimeZone equals handler: error test
--EXTENSIONS--
intl
--FILE--
<?php

class A extends IntlTimeZone {
function __construct() {}
}

$tz = new A();
$tz2 = intltz_get_gmt();
var_dump($tz, $tz2);
try {
	var_dump($tz == $tz2);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
object(A)#1 (1) {
  ["valid"]=>
  bool(false)
}
object(IntlTimeZone)#2 (4) {
  ["valid"]=>
  bool(true)
  ["id"]=>
  string(3) "GMT"
  ["rawOffset"]=>
  int(0)
  ["currentOffset"]=>
  int(0)
}
Exception: Comparison with at least one unconstructed IntlTimeZone operand
