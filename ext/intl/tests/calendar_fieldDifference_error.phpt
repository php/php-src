--TEST--
IntlCalendar::fieldDifference(): bad arguments
--INI--
date.timezone=Atlantic/Azores
--EXTENSIONS--
intl
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

$c = new IntlGregorianCalendar(NULL, 'pt_PT');

try {
    var_dump($c->fieldDifference($c, 2, 3));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
var_dump($c->fieldDifference(INF, 2));

try {
    var_dump(intlcal_field_difference($c, 0, 1, 2));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
var_dump(intlcal_field_difference(1, 0, 1));
?>
--EXPECTF--
IntlCalendar::fieldDifference() expects exactly 2 arguments, 3 given

Warning: IntlCalendar::fieldDifference(): intlcal_field_difference: Call to ICU method has failed in %s on line %d
bool(false)
intlcal_field_difference() expects exactly 3 arguments, 4 given

Fatal error: Uncaught TypeError: intlcal_field_difference(): Argument #1 ($calendar) must be of type IntlCalendar, int given in %s:%d
Stack trace:
#0 %s(%d): intlcal_field_difference(1, 0, 1)
#1 {main}
  thrown in %s on line %d
