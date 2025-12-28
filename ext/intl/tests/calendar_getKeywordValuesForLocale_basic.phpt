--TEST--
IntlCalendar::getKeywordValuesForLocale() basic test
--EXTENSIONS--
intl
--FILE--
<?php

var_dump(iterator_to_array(IntlCalendar::getKeywordValuesForLocale('calendar', 'pt', true)));
echo "\n";

$var = iterator_to_array(intlcal_get_keyword_values_for_locale('calendar', 'pt', false));
var_dump(count($var) > 8);
var_dump(in_array('japanese', $var));

?>
--EXPECT--
array(1) {
  [0]=>
  string(9) "gregorian"
}

bool(true)
bool(true)
