--TEST--
DateTime::add() -- massive
--CREDITS--
Daniel Convissor <danielc@php.net>
--FILE--
<?php

require 'examine_diff.inc';
define('PHPT_DATETIME_SHOW', PHPT_DATETIME_SHOW_ADD);
require 'DateTime_data-massive.inc';

?>
--EXPECTF--
test_massive_positive: ADD: -333333-01-01 16:18:02 %s + P+666666Y0M0DT0H0M0S = **333333-01-01 16:18:02 %s**
test_massive_negative: ADD: 333333-01-01 16:18:02 %s + P-666666Y0M0DT0H0M0S = **-333333-01-01 16:18:02 %s**
