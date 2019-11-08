<?php

function cal_days_in_month(int $calendar, int $month, int $year): int {}

function cal_from_jd(int $jd, int $calendar): array {}

function cal_info(?int $calendar = UNKNOWN): array {}

function cal_to_jd(int $calendar, int $month, int $day, int $year): int {}

function easter_date(int $year = UNKNOWN, int $method = CAL_EASTER_DEFAULT): int {}

function easter_days(int $year = UNKNOWN, int $method = CAL_EASTER_DEFAULT): int {}

function frenchtojd(int $month, int $day, int $year): int {}

function gregoriantojd(int $month, int $day, int $year): int {}

function jddayofweek(int $juliandaycount, int $mode = CAL_DOW_DAYNO): int|string {}

function jdmonthname(int $juliandaycount, int $mode): string {}

function jdtofrench(int $juliandaycount): string {}

function jdtogregorian(int $juliandaycount): string {}

function jdtojewish(int $juliandaycount, bool $hebrew = false, int $fl = 0): string {}

function jdtojulian(int $juliandaycount): string {}

function jdtounix(int $jday): int {}

function jewishtojd(int $month, int $day, int $year): int {}

function juliantojd(int $month, int $day, int $year): int {}

function unixtojd(int $timestamp = UNKNOWN): int|false {}
