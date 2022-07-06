--TEST--
DatePeriod: Test wrong recurrence parameter on __construct
--FILE--
<?php
try {
    new DatePeriod(new DateTime('yesterday'), new DateInterval('P1D'), 0);
} catch (Exception $exception) {
    echo $exception->getMessage(), "\n";
}

try {
    new DatePeriod(new DateTime('yesterday'), new DateInterval('P1D'), -1);
} catch (Exception $exception) {
    echo $exception->getMessage(), "\n";
}

?>
--EXPECT--
DatePeriod::__construct(): Recurrence count must be greater than 0
DatePeriod::__construct(): Recurrence count must be greater than 0
