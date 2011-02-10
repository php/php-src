--TEST--
DateTime::diff() add() sub() -- absolute 
--CREDITS--
Daniel Convissor <danielc@php.net>
--FILE--
<?php

/*
 * Note: test names match method names in a set of PHPUnit tests
 * in a userland package.  Please be so kind as to leave them.
 */

require './examine_diff.inc';
date_default_timezone_set('America/New_York');


/*
 * Absolute
 */
echo "test_absolute_7: ";
examine_diff('2009-01-14', '2009-01-07', 'P+0Y0M7DT0H0M0S', 7, true);

echo "test_absolute_negative_7: ";
examine_diff('2009-01-07', '2009-01-14', 'P+0Y0M7DT0H0M0S', 7, true);

?>
--EXPECT--
test_absolute_7: FWD: 2009-01-14 00:00:00 EST - 2009-01-07 00:00:00 EST = **P+0Y0M7DT0H0M0S** | BACK: 2009-01-07 00:00:00 EST + P+0Y0M7DT0H0M0S = **2009-01-14 00:00:00 EST** | DAYS: **7**
test_absolute_negative_7: FWD: 2009-01-07 00:00:00 EST - 2009-01-14 00:00:00 EST = **P+0Y0M7DT0H0M0S** | BACK: 2009-01-14 00:00:00 EST - P+0Y0M7DT0H0M0S = **2009-01-07 00:00:00 EST** | DAYS: **7**
