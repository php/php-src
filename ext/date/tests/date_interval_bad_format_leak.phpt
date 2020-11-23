--TEST--
DateInterval with bad format should not leak period
--FILE--
<?php

try {
    new DateInterval('P3"D');
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

try {
    new DatePeriod('P3"D');
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

try {
    new DatePeriod('2008-03-01T12:00:00Z1');
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
DateInterval::__construct(): Unknown or bad format (P3"D)
DatePeriod::__construct(): Unknown or bad format (P3"D)
DatePeriod::__construct(): Unknown or bad format (2008-03-01T12:00:00Z1)
