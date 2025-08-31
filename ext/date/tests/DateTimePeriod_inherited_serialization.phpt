--TEST--
Inherited DateTimePeriod serialisation with custom properties
--FILE--
<?php
date_default_timezone_set("Europe/London");

class MyDatePeriod extends DatePeriod
{
	public function __construct(
		DateTimeInterface $start,
		DateInterval $interval,
		int $recurrences,
		int $options = 0,
		public ?bool $myProperty = null,
	) {
		parent::__construct($start, $interval, $recurrences, $options);
	}
}

$d = new MyDatePeriod(new DateTimeImmutable(), new DateInterval("PT5S"), 5, myProperty: true);
$e = unserialize(serialize($d));
var_dump($e->myProperty);
?>
--EXPECTF--
bool(true)
