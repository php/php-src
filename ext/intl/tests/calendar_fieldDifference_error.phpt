--TEST--
IntlCalendar::fieldDifference(): bad arguments
--EXTENSIONS--
intl
--FILE--
<?php

$c = new IntlGregorianCalendar(NULL, 'pt_PT');

var_dump($c->fieldDifference(INF, 2));
var_dump($c->getErrorMessage());

?>
--EXPECT--
bool(false)
string(88) "IntlCalendar::fieldDifference(): Call to ICU method has failed: U_ILLEGAL_ARGUMENT_ERROR"
