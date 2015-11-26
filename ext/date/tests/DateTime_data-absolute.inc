<?php

/*
 * Note: test names match method names in a set of PHPUnit tests
 * in a userland package.  Please be so kind as to leave them.
 */

date_default_timezone_set('America/New_York');


/*
 * Absolute
 */
echo "test_absolute_7: ";
examine_diff('2009-01-14', '2009-01-07', 'P+0Y0M7DT0H0M0S', 7, true);

echo "test_absolute_negative_7: ";
examine_diff('2009-01-07', '2009-01-14', 'P+0Y0M7DT0H0M0S', 7, true);

//14 - 7 = 7
//7 + 7 = 14
//
//7 - 14 = -7
//14 - 7 = 7
