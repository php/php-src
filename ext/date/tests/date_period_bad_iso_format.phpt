--TEST--
Test bad ISO date formats passed to DatePeriod constructor
--FILE--
<?php

try {
    new DatePeriod("R4");
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

try {
    new DatePeriod("R4/2012-07-01T00:00:00Z");
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}


try {
    new DatePeriod("2012-07-01T00:00:00Z/P7D");
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
DatePeriod::__construct(): ISO interval must contain a start date, "R4" given
DatePeriod::__construct(): ISO interval must contain an interval, "R4/2012-07-01T00:00:00Z" given
DatePeriod::__construct(): ISO interval must contain an end date or a recurrence count, "2012-07-01T00:00:00Z/P7D" given
