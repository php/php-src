--TEST--
datefmt_parse/datefmt_localtime references type system
--EXTENSIONS--
intl
--FILE--
<?php

class Test {
    public float $prop = 1.0;
}

$test = new Test;
$offset1 =& $test->prop;
$offset2 =& $test->prop;

$fmt = datefmt_create(
    'en_US',
    IntlDateFormatter::FULL,
    IntlDateFormatter::FULL,
    'America/Los_Angeles',
    IntlDateFormatter::GREGORIAN
);
datefmt_localtime($fmt, 'Wednesday, December 31, 1969 4:00:00 PM PT', $offset1);
datefmt_parse($fmt, 'Wednesday, December 31, 1969 4:00:00 PM PT', $offset2);
var_dump($offset1, $offset2);
var_dump($test);

?>
--EXPECT--
float(1)
float(1)
object(Test)#1 (1) {
  ["prop"]=>
  &float(1)
}
