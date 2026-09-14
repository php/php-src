--TEST--
Test IntlRelativeDateTimeFormatter formatting - short style - locale with a decimal comma
--EXTENSIONS--
intl
--FILE--
<?php

require_once __DIR__ . '/format.inc';

printRelativeDateTimeTable(
    new IntlRelativeDateTimeFormatter('de_DE', IntlRelativeDateTimeFormatter::STYLE_SHORT),
    offsets: [-2.75, -1.75, -1.5, 1.5, 1.75, 2.75],
    units: [
        'day' => IntlRelativeDateTimeFormatter::UNIT_DAY,
        'hour' => IntlRelativeDateTimeFormatter::UNIT_HOUR,
        'minute' => IntlRelativeDateTimeFormatter::UNIT_MINUTE,
    ],
);

?>
--EXPECT--
=== day ==============================

offset  format          formatNumeric
------  --------------  --------------
 -2.75  vor 2,75 Tagen  vor 2,75 Tagen
 -1.75  vor 1,75 Tagen  vor 1,75 Tagen
  -1.5  vor 1,5 Tagen   vor 1,5 Tagen
   1.5  in 1,5 Tagen    in 1,5 Tagen
  1.75  in 1,75 Tagen   in 1,75 Tagen
  2.75  in 2,75 Tagen   in 2,75 Tagen

=== hour =============================

offset  format          formatNumeric
------  --------------  --------------
 -2.75  vor 2,75 Std.   vor 2,75 Std.
 -1.75  vor 1,75 Std.   vor 1,75 Std.
  -1.5  vor 1,5 Std.    vor 1,5 Std.
   1.5  in 1,5 Std.     in 1,5 Std.
  1.75  in 1,75 Std.    in 1,75 Std.
  2.75  in 2,75 Std.    in 2,75 Std.

=== minute ===========================

offset  format          formatNumeric
------  --------------  --------------
 -2.75  vor 2,75 Min.   vor 2,75 Min.
 -1.75  vor 1,75 Min.   vor 1,75 Min.
  -1.5  vor 1,5 Min.    vor 1,5 Min.
   1.5  in 1,5 Min.     in 1,5 Min.
  1.75  in 1,75 Min.    in 1,75 Min.
  2.75  in 2,75 Min.    in 2,75 Min.
