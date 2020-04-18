--TEST--
Test DateTime class inheritance : with user space __construct magic method
--FILE--
<?php

//Set the default time zone
date_default_timezone_set("Europe/London");

echo "*** Testing new DateTime() : with user space __construct magic method ***\n";

class DateTimeExt extends DateTime
{
    public function __construct ($date = null, DateTimeZone  $dtz = null)
    {
        if($dtz === null)
        {
            $dtz = new DateTimeZone(date_default_timezone_get());
        }
        parent::__construct($date, $dtz);
    }
}

$d = new DateTimeExt("1967-05-01 22:30:41");
echo $d->format("F j, Y, g:i:s a") . "\n";

?>
--EXPECT--
*** Testing new DateTime() : with user space __construct magic method ***
May 1, 1967, 10:30:41 pm
