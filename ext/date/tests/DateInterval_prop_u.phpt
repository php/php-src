--TEST--
DateInterval::$u behavior
--FILE--
<?php

function dump_usecs(DateInterval $interval): void
{
    var_dump(
        $interval->u,
        sprintf("%.6F", $interval->f),
        $interval->format("%f"),
        $interval->format("%F")
    );
    echo "\n";
}

$interval = new DateInterval('PT0S');
dump_usecs($interval);

$interval->u = 123;
dump_usecs($interval);

$interval->f = 0.456;
dump_usecs($interval);

$interval->u = 987654;
dump_usecs($interval);

?>
--EXPECT--
int(0)
string(8) "0.000000"
string(1) "0"
string(6) "000000"

int(123)
string(8) "0.000123"
string(3) "123"
string(6) "000123"

int(456000)
string(8) "0.456000"
string(6) "456000"
string(6) "456000"

int(987654)
string(8) "0.987654"
string(6) "987654"
string(6) "987654"

