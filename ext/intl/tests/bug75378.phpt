--TEST--
Bug #75378 ([REGRESSION] IntlDateFormatter::parse() does not change $position argument)
--EXTENSIONS--
intl
--FILE--
<?php

$formatter = new IntlDateFormatter("en-GB", IntlDateFormatter::NONE, IntlDateFormatter::NONE, 'UTC', null, 'yyyy-MM-dd');
$position = 0;
$parsedDate = $formatter->parse("2017-10-12", $position);
var_dump($position);
$localdate = $formatter->localtime("2017-10-12 00:00:00", $position1);
var_dump($position1);
?>
--EXPECT--
int(10)
int(10)
