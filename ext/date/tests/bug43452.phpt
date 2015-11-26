--TEST--
Bug #43452 ("weekday" is not equivalent to "1 weekday" of the current weekday is "weekday")
--FILE--
<?php
date_default_timezone_set('Europe/Oslo');
// <day> is equivalent to 1 <day> and will *not* forward if the current day
// (November 1st) is the same day of week.
$day = strtotime( "Thursday Nov 2007" );
echo date( DateTime::ISO8601, $day ), "\n";
$day = strtotime( "1 Thursday Nov 2007" );
echo date( DateTime::ISO8601, $day ), "\n";
$day = strtotime( "2 Thursday Nov 2007" );
echo date( DateTime::ISO8601, $day ), "\n";
$day = strtotime( "3 Thursday Nov 2007" );
echo date( DateTime::ISO8601, $day ), "\n\n";

// forward one week, then behaves like above for week days
$day = strtotime( "Thursday Nov 2007" );
echo date( DateTime::ISO8601, $day ), "\n";
$day = strtotime( "+1 week Thursday Nov 2007" );
echo date( DateTime::ISO8601, $day ), "\n";
$day = strtotime( "+2 week Thursday Nov 2007" );
echo date( DateTime::ISO8601, $day ), "\n";
$day = strtotime( "+3 week Thursday Nov 2007" );
echo date( DateTime::ISO8601, $day ), "\n\n";

// First, second, etc skip to the first/second weekday *after* the current day.
// This makes "first thursday" equivalent to "+1 week thursday" - but only
// if the current day-of-week is the one mentioned in the phrase.
$day = strtotime( "Thursday Nov 2007" );
echo date( DateTime::ISO8601, $day ), "\n";
$day = strtotime( "first Thursday Nov 2007" );
echo date( DateTime::ISO8601, $day ), "\n";
$day = strtotime( "second Thursday Nov 2007" );
echo date( DateTime::ISO8601, $day ), "\n";
$day = strtotime( "third Thursday Nov 2007" );
echo date( DateTime::ISO8601, $day ), "\n\n";

// Now the same where the current day-of-week does not match the one in the
// phrase.
$day = strtotime( "Friday Nov 2007" );
echo date( DateTime::ISO8601, $day ), "\n";
$day = strtotime( "first Friday Nov 2007" );
echo date( DateTime::ISO8601, $day ), "\n";
$day = strtotime( "second Friday Nov 2007" );
echo date( DateTime::ISO8601, $day ), "\n";
$day = strtotime( "third Friday Nov 2007" );
echo date( DateTime::ISO8601, $day ), "\n\n";

?>
--EXPECT--
2007-11-01T00:00:00+0100
2007-11-01T00:00:00+0100
2007-11-08T00:00:00+0100
2007-11-15T00:00:00+0100

2007-11-01T00:00:00+0100
2007-11-08T00:00:00+0100
2007-11-15T00:00:00+0100
2007-11-22T00:00:00+0100

2007-11-01T00:00:00+0100
2007-11-08T00:00:00+0100
2007-11-15T00:00:00+0100
2007-11-22T00:00:00+0100

2007-11-02T00:00:00+0100
2007-11-02T00:00:00+0100
2007-11-09T00:00:00+0100
2007-11-16T00:00:00+0100
