--TEST--
Test IntlRelativeDateTimeFormatter formatting - large offsets - grouping and values beyond integer range
--EXTENSIONS--
intl
--FILE--
<?php

require_once __DIR__ . '/format.inc';

printRelativeDateTimeTable(
    new IntlRelativeDateTimeFormatter('en_US'),
    offsets: [-1e20, -1e6, 1e6, 1e20],
    units: ['second' => IntlRelativeDateTimeFormatter::UNIT_SECOND],
);

?>
--EXPECT--
=== second ===============================================================================

  offset  format                                   formatNumeric
--------  ---------------------------------------  ---------------------------------------
-1.0E+20  100,000,000,000,000,000,000 seconds ago  100,000,000,000,000,000,000 seconds ago
-1000000  1,000,000 seconds ago                    1,000,000 seconds ago
 1000000  in 1,000,000 seconds                     in 1,000,000 seconds
 1.0E+20  in 100,000,000,000,000,000,000 seconds   in 100,000,000,000,000,000,000 seconds
