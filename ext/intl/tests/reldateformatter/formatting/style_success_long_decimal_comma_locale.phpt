--TEST--
Test IntlRelativeDateTimeFormatter formatting - long style - locale with a decimal comma
--EXTENSIONS--
intl
--FILE--
<?php

require_once __DIR__ . '/format.inc';

printRelativeDateTimeTable(
    new IntlRelativeDateTimeFormatter('de_DE', IntlRelativeDateTimeFormatter::STYLE_LONG),
    offsets: [-2.75, -1.75, -1.5, 1.5, 1.75, 2.75],
    units: [
        'day' => IntlRelativeDateTimeFormatter::UNIT_DAY,
        'hour' => IntlRelativeDateTimeFormatter::UNIT_HOUR,
        'minute' => IntlRelativeDateTimeFormatter::UNIT_MINUTE,
    ],
);

?>
--EXPECT--
=== day ==================================

offset  format            formatNumeric
------  ----------------  ----------------
 -2.75  vor 2,75 Tagen    vor 2,75 Tagen
 -1.75  vor 1,75 Tagen    vor 1,75 Tagen
  -1.5  vor 1,5 Tagen     vor 1,5 Tagen
   1.5  in 1,5 Tagen      in 1,5 Tagen
  1.75  in 1,75 Tagen     in 1,75 Tagen
  2.75  in 2,75 Tagen     in 2,75 Tagen

=== hour =================================

offset  format            formatNumeric
------  ----------------  ----------------
 -2.75  vor 2,75 Stunden  vor 2,75 Stunden
 -1.75  vor 1,75 Stunden  vor 1,75 Stunden
  -1.5  vor 1,5 Stunden   vor 1,5 Stunden
   1.5  in 1,5 Stunden    in 1,5 Stunden
  1.75  in 1,75 Stunden   in 1,75 Stunden
  2.75  in 2,75 Stunden   in 2,75 Stunden

=== minute ===============================

offset  format            formatNumeric
------  ----------------  ----------------
 -2.75  vor 2,75 Minuten  vor 2,75 Minuten
 -1.75  vor 1,75 Minuten  vor 1,75 Minuten
  -1.5  vor 1,5 Minuten   vor 1,5 Minuten
   1.5  in 1,5 Minuten    in 1,5 Minuten
  1.75  in 1,75 Minuten   in 1,75 Minuten
  2.75  in 2,75 Minuten   in 2,75 Minuten
