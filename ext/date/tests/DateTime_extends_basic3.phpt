--TEST--
Test DateTime class inheritance : with user space fromat() method
--FILE--
<?php
//Set the default time zone
date_default_timezone_set("Europe/London");

echo "*** Testing new DateTime() : with user format() method ***\n";

class DateTimeExt extends DateTime
{
	public function format($format = "F j, Y, g:i:s a")
	{
		return parent::format($format);
	}
}

$d = new DateTimeExt("1967-05-01 22:30:41");
echo $d->format() . "\n";

?>
===DONE===
--EXPECTF--
*** Testing new DateTime() : with user format() method ***
May 1, 1967, 10:30:41 pm
===DONE===
