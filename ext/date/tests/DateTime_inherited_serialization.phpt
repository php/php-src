--TEST--
Inherited DateTime serialisation with custom properties
--FILE--
<?php
date_default_timezone_set("Europe/London");

class MyDateTime extends DateTime
{
	public function __construct(
		string $datetime = "now",
		?DateTimeZone $timezone = null,
		public ?bool $myProperty = null,
	) {
		parent::__construct($datetime, $timezone);
	}
}

$d = new MyDateTime("2023-01-25 16:32:55", myProperty: true);
$e = unserialize(serialize($d));
var_dump($e->myProperty);
?>
--EXPECTF--
bool(true)
