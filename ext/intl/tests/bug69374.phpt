--TEST--
IntlDateFormatter::formatObject(): returns wrong utf8 value when $format param is utf8 string pattern.
--SKIPIF--
<?php
if (!extension_loaded('intl')) die('skip intl extension not enabled'); ?>
<?php if (version_compare(INTL_ICU_VERSION, '50.1.2') < 0) die('skip for ICU >= 51.1.2'); ?>
--FILE--
<?php
$millitimestamp = 1428133423941.0; // 14:43:43 April 04 2015
$pattern1 = '\'tháng\' MM, y'; // yMM format for Vietnamese
$pattern2 = 'y년 MMM'; // yMM format for Korean
$date = IntlCalendar::createInstance('Asia/Ho_Chi_Minh');
$date->setTime($millitimestamp);
echo IntlDateFormatter::formatObject($date, $pattern1, 'vi_VN'), "\n";
echo IntlDateFormatter::formatObject ($date, $pattern2, 'ko_KR'), "\n";
?>
--EXPECT--
tháng 04, 2015
2015년 4월
