--TEST--
Bug #48476 (cloning extended DateTime class without calling parent::__constr crashed PHP)
--FILE--
<?php
class MyDateTime extends DateTime {
	public function __construct() { }
}
class MyDateTimeZone extends DateTimeZone {
	public function __construct() { }
}

$o = new MyDateTime;
var_dump($o->format("d"));
$x = clone $o;

var_dump($x->format("d"));

clone $o;


var_dump(timezone_location_get(clone new MyDateTimezone));
?>
--EXPECTF--
Warning: DateTime::format(): The DateTime object has not been correctly initialized by its constructor in %sbug48476.php on line 10
bool(false)

Warning: DateTime::format(): The DateTime object has not been correctly initialized by its constructor in %sbug48476.php on line 13
bool(false)

Warning: timezone_location_get(): The DateTimeZone object has not been correctly initialized by its constructor in %sbug48476.php on line 18
bool(false)
