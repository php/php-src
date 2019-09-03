<?php

/** @return int|false */
function cal_days_in_month(int $calendar, int $month, int $year) {}

/** @return array<string, array<int, string>&int&string>|false */
function cal_from_jd(int $jd, int $calendar) {}

/** @return array|false */
function cal_info(?int $calendar = UNKNOWN) {}

/** @return int|false */
function cal_to_jd(int $calendar, int $month, int $day, int $year) {}

/** @return int|false */
function easter_date(int $year = UNKNOWN, int $method = CAL_EASTER_DEFAULT) {}

function easter_days(int $year = UNKNOWN, int $method = CAL_EASTER_DEFAULT): int {}

function frenchtojd(int $month, int $day, int $year): int {}

function gregoriantojd(int $month, int $day, int $year): int {}

/** @return int|string */
function jddayofweek(int $juliandaycount, int $mode = CAL_DOW_DAYNO) {}

function jdmonthname(int $juliandaycount, int $mode): string {}

function jdtofrench(int $juliandaycount): string {}

function jdtogregorian(int $juliandaycount): string {}

/** @return string|false */
function jdtojewish(int $juliandaycount, bool $hebrew = false, int $fl = 0) {}

function jdtojulian(int $juliandaycount): string {}

/** @return int|false */
function jdtounix(int $jday) {}

function jewishtojd(int $month, int $day, int $year): int {}

function juliantojd(int $month, int $day, int $year): int {}

/** @return int|false */
function unixtojd(int $timestamp = UNKNOWN) {}
