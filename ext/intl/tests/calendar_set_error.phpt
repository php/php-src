--TEST--
IntlCalendar::set(): bad arguments
--INI--
date.timezone=Atlantic/Azores
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

$c = new IntlGregorianCalendar(NULL, 'pt_PT');

try {
    $c->set(1, 2, 3, 4, 5, 6, 7);
} catch (ArgumentCountError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $c->set(1, 2, 3, 4);
} catch (ArgumentCountError $exception) {
    echo $exception->getMessage() . "\n";
}

var_dump($c->set(-1, 2));

var_dump(intlcal_set($c, -1, 2));
var_dump(intlcal_set(1, 2, 3));
--EXPECTF--
IntlCalendar::set() expects at most 6 parameters, 7 given
No variant with 4 arguments

Warning: IntlCalendar::set(): intlcal_set: invalid field in %s on line %d
bool(false)

Warning: intlcal_set(): intlcal_set: invalid field in %s on line %d
bool(false)

Fatal error: Uncaught TypeError: intlcal_set(): Argument #1 ($calendar) must be of type IntlCalendar, int given in %s:%d
Stack trace:
#0 %s(%d): intlcal_set(1, 2, 3)
#1 {main}
  thrown in %s on line %d
