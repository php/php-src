--TEST--
Test IntlRelativeDateTimeFormatter formatting - narrow style - all units and fractional offsets
--EXTENSIONS--
intl
--SKIPIF--
<?php
if (version_compare(INTL_ICU_VERSION, '65.0', '<')) {
    die('skip ICU >= 65.0 required for relative hour and minute names');
}
?>
--FILE--
<?php

require_once __DIR__ . '/format.inc';

$formatter = new IntlRelativeDateTimeFormatter('en_US', IntlRelativeDateTimeFormatter::STYLE_NARROW);
printRelativeDateTimeTable($formatter);

?>
--EXPECT--
=== year =========================

offset  format       formatNumeric
------  -----------  -------------
    -3  3y ago       3y ago
    -2  2y ago       2y ago
 -1.75  1.75y ago    1.75y ago
  -1.5  1.5y ago     1.5y ago
    -1  last yr.     1y ago
  -0.5  0.5y ago     0.5y ago
     0  this yr.     in 0y
     0  this yr.     in 0y
    -0  this yr.     0y ago
   0.5  in 0.5y      in 0.5y
     1  next yr.     in 1y
   1.5  in 1.5y      in 1.5y
  1.75  in 1.75y     in 1.75y
     2  in 2y        in 2y
     3  in 3y        in 3y

=== quarter ======================

offset  format       formatNumeric
------  -----------  -------------
    -3  3q ago       3q ago
    -2  2q ago       2q ago
 -1.75  1.75q ago    1.75q ago
  -1.5  1.5q ago     1.5q ago
    -1  last qtr.    1q ago
  -0.5  0.5q ago     0.5q ago
     0  this qtr.    in 0q
     0  this qtr.    in 0q
    -0  this qtr.    0q ago
   0.5  in 0.5q      in 0.5q
     1  next qtr.    in 1q
   1.5  in 1.5q      in 1.5q
  1.75  in 1.75q     in 1.75q
     2  in 2q        in 2q
     3  in 3q        in 3q

=== month ========================

offset  format       formatNumeric
------  -----------  -------------
    -3  3mo ago      3mo ago
    -2  2mo ago      2mo ago
 -1.75  1.75mo ago   1.75mo ago
  -1.5  1.5mo ago    1.5mo ago
    -1  last mo.     1mo ago
  -0.5  0.5mo ago    0.5mo ago
     0  this mo.     in 0mo
     0  this mo.     in 0mo
    -0  this mo.     0mo ago
   0.5  in 0.5mo     in 0.5mo
     1  next mo.     in 1mo
   1.5  in 1.5mo     in 1.5mo
  1.75  in 1.75mo    in 1.75mo
     2  in 2mo       in 2mo
     3  in 3mo       in 3mo

=== week =========================

offset  format       formatNumeric
------  -----------  -------------
    -3  3w ago       3w ago
    -2  2w ago       2w ago
 -1.75  1.75w ago    1.75w ago
  -1.5  1.5w ago     1.5w ago
    -1  last wk.     1w ago
  -0.5  0.5w ago     0.5w ago
     0  this wk.     in 0w
     0  this wk.     in 0w
    -0  this wk.     0w ago
   0.5  in 0.5w      in 0.5w
     1  next wk.     in 1w
   1.5  in 1.5w      in 1.5w
  1.75  in 1.75w     in 1.75w
     2  in 2w        in 2w
     3  in 3w        in 3w

=== day ==========================

offset  format       formatNumeric
------  -----------  -------------
    -3  3d ago       3d ago
    -2  2d ago       2d ago
 -1.75  1.75d ago    1.75d ago
  -1.5  1.5d ago     1.5d ago
    -1  yesterday    1d ago
  -0.5  0.5d ago     0.5d ago
     0  today        in 0d
     0  today        in 0d
    -0  today        0d ago
   0.5  in 0.5d      in 0.5d
     1  tomorrow     in 1d
   1.5  in 1.5d      in 1.5d
  1.75  in 1.75d     in 1.75d
     2  in 2d        in 2d
     3  in 3d        in 3d

=== hour =========================

offset  format       formatNumeric
------  -----------  -------------
    -3  3h ago       3h ago
    -2  2h ago       2h ago
 -1.75  1.75h ago    1.75h ago
  -1.5  1.5h ago     1.5h ago
    -1  1h ago       1h ago
  -0.5  0.5h ago     0.5h ago
     0  this hour    in 0h
     0  this hour    in 0h
    -0  this hour    0h ago
   0.5  in 0.5h      in 0.5h
     1  in 1h        in 1h
   1.5  in 1.5h      in 1.5h
  1.75  in 1.75h     in 1.75h
     2  in 2h        in 2h
     3  in 3h        in 3h

=== minute =======================

offset  format       formatNumeric
------  -----------  -------------
    -3  3m ago       3m ago
    -2  2m ago       2m ago
 -1.75  1.75m ago    1.75m ago
  -1.5  1.5m ago     1.5m ago
    -1  1m ago       1m ago
  -0.5  0.5m ago     0.5m ago
     0  this minute  in 0m
     0  this minute  in 0m
    -0  this minute  0m ago
   0.5  in 0.5m      in 0.5m
     1  in 1m        in 1m
   1.5  in 1.5m      in 1.5m
  1.75  in 1.75m     in 1.75m
     2  in 2m        in 2m
     3  in 3m        in 3m

=== second =======================

offset  format       formatNumeric
------  -----------  -------------
    -3  3s ago       3s ago
    -2  2s ago       2s ago
 -1.75  1.75s ago    1.75s ago
  -1.5  1.5s ago     1.5s ago
    -1  1s ago       1s ago
  -0.5  0.5s ago     0.5s ago
     0  now          in 0s
     0  now          in 0s
    -0  now          0s ago
   0.5  in 0.5s      in 0.5s
     1  in 1s        in 1s
   1.5  in 1.5s      in 1.5s
  1.75  in 1.75s     in 1.75s
     2  in 2s        in 2s
     3  in 3s        in 3s

=== sunday =======================

offset  format       formatNumeric
------  -----------  -------------
    -3  3 Su ago     3 Su ago
    -2  2 Su ago     2 Su ago
 -1.75  1.75 Su ago  1.75 Su ago
  -1.5  1.5 Su ago   1.5 Su ago
    -1  last Su      1 Su ago
  -0.5  0.5 Su ago   0.5 Su ago
     0  this Su      in 0 Su
     0  this Su      in 0 Su
    -0  this Su      0 Su ago
   0.5  in 0.5 Su    in 0.5 Su
     1  next Su      in 1 Su
   1.5  in 1.5 Su    in 1.5 Su
  1.75  in 1.75 Su   in 1.75 Su
     2  in 2 Su      in 2 Su
     3  in 3 Su      in 3 Su

=== monday =======================

offset  format       formatNumeric
------  -----------  -------------
    -3  3 M ago      3 M ago
    -2  2 M ago      2 M ago
 -1.75  1.75 M ago   1.75 M ago
  -1.5  1.5 M ago    1.5 M ago
    -1  last M       1 M ago
  -0.5  0.5 M ago    0.5 M ago
     0  this M       in 0 M
     0  this M       in 0 M
    -0  this M       0 M ago
   0.5  in 0.5 M     in 0.5 M
     1  next M       in 1 M
   1.5  in 1.5 M     in 1.5 M
  1.75  in 1.75 M    in 1.75 M
     2  in 2 M       in 2 M
     3  in 3 M       in 3 M

=== tuesday ======================

offset  format       formatNumeric
------  -----------  -------------
    -3  3 Tu ago     3 Tu ago
    -2  2 Tu ago     2 Tu ago
 -1.75  1.75 Tu ago  1.75 Tu ago
  -1.5  1.5 Tu ago   1.5 Tu ago
    -1  last Tu      1 Tu ago
  -0.5  0.5 Tu ago   0.5 Tu ago
     0  this Tu      in 0 Tu
     0  this Tu      in 0 Tu
    -0  this Tu      0 Tu ago
   0.5  in 0.5 Tu    in 0.5 Tu
     1  next Tu      in 1 Tu
   1.5  in 1.5 Tu    in 1.5 Tu
  1.75  in 1.75 Tu   in 1.75 Tu
     2  in 2 Tu      in 2 Tu
     3  in 3 Tu      in 3 Tu

=== wednesday ====================

offset  format       formatNumeric
------  -----------  -------------
    -3  3 W ago      3 W ago
    -2  2 W ago      2 W ago
 -1.75  1.75 W ago   1.75 W ago
  -1.5  1.5 W ago    1.5 W ago
    -1  last W       1 W ago
  -0.5  0.5 W ago    0.5 W ago
     0  this W       in 0 W
     0  this W       in 0 W
    -0  this W       0 W ago
   0.5  in 0.5 W     in 0.5 W
     1  next W       in 1 W
   1.5  in 1.5 W     in 1.5 W
  1.75  in 1.75 W    in 1.75 W
     2  in 2 W       in 2 W
     3  in 3 W       in 3 W

=== thursday =====================

offset  format       formatNumeric
------  -----------  -------------
    -3  3 Th ago     3 Th ago
    -2  2 Th ago     2 Th ago
 -1.75  1.75 Th ago  1.75 Th ago
  -1.5  1.5 Th ago   1.5 Th ago
    -1  last Th      1 Th ago
  -0.5  0.5 Th ago   0.5 Th ago
     0  this Th      in 0 Th
     0  this Th      in 0 Th
    -0  this Th      0 Th ago
   0.5  in 0.5 Th    in 0.5 Th
     1  next Th      in 1 Th
   1.5  in 1.5 Th    in 1.5 Th
  1.75  in 1.75 Th   in 1.75 Th
     2  in 2 Th      in 2 Th
     3  in 3 Th      in 3 Th

=== friday =======================

offset  format       formatNumeric
------  -----------  -------------
    -3  3 F ago      3 F ago
    -2  2 F ago      2 F ago
 -1.75  1.75 F ago   1.75 F ago
  -1.5  1.5 F ago    1.5 F ago
    -1  last F       1 F ago
  -0.5  0.5 F ago    0.5 F ago
     0  this F       in 0 F
     0  this F       in 0 F
    -0  this F       0 F ago
   0.5  in 0.5 F     in 0.5 F
     1  next F       in 1 F
   1.5  in 1.5 F     in 1.5 F
  1.75  in 1.75 F    in 1.75 F
     2  in 2 F       in 2 F
     3  in 3 F       in 3 F

=== saturday =====================

offset  format       formatNumeric
------  -----------  -------------
    -3  3 Sa ago     3 Sa ago
    -2  2 Sa ago     2 Sa ago
 -1.75  1.75 Sa ago  1.75 Sa ago
  -1.5  1.5 Sa ago   1.5 Sa ago
    -1  last Sa      1 Sa ago
  -0.5  0.5 Sa ago   0.5 Sa ago
     0  this Sa      in 0 Sa
     0  this Sa      in 0 Sa
    -0  this Sa      0 Sa ago
   0.5  in 0.5 Sa    in 0.5 Sa
     1  next Sa      in 1 Sa
   1.5  in 1.5 Sa    in 1.5 Sa
  1.75  in 1.75 Sa   in 1.75 Sa
     2  in 2 Sa      in 2 Sa
     3  in 3 Sa      in 3 Sa
