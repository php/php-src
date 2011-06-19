--TEST--
DateTime::diff() -- absolute 
--CREDITS--
Daniel Convissor <danielc@php.net>
--FILE--
<?php

require 'examine_diff.inc';
define('PHPT_DATETIME_SHOW', PHPT_DATETIME_SHOW_DIFF);
require 'DateTime_data-absolute.inc';

?>
--EXPECT--
test_absolute_7: DIFF: 2009-01-14 00:00:00 EST - 2009-01-07 00:00:00 EST = **P+0Y0M7DT0H0M0S**
test_absolute_negative_7: DIFF: 2009-01-07 00:00:00 EST - 2009-01-14 00:00:00 EST = **P+0Y0M7DT0H0M0S**
