--TEST--
Bug #53879 (DateTime::createFromFormat() fails to parse cookie expiration date)
--INI--
date.timezone=UTC
--FILE--
<?php
$date = DateTime::createFromFormat(DateTime::COOKIE, "Mon, 21-Jan-2041 15:24:52 GMT");
print_r($date);
?>
--EXPECTF--
DateTime Object
(
    [date] => 2041-01-21 15:24:52.000000
    [timezone_type] => 2
    [timezone] => GMT
)
