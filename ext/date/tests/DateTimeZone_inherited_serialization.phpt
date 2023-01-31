--TEST--
Inherited DateTimeZone serialisation with custom properties
--FILE--
<?php
date_default_timezone_set("Europe/London");

class MyDateTimeZone extends DateTimeZone
{
	public function __construct(
		string $timezone = "Europe/Kyiv",
		public ?bool $myProperty = null,
	) {
		parent::__construct($timezone);
	}
}

$d = new MyDateTimeZone("Europe/London", myProperty: true);
$e = unserialize(serialize($d));
var_dump($e->myProperty);
?>
--EXPECTF--
bool(true)
