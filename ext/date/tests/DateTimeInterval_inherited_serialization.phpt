--TEST--
Inherited DateTimeInterval serialisation with custom properties
--FILE--
<?php
date_default_timezone_set("Europe/London");

class MyDateInterval extends DateInterval
{
	public function __construct(
		string $duration,
		public ?bool $myProperty = null,
	) {
		parent::__construct($duration);
	}
}

$d = new MyDateInterval("P1W2D", myProperty: true);
$e = unserialize(serialize($d));
var_dump($e->myProperty);
?>
--EXPECTF--
bool(true)
