--TEST--
Bug #54043: Remove inconsitency of internal exceptions and user defined exceptions
--FILE--
<?php

$time = '9999-11-33';	// obviously invalid ;-)
$timeZone = new DateTimeZone('UTC');

try {
    $dateTime = new DateTime($time, $timeZone);
} catch (Exception $e) {
    var_dump($e->getMessage());
}

var_dump(error_get_last());

?>
--EXPECT--
string(105) "DateTime::__construct(): Failed to parse time string (9999-11-33) at position 9 (3): Unexpected character"
NULL
