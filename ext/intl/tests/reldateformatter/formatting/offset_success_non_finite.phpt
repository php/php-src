--TEST--
Test IntlRelativeDateTimeFormatter formatting - non-finite offsets - infinities and NaN
--EXTENSIONS--
intl
--FILE--
<?php

require_once __DIR__ . '/format.inc';

$formatter = new IntlRelativeDateTimeFormatter('en_US');
printRelativeDateTimeTable(
    $formatter,
    offsets: [-INF, INF, NAN],
    units: ['second' => IntlRelativeDateTimeFormatter::UNIT_SECOND],
);

var_dump($formatter->getErrorCode());
var_dump($formatter->getErrorMessage());

?>
--EXPECT--
=== second ===========================

offset  format          formatNumeric
------  --------------  --------------
  -INF  ∞ seconds ago   ∞ seconds ago
   INF  in ∞ seconds    in ∞ seconds
   NAN  in NaN seconds  in NaN seconds
int(0)
string(12) "U_ZERO_ERROR"
