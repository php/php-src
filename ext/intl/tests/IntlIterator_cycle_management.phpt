--TEST--
IntlIterator cycle management
--EXTENSIONS--
intl
--FILE--
<?php
function break_cycle_test() {
	$obj = IntlCalendar::getKeywordValuesForLocale('calendar', 'fa_IR', true);
	unset($obj);
	var_dump(gc_collect_cycles());
}
break_cycle_test();
break_cycle_test();
break_cycle_test();
?>
--EXPECT--
int(1)
int(1)
int(1)
