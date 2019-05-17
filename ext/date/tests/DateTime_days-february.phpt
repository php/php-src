--TEST--
DateTime::diff() days -- february
--CREDITS--
Daniel Convissor <danielc@php.net>
--FILE--
<?php

require 'examine_diff.inc';
define('PHPT_DATETIME_SHOW', PHPT_DATETIME_SHOW_DAYS);
require 'DateTime_data-february.inc';

?>
--EXPECT--
test_bug_49081__1: DAYS: **30**
test_bug_49081__2: DAYS: **31**
test_bug_49081__3: DAYS: **1**
test_bug_49081__4: DAYS: **29**
test_bug_49081__5: DAYS: **30**
test_bug_49081__6: DAYS: **31**
test_bug_49081__7: DAYS: **32**
test_bug_49081__8: DAYS: **28**
test_bug_49081__9: DAYS: **29**
test_bug_49081__10: DAYS: **30**
test_bug_49081__11: DAYS: **31**
test_bug_49081__12: DAYS: **1**
test_bug_49081__13: DAYS: **27**
test_bug_49081__14: DAYS: **28**
test_bug_49081__15: DAYS: **29**
test_bug_49081__16: DAYS: **30**
test_bug_49081__17: DAYS: **31**
test_bug_49081__18: DAYS: **32**
test_bug_49081__19: DAYS: **59**
test_bug_49081__20: DAYS: **29**
test_bug_49081__21: DAYS: **55**
test_bug_49081__22: DAYS: **56**
test_bug_49081__23: DAYS: **57**
test_bug_49081__24: DAYS: **58**
test_bug_49081__25: DAYS: **59**
test_bug_49081__26: DAYS: **60**
test_bug_49081__27: DAYS: **30**
test_bug_49081__28: DAYS: **27**
test_bug_49081__29: DAYS: **28**
test_bug_49081__30: DAYS: **29**
test_bug_49081__31: DAYS: **28**
test_bug_49081__32: DAYS: **29**
test_bug_49081_negative__1: DAYS: **30**
test_bug_49081_negative__2: DAYS: **31**
test_bug_49081_negative__3: DAYS: **1**
test_bug_49081_negative__4: DAYS: **29**
test_bug_49081_negative__5: DAYS: **30**
test_bug_49081_negative__6: DAYS: **31**
test_bug_49081_negative__7: DAYS: **32**
test_bug_49081_negative__8: DAYS: **28**
test_bug_49081_negative__9: DAYS: **29**
test_bug_49081_negative__10: DAYS: **30**
test_bug_49081_negative__11: DAYS: **31**
test_bug_49081_negative__12: DAYS: **1**
test_bug_49081_negative__13: DAYS: **27**
test_bug_49081_negative__14: DAYS: **28**
test_bug_49081_negative__15: DAYS: **29**
test_bug_49081_negative__16: DAYS: **30**
test_bug_49081_negative__17: DAYS: **31**
test_bug_49081_negative__18: DAYS: **32**
test_bug_49081_negative__19: DAYS: **59**
test_bug_49081_negative__20: DAYS: **29**
test_bug_49081_negative__21: DAYS: **55**
test_bug_49081_negative__22: DAYS: **56**
test_bug_49081_negative__23: DAYS: **57**
test_bug_49081_negative__24: DAYS: **58**
test_bug_49081_negative__25: DAYS: **59**
test_bug_49081_negative__26: DAYS: **60**
test_bug_49081_negative__27: DAYS: **30**
test_bug_49081_negative__28: DAYS: **27**
test_bug_49081_negative__29: DAYS: **28**
test_bug_49081_negative__30: DAYS: **29**
test_bug_49081_negative__31: DAYS: **28**
test_bug_49081_negative__32: DAYS: **29**
