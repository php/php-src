--TEST--
Test IntlRelativeDateTimeFormatter formatting - offsets near zero - relative and numeric output
--EXTENSIONS--
intl
--FILE--
<?php

require_once __DIR__ . '/format.inc';

printRelativeDateTimeTable(
    new IntlRelativeDateTimeFormatter('en_US'),
    offsets: [-0.001, 0.001],
    units: ['second' => IntlRelativeDateTimeFormatter::UNIT_SECOND],
);

?>
--EXPECT--
=== second ======================

offset  format  formatNumeric
------  ------  -----------------
-0.001  now     0.001 seconds ago
 0.001  now     in 0.001 seconds
