--TEST--
Bug #80763 (msgfmt_format() does not accept DateTime references)
--SKIPIF--
<?php
if (!extension_loaded('intl')) die('skip intl extension not available');
?>
--FILE--
<?php
$today = new DateTime('2021-02-17 12:00:00');
$formatter = new \MessageFormatter('en_US', 'Today is {today, date, full}.');
$params = ['today' => $today];
array_walk($params, fn() => 1);
var_dump($formatter->format($params));
?>
--EXPECT--
string(38) "Today is Wednesday, February 17, 2021."
