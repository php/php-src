<?php

/** @generate-function-entries */

function cal_days_in_month(int $calendar, int $month, int $year): int {}

function cal_from_jd(int $julian_day, int $calendar): array {}

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
