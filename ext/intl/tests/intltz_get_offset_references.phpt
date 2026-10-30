--TEST--
intltz_get_offset references
--EXTENSIONS--
intl
--FILE--
<?php
$tz = IntlTimeZone::createTimeZone('Europe/Brussels');

class Test {
  public string $a, $b;
}
$test = new Test;
$test->a = $test->b = "hello";

$rawOffset =& $test->a;
$dstOffset =& $test->b;
intltz_get_offset($tz, 0.0, true, $rawOffset, $dstOffset);
var_dump($test);
?>
--EXPECT--
object(Test)#2 (2) {
  ["a"]=>
  &string(7) "3600000"
  ["b"]=>
  &string(1) "0"
}
