--TEST--
IntlDateFormatter::formatObject(): returns wrong value when time style is NONE.
--EXTENSIONS--
intl
--SKIPIF--
<?php
if (version_compare(INTL_ICU_VERSION, '50.1.2') < 0) die('skip for ICU < 51.1.2');
if (version_compare(INTL_ICU_VERSION, '74.1') >= 0) die('skip for ICU >= 74.1');
?>
--FILE--
<?php
$millitimestamp = 1428133423941.0; // 14:43:43 April 04 2015
$date = IntlCalendar::createInstance('Asia/Ho_Chi_Minh');
$date->setTime($millitimestamp);
echo IntlDateFormatter::formatObject($date, array(IntlDateFormatter::SHORT, IntlDateFormatter::NONE), 'vi_VN'), "\n";
echo IntlDateFormatter::formatObject ($date, array(IntlDateFormatter::SHORT, IntlDateFormatter::NONE), 'ko_KR'), "\n";
?>
--EXPECT--
04/04/2015
15. 4. 4.
