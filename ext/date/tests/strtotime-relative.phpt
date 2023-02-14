--TEST--
strtotime() with relative offsets
--FILE--
<?php

date_default_timezone_set('UTC');

$base_time = 1204200000; // 28 Feb 2008 12:00:00

$offsets = array(
    // offset around a day
    '80412 seconds',
    '86399 seconds',
    '86400 seconds',
    '86401 seconds',
    '112913 seconds',

    // offset around 7 days
    '134 hours',
    '167 hours',
    '168 hours',
    '169 hours',
    '183 hours',

    // offset around 6 months
    '178 days',
    '179 days',
    '180 days',
    '183 days',
    '184 days',

    // offset around 10 years
    '115 months',
    '119 months',
    '120 months',
    '121 months',
    '128 months',

    // offset around 25 years (can't do much more reliably with strtotime)
    '24 years',
    '25 years',
    '26 years'
);

foreach ($offsets AS $offset) {
    foreach (array('+', '-') AS $direction) {
        echo "$direction$offset: " . date(DATE_ATOM, strtotime("$direction$offset", $base_time)) . "\n";
    }
}

?>
--EXPECT--
+80412 seconds: 2008-02-29T10:20:12+00:00
-80412 seconds: 2008-02-27T13:39:48+00:00
+86399 seconds: 2008-02-29T11:59:59+00:00
-86399 seconds: 2008-02-27T12:00:01+00:00
+86400 seconds: 2008-02-29T12:00:00+00:00
-86400 seconds: 2008-02-27T12:00:00+00:00
+86401 seconds: 2008-02-29T12:00:01+00:00
-86401 seconds: 2008-02-27T11:59:59+00:00
+112913 seconds: 2008-02-29T19:21:53+00:00
-112913 seconds: 2008-02-27T04:38:07+00:00
+134 hours: 2008-03-05T02:00:00+00:00
-134 hours: 2008-02-22T22:00:00+00:00
+167 hours: 2008-03-06T11:00:00+00:00
-167 hours: 2008-02-21T13:00:00+00:00
+168 hours: 2008-03-06T12:00:00+00:00
-168 hours: 2008-02-21T12:00:00+00:00
+169 hours: 2008-03-06T13:00:00+00:00
-169 hours: 2008-02-21T11:00:00+00:00
+183 hours: 2008-03-07T03:00:00+00:00
-183 hours: 2008-02-20T21:00:00+00:00
+178 days: 2008-08-24T12:00:00+00:00
-178 days: 2007-09-03T12:00:00+00:00
+179 days: 2008-08-25T12:00:00+00:00
-179 days: 2007-09-02T12:00:00+00:00
+180 days: 2008-08-26T12:00:00+00:00
-180 days: 2007-09-01T12:00:00+00:00
+183 days: 2008-08-29T12:00:00+00:00
-183 days: 2007-08-29T12:00:00+00:00
+184 days: 2008-08-30T12:00:00+00:00
-184 days: 2007-08-28T12:00:00+00:00
+115 months: 2017-09-28T12:00:00+00:00
-115 months: 1998-07-28T12:00:00+00:00
+119 months: 2018-01-28T12:00:00+00:00
-119 months: 1998-03-28T12:00:00+00:00
+120 months: 2018-02-28T12:00:00+00:00
-120 months: 1998-02-28T12:00:00+00:00
+121 months: 2018-03-28T12:00:00+00:00
-121 months: 1998-01-28T12:00:00+00:00
+128 months: 2018-10-28T12:00:00+00:00
-128 months: 1997-06-28T12:00:00+00:00
+24 years: 2032-02-28T12:00:00+00:00
-24 years: 1984-02-28T12:00:00+00:00
+25 years: 2033-02-28T12:00:00+00:00
-25 years: 1983-02-28T12:00:00+00:00
+26 years: 2034-02-28T12:00:00+00:00
-26 years: 1982-02-28T12:00:00+00:00
