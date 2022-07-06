<?php

/*
 * Note: test names match method names in a set of PHPUnit tests
 * in a userland package.  Please be so kind as to leave them.
 */

date_default_timezone_set('America/New_York');


/*
 * Massive dates
 */
echo "test_massive_positive: ";
$end = new DateTime;
$end->setDate(333333, 1, 1);
$end->setTime(16, 18, 02);

$start = new DateTime;
$start->setDate(-333333, 1, 1);
$start->setTime(16, 18, 02);

examine_diff($end, $start, 'P+666666Y0M0DT0H0M0S', 243494757);

echo "test_massive_negative: ";
$end = new DateTime;
$end->setDate(-333333, 1, 1);
$end->setTime(16, 18, 02);

$start = new DateTime;
$start->setDate(333333, 1, 1);
$start->setTime(16, 18, 02);

examine_diff($end, $start, 'P-666666Y0M0DT0H0M0S', 243494757);
