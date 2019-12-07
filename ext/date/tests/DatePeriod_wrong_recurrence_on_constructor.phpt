--TEST--
DatePeriod: Test wrong recurrence parameter on __construct
--FILE--
<?php
try {
    new DatePeriod(new DateTime('yesterday'), new DateInterval('P1D'), 0);
} catch (\ValueError $exception) {
    echo $exception->getMessage(), "\n";
}

try {
    new DatePeriod(new DateTime('yesterday'), new DateInterval('P1D'),-1);
} catch (\ValueError $exception) {
    echo $exception->getMessage(), "\n";
}
?>
--EXPECT--
The recurrence count '0' is invalid. Needs to be > 0
The recurrence count '-1' is invalid. Needs to be > 0
