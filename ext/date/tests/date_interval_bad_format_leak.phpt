--TEST--
DateInterval with bad format should not leak period
--FILE--
<?php

try {
    $interval = new DateInterval('P3"D');
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

try {
    $perid = new DatePeriod('P3"D');
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
DateInterval::__construct(): Unknown or bad format (P3"D)
DatePeriod::__construct(): Unknown or bad format (P3"D)
