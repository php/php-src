--TEST--
IntlDateFormatter::parseToCalendar() reference type system breaks
--EXTENSIONS--
intl
--FILE--
<?php
class Test {
    public float $prop = 0.0;
}
$test = new Test;
$offset =& $test->prop;

$oIntlDateFormatter = new IntlDateFormatter("en_GB");
$oIntlDateFormatter->setTimeZone('Europe/Berlin');
$oIntlDateFormatter->setPattern('VV');
var_dump($oIntlDateFormatter->parseToCalendar('America/Los_Angeles', $offset));
var_dump($offset);
var_dump($test);
?>
--EXPECTF--
int(%d)
float(%f)
object(Test)#%d (1) {
  ["prop"]=>
  &float(%f)
}
