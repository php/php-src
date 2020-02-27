<?php

/**
 * Helper for the DateTime diff/days/math tests
 *
 * @author Daniel Convissor <danielc@analysisandsolutions.com>
 */

/**#@+
 * Which test segments should be displayed?
 */
define('PHPT_DATETIME_SHOW_DIFF', 1);
define('PHPT_DATETIME_SHOW_DAYS', 2);
define('PHPT_DATETIME_SHOW_ADD', 3);
define('PHPT_DATETIME_SHOW_SUB', 4);
/**#@-*/

/**
 * Provides a consistent interface for executing date diff/add/sub tests
 *
 * @param string|DateTime $end_date  the end date in YYYY-MM-DD format
 *                        (can include time HH:MM:SS) or a DateTime object
 * @param string|DateTime $start_date  the start date in YYYY-MM-DD format
 *                        (can include time HH:MM:SS) or a DateTime object
 * @param string $expect_spec  the expected result of the tests, in the
 *               special interval specification used for this test suite.
 *               This spec includes a "+" or "-" after the "P" in order to
 *               indicate which direction to go.
 * @param int $expect_days  the number of days to compare with the
 *            interval's "days" property
 * @param bool $absolute  should the result always be a positive number?
 *
 * @return void
 */
function examine_diff($end_date, $start_date, $expect_spec, $expect_days, $absolute = false) {
    if (is_string($start_date)) {
        $start = new DateTime($start_date);
    } else {
        $start = $start_date;
    }
    $start_date = $start->format('Y-m-d H:i:s T');

    if (is_string($end_date)) {
        $end = new DateTime($end_date);
    } else {
        $end = $end_date;
    }
    $end_date = $end->format('Y-m-d H:i:s T');

    $expect_interval = new DateInterval('P' . substr($expect_spec, 2));
    if (substr($expect_spec, 1, 1) == '-') {
        $expect_interval->invert = true;
    }

    if (PHPT_DATETIME_SHOW == PHPT_DATETIME_SHOW_DIFF) {
        $result_interval = $start->diff($end, $absolute);
        $result_spec = $result_interval->format('P%R%yY%mM%dDT%hH%iM%sS');
        echo "DIFF: $end_date - $start_date = **$result_spec**\n";
        // echo "DIFF: $end_date - $start_date = **$expect_spec**\n";
    }
    if (PHPT_DATETIME_SHOW == PHPT_DATETIME_SHOW_DAYS) {
        $result_interval = $start->diff($end, $absolute);
        $result_days = $result_interval->format('%a');
        echo "DAYS: **$result_days**\n";
        // echo "DAYS: **$expect_days**\n";
    }
    if (PHPT_DATETIME_SHOW == PHPT_DATETIME_SHOW_ADD) {
        $start->add($expect_interval);
        $result_end_date = $start->format('Y-m-d H:i:s T');
        echo "ADD: $start_date + $expect_spec = **$result_end_date**\n";
        // echo "ADD: $start_date + $expect_spec = **$end_date**\n";
    }
    if (PHPT_DATETIME_SHOW == PHPT_DATETIME_SHOW_SUB) {
        $end->sub($expect_interval);
        $result_start_date = $end->format('Y-m-d H:i:s T');
        echo "SUB: $end_date - $expect_spec = **$result_start_date**\n";
        // echo "SUB: $end_date - $expect_spec = **$start_date**\n";
    }
}
