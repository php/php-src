--TEST--
Test DateTime class inheritance : with user space format() method
--FILE--
<?php
//Set the default time zone
date_default_timezone_set("Europe/London");

echo "*** Testing new DateTime() : with user format() method ***\n";

class DateTimeExt extends DateTime
{
    public function format($format = "F j, Y, g:i:s a"): string
    {
        return parent::format($format);
    }
}

$d = new DateTimeExt("1967-05-01 22:30:41");
echo $d->format() . "\n";

?>
--EXPECT--
*** Testing new DateTime() : with user format() method ***
May 1, 1967, 10:30:41 pm
