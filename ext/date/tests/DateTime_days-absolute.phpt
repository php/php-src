--TEST--
DateTime::diff() days -- absolute 
--CREDITS--
Daniel Convissor <danielc@php.net>
--FILE--
<?php

require 'examine_diff.inc';
define('PHPT_DATETIME_SHOW', PHPT_DATETIME_SHOW_DAYS);
require 'DateTime_data-absolute.inc';

?>
--EXPECT--
test_absolute_7: DAYS: **7**
test_absolute_negative_7: DAYS: **7**
