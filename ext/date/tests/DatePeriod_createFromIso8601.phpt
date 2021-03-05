--TEST--
Test DatePeriod::createFromIso8601()
--FILE--
<?php

class MyDatePeriod extends DatePeriod
{
}

$p1 = MyDatePeriod::createFromIso8601("R4/2012-07-01T00:00:00Z/P7D");
$p2 = new MyDatePeriod("R4/2012-07-01T00:00:00Z/P7D");

var_dump($p1::class);
var_dump($p1 == $p2);

try {
    MyDatePeriod::createFromIso8601("");
} catch (Exception $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
string(12) "MyDatePeriod"
bool(true)
DatePeriod::createFromIso8601(): Unknown or bad format ()
