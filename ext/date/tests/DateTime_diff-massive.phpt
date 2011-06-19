--TEST--
DateTime::diff() -- massive
--CREDITS--
Daniel Convissor <danielc@php.net>
--FILE--
<?php

require 'examine_diff.inc';
define('PHPT_DATETIME_SHOW', PHPT_DATETIME_SHOW_DIFF);
require 'DateTime_data-massive.inc';

?>
--EXPECT--
test_massive_positive: DIFF: 333333-01-01 16:18:02 EST - -333333-01-01 16:18:02 EST = **P+666666Y0M0DT0H0M0S**
test_massive_negative: DIFF: -333333-01-01 16:18:02 EST - 333333-01-01 16:18:02 EST = **P-666666Y0M0DT0H0M0S**
