--TEST--
Test IntlRelativeDateTimeFormatter formatting - short style - all units and fractional offsets
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

$formatter = new IntlRelativeDateTimeFormatter('en_US', IntlRelativeDateTimeFormatter::STYLE_SHORT);
printRelativeDateTimeTable($formatter);

?>
--EXPECT--
=== year =============================

offset  format          formatNumeric
------  --------------  --------------
    -3  3 yr. ago       3 yr. ago
    -2  2 yr. ago       2 yr. ago
 -1.75  1.75 yr. ago    1.75 yr. ago
  -1.5  1.5 yr. ago     1.5 yr. ago
    -1  last yr.        1 yr. ago
  -0.5  0.5 yr. ago     0.5 yr. ago
     0  this yr.        in 0 yr.
     0  this yr.        in 0 yr.
    -0  this yr.        0 yr. ago
   0.5  in 0.5 yr.      in 0.5 yr.
     1  next yr.        in 1 yr.
   1.5  in 1.5 yr.      in 1.5 yr.
  1.75  in 1.75 yr.     in 1.75 yr.
     2  in 2 yr.        in 2 yr.
     3  in 3 yr.        in 3 yr.

=== quarter ==========================

offset  format          formatNumeric
------  --------------  --------------
    -3  3 qtrs. ago     3 qtrs. ago
    -2  2 qtrs. ago     2 qtrs. ago
 -1.75  1.75 qtrs. ago  1.75 qtrs. ago
  -1.5  1.5 qtrs. ago   1.5 qtrs. ago
    -1  last qtr.       1 qtr. ago
  -0.5  0.5 qtrs. ago   0.5 qtrs. ago
     0  this qtr.       in 0 qtrs.
     0  this qtr.       in 0 qtrs.
    -0  this qtr.       0 qtrs. ago
   0.5  in 0.5 qtrs.    in 0.5 qtrs.
     1  next qtr.       in 1 qtr.
   1.5  in 1.5 qtrs.    in 1.5 qtrs.
  1.75  in 1.75 qtrs.   in 1.75 qtrs.
     2  in 2 qtrs.      in 2 qtrs.
     3  in 3 qtrs.      in 3 qtrs.

=== month ============================

offset  format          formatNumeric
------  --------------  --------------
    -3  3 mo. ago       3 mo. ago
    -2  2 mo. ago       2 mo. ago
 -1.75  1.75 mo. ago    1.75 mo. ago
  -1.5  1.5 mo. ago     1.5 mo. ago
    -1  last mo.        1 mo. ago
  -0.5  0.5 mo. ago     0.5 mo. ago
     0  this mo.        in 0 mo.
     0  this mo.        in 0 mo.
    -0  this mo.        0 mo. ago
   0.5  in 0.5 mo.      in 0.5 mo.
     1  next mo.        in 1 mo.
   1.5  in 1.5 mo.      in 1.5 mo.
  1.75  in 1.75 mo.     in 1.75 mo.
     2  in 2 mo.        in 2 mo.
     3  in 3 mo.        in 3 mo.

=== week =============================

offset  format          formatNumeric
------  --------------  --------------
    -3  3 wk. ago       3 wk. ago
    -2  2 wk. ago       2 wk. ago
 -1.75  1.75 wk. ago    1.75 wk. ago
  -1.5  1.5 wk. ago     1.5 wk. ago
    -1  last wk.        1 wk. ago
  -0.5  0.5 wk. ago     0.5 wk. ago
     0  this wk.        in 0 wk.
     0  this wk.        in 0 wk.
    -0  this wk.        0 wk. ago
   0.5  in 0.5 wk.      in 0.5 wk.
     1  next wk.        in 1 wk.
   1.5  in 1.5 wk.      in 1.5 wk.
  1.75  in 1.75 wk.     in 1.75 wk.
     2  in 2 wk.        in 2 wk.
     3  in 3 wk.        in 3 wk.

=== day ==============================

offset  format          formatNumeric
------  --------------  --------------
    -3  3 days ago      3 days ago
    -2  2 days ago      2 days ago
 -1.75  1.75 days ago   1.75 days ago
  -1.5  1.5 days ago    1.5 days ago
    -1  yesterday       1 day ago
  -0.5  0.5 days ago    0.5 days ago
     0  today           in 0 days
     0  today           in 0 days
    -0  today           0 days ago
   0.5  in 0.5 days     in 0.5 days
     1  tomorrow        in 1 day
   1.5  in 1.5 days     in 1.5 days
  1.75  in 1.75 days    in 1.75 days
     2  in 2 days       in 2 days
     3  in 3 days       in 3 days

=== hour =============================

offset  format          formatNumeric
------  --------------  --------------
    -3  3 hr. ago       3 hr. ago
    -2  2 hr. ago       2 hr. ago
 -1.75  1.75 hr. ago    1.75 hr. ago
  -1.5  1.5 hr. ago     1.5 hr. ago
    -1  1 hr. ago       1 hr. ago
  -0.5  0.5 hr. ago     0.5 hr. ago
     0  this hour       in 0 hr.
     0  this hour       in 0 hr.
    -0  this hour       0 hr. ago
   0.5  in 0.5 hr.      in 0.5 hr.
     1  in 1 hr.        in 1 hr.
   1.5  in 1.5 hr.      in 1.5 hr.
  1.75  in 1.75 hr.     in 1.75 hr.
     2  in 2 hr.        in 2 hr.
     3  in 3 hr.        in 3 hr.

=== minute ===========================

offset  format          formatNumeric
------  --------------  --------------
    -3  3 min. ago      3 min. ago
    -2  2 min. ago      2 min. ago
 -1.75  1.75 min. ago   1.75 min. ago
  -1.5  1.5 min. ago    1.5 min. ago
    -1  1 min. ago      1 min. ago
  -0.5  0.5 min. ago    0.5 min. ago
     0  this minute     in 0 min.
     0  this minute     in 0 min.
    -0  this minute     0 min. ago
   0.5  in 0.5 min.     in 0.5 min.
     1  in 1 min.       in 1 min.
   1.5  in 1.5 min.     in 1.5 min.
  1.75  in 1.75 min.    in 1.75 min.
     2  in 2 min.       in 2 min.
     3  in 3 min.       in 3 min.

=== second ===========================

offset  format          formatNumeric
------  --------------  --------------
    -3  3 sec. ago      3 sec. ago
    -2  2 sec. ago      2 sec. ago
 -1.75  1.75 sec. ago   1.75 sec. ago
  -1.5  1.5 sec. ago    1.5 sec. ago
    -1  1 sec. ago      1 sec. ago
  -0.5  0.5 sec. ago    0.5 sec. ago
     0  now             in 0 sec.
     0  now             in 0 sec.
    -0  now             0 sec. ago
   0.5  in 0.5 sec.     in 0.5 sec.
     1  in 1 sec.       in 1 sec.
   1.5  in 1.5 sec.     in 1.5 sec.
  1.75  in 1.75 sec.    in 1.75 sec.
     2  in 2 sec.       in 2 sec.
     3  in 3 sec.       in 3 sec.

=== sunday ===========================

offset  format          formatNumeric
------  --------------  --------------
    -3  3 Sun. ago      3 Sun. ago
    -2  2 Sun. ago      2 Sun. ago
 -1.75  1.75 Sun. ago   1.75 Sun. ago
  -1.5  1.5 Sun. ago    1.5 Sun. ago
    -1  last Sun.       1 Sun. ago
  -0.5  0.5 Sun. ago    0.5 Sun. ago
     0  this Sun.       in 0 Sun.
     0  this Sun.       in 0 Sun.
    -0  this Sun.       0 Sun. ago
   0.5  in 0.5 Sun.     in 0.5 Sun.
     1  next Sun.       in 1 Sun.
   1.5  in 1.5 Sun.     in 1.5 Sun.
  1.75  in 1.75 Sun.    in 1.75 Sun.
     2  in 2 Sun.       in 2 Sun.
     3  in 3 Sun.       in 3 Sun.

=== monday ===========================

offset  format          formatNumeric
------  --------------  --------------
    -3  3 Mon. ago      3 Mon. ago
    -2  2 Mon. ago      2 Mon. ago
 -1.75  1.75 Mon. ago   1.75 Mon. ago
  -1.5  1.5 Mon. ago    1.5 Mon. ago
    -1  last Mon.       1 Mon. ago
  -0.5  0.5 Mon. ago    0.5 Mon. ago
     0  this Mon.       in 0 Mon.
     0  this Mon.       in 0 Mon.
    -0  this Mon.       0 Mon. ago
   0.5  in 0.5 Mon.     in 0.5 Mon.
     1  next Mon.       in 1 Mon.
   1.5  in 1.5 Mon.     in 1.5 Mon.
  1.75  in 1.75 Mon.    in 1.75 Mon.
     2  in 2 Mon.       in 2 Mon.
     3  in 3 Mon.       in 3 Mon.

=== tuesday ==========================

offset  format          formatNumeric
------  --------------  --------------
    -3  3 Tue. ago      3 Tue. ago
    -2  2 Tue. ago      2 Tue. ago
 -1.75  1.75 Tue. ago   1.75 Tue. ago
  -1.5  1.5 Tue. ago    1.5 Tue. ago
    -1  last Tue.       1 Tue. ago
  -0.5  0.5 Tue. ago    0.5 Tue. ago
     0  this Tue.       in 0 Tue.
     0  this Tue.       in 0 Tue.
    -0  this Tue.       0 Tue. ago
   0.5  in 0.5 Tue.     in 0.5 Tue.
     1  next Tue.       in 1 Tue.
   1.5  in 1.5 Tue.     in 1.5 Tue.
  1.75  in 1.75 Tue.    in 1.75 Tue.
     2  in 2 Tue.       in 2 Tue.
     3  in 3 Tue.       in 3 Tue.

=== wednesday ========================

offset  format          formatNumeric
------  --------------  --------------
    -3  3 Wed. ago      3 Wed. ago
    -2  2 Wed. ago      2 Wed. ago
 -1.75  1.75 Wed. ago   1.75 Wed. ago
  -1.5  1.5 Wed. ago    1.5 Wed. ago
    -1  last Wed.       1 Wed. ago
  -0.5  0.5 Wed. ago    0.5 Wed. ago
     0  this Wed.       in 0 Wed.
     0  this Wed.       in 0 Wed.
    -0  this Wed.       0 Wed. ago
   0.5  in 0.5 Wed.     in 0.5 Wed.
     1  next Wed.       in 1 Wed.
   1.5  in 1.5 Wed.     in 1.5 Wed.
  1.75  in 1.75 Wed.    in 1.75 Wed.
     2  in 2 Wed.       in 2 Wed.
     3  in 3 Wed.       in 3 Wed.

=== thursday =========================

offset  format          formatNumeric
------  --------------  --------------
    -3  3 Thu. ago      3 Thu. ago
    -2  2 Thu. ago      2 Thu. ago
 -1.75  1.75 Thu. ago   1.75 Thu. ago
  -1.5  1.5 Thu. ago    1.5 Thu. ago
    -1  last Thu.       1 Thu. ago
  -0.5  0.5 Thu. ago    0.5 Thu. ago
     0  this Thu.       in 0 Thu.
     0  this Thu.       in 0 Thu.
    -0  this Thu.       0 Thu. ago
   0.5  in 0.5 Thu.     in 0.5 Thu.
     1  next Thu.       in 1 Thu.
   1.5  in 1.5 Thu.     in 1.5 Thu.
  1.75  in 1.75 Thu.    in 1.75 Thu.
     2  in 2 Thu.       in 2 Thu.
     3  in 3 Thu.       in 3 Thu.

=== friday ===========================

offset  format          formatNumeric
------  --------------  --------------
    -3  3 Fri. ago      3 Fri. ago
    -2  2 Fri. ago      2 Fri. ago
 -1.75  1.75 Fri. ago   1.75 Fri. ago
  -1.5  1.5 Fri. ago    1.5 Fri. ago
    -1  last Fri.       1 Fri. ago
  -0.5  0.5 Fri. ago    0.5 Fri. ago
     0  this Fri.       in 0 Fri.
     0  this Fri.       in 0 Fri.
    -0  this Fri.       0 Fri. ago
   0.5  in 0.5 Fri.     in 0.5 Fri.
     1  next Fri.       in 1 Fri.
   1.5  in 1.5 Fri.     in 1.5 Fri.
  1.75  in 1.75 Fri.    in 1.75 Fri.
     2  in 2 Fri.       in 2 Fri.
     3  in 3 Fri.       in 3 Fri.

=== saturday =========================

offset  format          formatNumeric
------  --------------  --------------
    -3  3 Sat. ago      3 Sat. ago
    -2  2 Sat. ago      2 Sat. ago
 -1.75  1.75 Sat. ago   1.75 Sat. ago
  -1.5  1.5 Sat. ago    1.5 Sat. ago
    -1  last Sat.       1 Sat. ago
  -0.5  0.5 Sat. ago    0.5 Sat. ago
     0  this Sat.       in 0 Sat.
     0  this Sat.       in 0 Sat.
    -0  this Sat.       0 Sat. ago
   0.5  in 0.5 Sat.     in 0.5 Sat.
     1  next Sat.       in 1 Sat.
   1.5  in 1.5 Sat.     in 1.5 Sat.
  1.75  in 1.75 Sat.    in 1.75 Sat.
     2  in 2 Sat.       in 2 Sat.
     3  in 3 Sat.       in 3 Sat.
