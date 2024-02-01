<?php

/** @generate-class-entries */

/**
 * @var int
 * @cvalue CAL_GREGORIAN
 */
const CAL_GREGORIAN = UNKNOWN;
/**
 * @var int
 * @cvalue CAL_JULIAN
 */
const CAL_JULIAN = UNKNOWN;
/**
 * @var int
 * @cvalue CAL_JEWISH
 */
const CAL_JEWISH = UNKNOWN;
/**
 * @var int
 * @cvalue CAL_FRENCH
 */
const CAL_FRENCH = UNKNOWN;
/**
 * @var int
 * @cvalue CAL_NUM_CALS
 */
const CAL_NUM_CALS = UNKNOWN;

/**
 * @var int
 * @cvalue CAL_DOW_DAYNO
 */
const CAL_DOW_DAYNO = UNKNOWN;
/**
 * @var int
 * @cvalue CAL_DOW_SHORT
 */
const CAL_DOW_SHORT = UNKNOWN;
/**
 * @var int
 * @cvalue CAL_DOW_LONG
 */
const CAL_DOW_LONG = UNKNOWN;

/**
 * @var int
 * @cvalue CAL_MONTH_GREGORIAN_SHORT
 */
const CAL_MONTH_GREGORIAN_SHORT = UNKNOWN;
/**
 * @var int
 * @cvalue CAL_MONTH_GREGORIAN_LONG
 */
const CAL_MONTH_GREGORIAN_LONG = UNKNOWN;
/**
 * @var int
 * @cvalue CAL_MONTH_JULIAN_SHORT
 */
const CAL_MONTH_JULIAN_SHORT = UNKNOWN;
/**
 * @var int
 * @cvalue CAL_MONTH_JULIAN_LONG
 */
const CAL_MONTH_JULIAN_LONG = UNKNOWN;
/**
 * @var int
 * @cvalue CAL_MONTH_JEWISH
 */
const CAL_MONTH_JEWISH = UNKNOWN;
/**
 * @var int
 * @cvalue CAL_MONTH_FRENCH
 */
const CAL_MONTH_FRENCH = UNKNOWN;

/**
 * @var int
 * @cvalue CAL_EASTER_DEFAULT
 */
const CAL_EASTER_DEFAULT = UNKNOWN;
/**
 * @var int
 * @cvalue CAL_EASTER_ROMAN
 */
const CAL_EASTER_ROMAN = UNKNOWN;
/**
 * @var int
 * @cvalue CAL_EASTER_ALWAYS_GREGORIAN
 */
const CAL_EASTER_ALWAYS_GREGORIAN = UNKNOWN;
/**
 * @var int
 * @cvalue CAL_EASTER_ALWAYS_JULIAN
 */
const CAL_EASTER_ALWAYS_JULIAN = UNKNOWN;

/**
 * @var int
 * @cvalue CAL_JEWISH_ADD_ALAFIM_GERESH
 */
const CAL_JEWISH_ADD_ALAFIM_GERESH = UNKNOWN;
/**
 * @var int
 * @cvalue CAL_JEWISH_ADD_ALAFIM
 */
const CAL_JEWISH_ADD_ALAFIM = UNKNOWN;
/**
 * @var int
 * @cvalue CAL_JEWISH_ADD_GERESHAYIM
 */
const CAL_JEWISH_ADD_GERESHAYIM = UNKNOWN;

function cal_days_in_month(int $calendar, int $month, int $year): int {}

/**
 * @return array<string, int|string|null>
 * @refcount 1
 */
function cal_from_jd(int $julian_day, int $calendar): array {}

/**
 * @return array<int|string, int|string|array>
 * @refcount 1
 */
function cal_info(int $calendar = -1): array {}

function cal_to_jd(int $calendar, int $month, int $day, int $year): int {}

function easter_date(?int $year = null, int $mode = CAL_EASTER_DEFAULT): int {}

function easter_days(?int $year = null, int $mode = CAL_EASTER_DEFAULT): int {}

function frenchtojd(int $month, int $day, int $year): int {}

function gregoriantojd(int $month, int $day, int $year): int {}

function jddayofweek(int $julian_day, int $mode = CAL_DOW_DAYNO): int|string {}

function jdmonthname(int $julian_day, int $mode): string {}

function jdtofrench(int $julian_day): string {}

function jdtogregorian(int $julian_day): string {}

function jdtojewish(int $julian_day, bool $hebrew = false, int $flags = 0): string {}

function jdtojulian(int $julian_day): string {}

function jdtounix(int $julian_day): int {}

function jewishtojd(int $month, int $day, int $year): int {}

function juliantojd(int $month, int $day, int $year): int {}

function unixtojd(?int $timestamp = null): int|false {}
