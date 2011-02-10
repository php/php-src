<?php

/**
 * Helper for the DateTime_diff_add_sub* tests
 *
 * @author Daniel Convissor <danielc@analysisandsolutions.com>
 */

/**
 * Provides a consistent interface for executing date diff tests
 *
 * Tests the diff() method then passes the resulting
 * interval to the add()/sub() method as a double check
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

	$force_sub = false;
	if ($absolute) {
		$tmp_interval = $start->diff($end);
		if ($tmp_interval->format('%r')) {
			$force_sub = true;
		}
	}

	$result_interval = $start->diff($end, $absolute);
	$result_spec = $result_interval->format('P%R%yY%mM%dDT%hH%iM%sS');
	$result_days = $result_interval->format('%a');

	// Also make sure add()/sub() works the same way as diff().
	if ($force_sub) {
		$start->sub($result_interval);
		$sign = '-';
	} else {
		$start->add($result_interval);
		$sign = '+';
	}

	$result_end_date = $start->format('Y-m-d H:i:s T');

	// Leaving this here for making adjustments later.
	$expect_full = "FWD: $end_date - $start_date = **$expect_spec** | "
		. "BACK: $start_date $sign $expect_spec = **$end_date** | "
		. "DAYS: **$expect_days**";
	// echo "$expect_full\n";
	// return;

	$result_full = "FWD: $end_date - $start_date = **$result_spec** | "
		. "BACK: $start_date $sign $result_spec = **$result_end_date** | "
		. "DAYS: **$result_days**";
	echo "$result_full\n";
}
