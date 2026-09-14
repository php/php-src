--TEST--
Test IntlRelativeDateTimeFormatter formatting - long style - all units and fractional offsets
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

$formatter = new IntlRelativeDateTimeFormatter('en_US', IntlRelativeDateTimeFormatter::STYLE_LONG);
printRelativeDateTimeTable($formatter);

?>
--EXPECT--
=== year =======================================

offset  format               formatNumeric
------  -------------------  -------------------
    -3  3 years ago          3 years ago
    -2  2 years ago          2 years ago
 -1.75  1.75 years ago       1.75 years ago
  -1.5  1.5 years ago        1.5 years ago
    -1  last year            1 year ago
  -0.5  0.5 years ago        0.5 years ago
     0  this year            in 0 years
     0  this year            in 0 years
    -0  this year            0 years ago
   0.5  in 0.5 years         in 0.5 years
     1  next year            in 1 year
   1.5  in 1.5 years         in 1.5 years
  1.75  in 1.75 years        in 1.75 years
     2  in 2 years           in 2 years
     3  in 3 years           in 3 years

=== quarter ====================================

offset  format               formatNumeric
------  -------------------  -------------------
    -3  3 quarters ago       3 quarters ago
    -2  2 quarters ago       2 quarters ago
 -1.75  1.75 quarters ago    1.75 quarters ago
  -1.5  1.5 quarters ago     1.5 quarters ago
    -1  last quarter         1 quarter ago
  -0.5  0.5 quarters ago     0.5 quarters ago
     0  this quarter         in 0 quarters
     0  this quarter         in 0 quarters
    -0  this quarter         0 quarters ago
   0.5  in 0.5 quarters      in 0.5 quarters
     1  next quarter         in 1 quarter
   1.5  in 1.5 quarters      in 1.5 quarters
  1.75  in 1.75 quarters     in 1.75 quarters
     2  in 2 quarters        in 2 quarters
     3  in 3 quarters        in 3 quarters

=== month ======================================

offset  format               formatNumeric
------  -------------------  -------------------
    -3  3 months ago         3 months ago
    -2  2 months ago         2 months ago
 -1.75  1.75 months ago      1.75 months ago
  -1.5  1.5 months ago       1.5 months ago
    -1  last month           1 month ago
  -0.5  0.5 months ago       0.5 months ago
     0  this month           in 0 months
     0  this month           in 0 months
    -0  this month           0 months ago
   0.5  in 0.5 months        in 0.5 months
     1  next month           in 1 month
   1.5  in 1.5 months        in 1.5 months
  1.75  in 1.75 months       in 1.75 months
     2  in 2 months          in 2 months
     3  in 3 months          in 3 months

=== week =======================================

offset  format               formatNumeric
------  -------------------  -------------------
    -3  3 weeks ago          3 weeks ago
    -2  2 weeks ago          2 weeks ago
 -1.75  1.75 weeks ago       1.75 weeks ago
  -1.5  1.5 weeks ago        1.5 weeks ago
    -1  last week            1 week ago
  -0.5  0.5 weeks ago        0.5 weeks ago
     0  this week            in 0 weeks
     0  this week            in 0 weeks
    -0  this week            0 weeks ago
   0.5  in 0.5 weeks         in 0.5 weeks
     1  next week            in 1 week
   1.5  in 1.5 weeks         in 1.5 weeks
  1.75  in 1.75 weeks        in 1.75 weeks
     2  in 2 weeks           in 2 weeks
     3  in 3 weeks           in 3 weeks

=== day ========================================

offset  format               formatNumeric
------  -------------------  -------------------
    -3  3 days ago           3 days ago
    -2  2 days ago           2 days ago
 -1.75  1.75 days ago        1.75 days ago
  -1.5  1.5 days ago         1.5 days ago
    -1  yesterday            1 day ago
  -0.5  0.5 days ago         0.5 days ago
     0  today                in 0 days
     0  today                in 0 days
    -0  today                0 days ago
   0.5  in 0.5 days          in 0.5 days
     1  tomorrow             in 1 day
   1.5  in 1.5 days          in 1.5 days
  1.75  in 1.75 days         in 1.75 days
     2  in 2 days            in 2 days
     3  in 3 days            in 3 days

=== hour =======================================

offset  format               formatNumeric
------  -------------------  -------------------
    -3  3 hours ago          3 hours ago
    -2  2 hours ago          2 hours ago
 -1.75  1.75 hours ago       1.75 hours ago
  -1.5  1.5 hours ago        1.5 hours ago
    -1  1 hour ago           1 hour ago
  -0.5  0.5 hours ago        0.5 hours ago
     0  this hour            in 0 hours
     0  this hour            in 0 hours
    -0  this hour            0 hours ago
   0.5  in 0.5 hours         in 0.5 hours
     1  in 1 hour            in 1 hour
   1.5  in 1.5 hours         in 1.5 hours
  1.75  in 1.75 hours        in 1.75 hours
     2  in 2 hours           in 2 hours
     3  in 3 hours           in 3 hours

=== minute =====================================

offset  format               formatNumeric
------  -------------------  -------------------
    -3  3 minutes ago        3 minutes ago
    -2  2 minutes ago        2 minutes ago
 -1.75  1.75 minutes ago     1.75 minutes ago
  -1.5  1.5 minutes ago      1.5 minutes ago
    -1  1 minute ago         1 minute ago
  -0.5  0.5 minutes ago      0.5 minutes ago
     0  this minute          in 0 minutes
     0  this minute          in 0 minutes
    -0  this minute          0 minutes ago
   0.5  in 0.5 minutes       in 0.5 minutes
     1  in 1 minute          in 1 minute
   1.5  in 1.5 minutes       in 1.5 minutes
  1.75  in 1.75 minutes      in 1.75 minutes
     2  in 2 minutes         in 2 minutes
     3  in 3 minutes         in 3 minutes

=== second =====================================

offset  format               formatNumeric
------  -------------------  -------------------
    -3  3 seconds ago        3 seconds ago
    -2  2 seconds ago        2 seconds ago
 -1.75  1.75 seconds ago     1.75 seconds ago
  -1.5  1.5 seconds ago      1.5 seconds ago
    -1  1 second ago         1 second ago
  -0.5  0.5 seconds ago      0.5 seconds ago
     0  now                  in 0 seconds
     0  now                  in 0 seconds
    -0  now                  0 seconds ago
   0.5  in 0.5 seconds       in 0.5 seconds
     1  in 1 second          in 1 second
   1.5  in 1.5 seconds       in 1.5 seconds
  1.75  in 1.75 seconds      in 1.75 seconds
     2  in 2 seconds         in 2 seconds
     3  in 3 seconds         in 3 seconds

=== sunday =====================================

offset  format               formatNumeric
------  -------------------  -------------------
    -3  3 Sundays ago        3 Sundays ago
    -2  2 Sundays ago        2 Sundays ago
 -1.75  1.75 Sundays ago     1.75 Sundays ago
  -1.5  1.5 Sundays ago      1.5 Sundays ago
    -1  last Sunday          1 Sunday ago
  -0.5  0.5 Sundays ago      0.5 Sundays ago
     0  this Sunday          in 0 Sundays
     0  this Sunday          in 0 Sundays
    -0  this Sunday          0 Sundays ago
   0.5  in 0.5 Sundays       in 0.5 Sundays
     1  next Sunday          in 1 Sunday
   1.5  in 1.5 Sundays       in 1.5 Sundays
  1.75  in 1.75 Sundays      in 1.75 Sundays
     2  in 2 Sundays         in 2 Sundays
     3  in 3 Sundays         in 3 Sundays

=== monday =====================================

offset  format               formatNumeric
------  -------------------  -------------------
    -3  3 Mondays ago        3 Mondays ago
    -2  2 Mondays ago        2 Mondays ago
 -1.75  1.75 Mondays ago     1.75 Mondays ago
  -1.5  1.5 Mondays ago      1.5 Mondays ago
    -1  last Monday          1 Monday ago
  -0.5  0.5 Mondays ago      0.5 Mondays ago
     0  this Monday          in 0 Mondays
     0  this Monday          in 0 Mondays
    -0  this Monday          0 Mondays ago
   0.5  in 0.5 Mondays       in 0.5 Mondays
     1  next Monday          in 1 Monday
   1.5  in 1.5 Mondays       in 1.5 Mondays
  1.75  in 1.75 Mondays      in 1.75 Mondays
     2  in 2 Mondays         in 2 Mondays
     3  in 3 Mondays         in 3 Mondays

=== tuesday ====================================

offset  format               formatNumeric
------  -------------------  -------------------
    -3  3 Tuesdays ago       3 Tuesdays ago
    -2  2 Tuesdays ago       2 Tuesdays ago
 -1.75  1.75 Tuesdays ago    1.75 Tuesdays ago
  -1.5  1.5 Tuesdays ago     1.5 Tuesdays ago
    -1  last Tuesday         1 Tuesday ago
  -0.5  0.5 Tuesdays ago     0.5 Tuesdays ago
     0  this Tuesday         in 0 Tuesdays
     0  this Tuesday         in 0 Tuesdays
    -0  this Tuesday         0 Tuesdays ago
   0.5  in 0.5 Tuesdays      in 0.5 Tuesdays
     1  next Tuesday         in 1 Tuesday
   1.5  in 1.5 Tuesdays      in 1.5 Tuesdays
  1.75  in 1.75 Tuesdays     in 1.75 Tuesdays
     2  in 2 Tuesdays        in 2 Tuesdays
     3  in 3 Tuesdays        in 3 Tuesdays

=== wednesday ==================================

offset  format               formatNumeric
------  -------------------  -------------------
    -3  3 Wednesdays ago     3 Wednesdays ago
    -2  2 Wednesdays ago     2 Wednesdays ago
 -1.75  1.75 Wednesdays ago  1.75 Wednesdays ago
  -1.5  1.5 Wednesdays ago   1.5 Wednesdays ago
    -1  last Wednesday       1 Wednesday ago
  -0.5  0.5 Wednesdays ago   0.5 Wednesdays ago
     0  this Wednesday       in 0 Wednesdays
     0  this Wednesday       in 0 Wednesdays
    -0  this Wednesday       0 Wednesdays ago
   0.5  in 0.5 Wednesdays    in 0.5 Wednesdays
     1  next Wednesday       in 1 Wednesday
   1.5  in 1.5 Wednesdays    in 1.5 Wednesdays
  1.75  in 1.75 Wednesdays   in 1.75 Wednesdays
     2  in 2 Wednesdays      in 2 Wednesdays
     3  in 3 Wednesdays      in 3 Wednesdays

=== thursday ===================================

offset  format               formatNumeric
------  -------------------  -------------------
    -3  3 Thursdays ago      3 Thursdays ago
    -2  2 Thursdays ago      2 Thursdays ago
 -1.75  1.75 Thursdays ago   1.75 Thursdays ago
  -1.5  1.5 Thursdays ago    1.5 Thursdays ago
    -1  last Thursday        1 Thursday ago
  -0.5  0.5 Thursdays ago    0.5 Thursdays ago
     0  this Thursday        in 0 Thursdays
     0  this Thursday        in 0 Thursdays
    -0  this Thursday        0 Thursdays ago
   0.5  in 0.5 Thursdays     in 0.5 Thursdays
     1  next Thursday        in 1 Thursday
   1.5  in 1.5 Thursdays     in 1.5 Thursdays
  1.75  in 1.75 Thursdays    in 1.75 Thursdays
     2  in 2 Thursdays       in 2 Thursdays
     3  in 3 Thursdays       in 3 Thursdays

=== friday =====================================

offset  format               formatNumeric
------  -------------------  -------------------
    -3  3 Fridays ago        3 Fridays ago
    -2  2 Fridays ago        2 Fridays ago
 -1.75  1.75 Fridays ago     1.75 Fridays ago
  -1.5  1.5 Fridays ago      1.5 Fridays ago
    -1  last Friday          1 Friday ago
  -0.5  0.5 Fridays ago      0.5 Fridays ago
     0  this Friday          in 0 Fridays
     0  this Friday          in 0 Fridays
    -0  this Friday          0 Fridays ago
   0.5  in 0.5 Fridays       in 0.5 Fridays
     1  next Friday          in 1 Friday
   1.5  in 1.5 Fridays       in 1.5 Fridays
  1.75  in 1.75 Fridays      in 1.75 Fridays
     2  in 2 Fridays         in 2 Fridays
     3  in 3 Fridays         in 3 Fridays

=== saturday ===================================

offset  format               formatNumeric
------  -------------------  -------------------
    -3  3 Saturdays ago      3 Saturdays ago
    -2  2 Saturdays ago      2 Saturdays ago
 -1.75  1.75 Saturdays ago   1.75 Saturdays ago
  -1.5  1.5 Saturdays ago    1.5 Saturdays ago
    -1  last Saturday        1 Saturday ago
  -0.5  0.5 Saturdays ago    0.5 Saturdays ago
     0  this Saturday        in 0 Saturdays
     0  this Saturday        in 0 Saturdays
    -0  this Saturday        0 Saturdays ago
   0.5  in 0.5 Saturdays     in 0.5 Saturdays
     1  next Saturday        in 1 Saturday
   1.5  in 1.5 Saturdays     in 1.5 Saturdays
  1.75  in 1.75 Saturdays    in 1.75 Saturdays
     2  in 2 Saturdays       in 2 Saturdays
     3  in 3 Saturdays       in 3 Saturdays
