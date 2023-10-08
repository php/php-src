--TEST--
Bug #81565 (date parsing fails when provided with timezones including seconds)
--FILE--
<?php
var_export(
    \DateTime::createFromFormat(
        'Y-m-d H:i:sO',
        '0021-08-21 00:00:00+00:49:56'
    )
);

echo "\n", (new DatetimeZone('+01:45:30'))->getName();
?>
--EXPECT--
\DateTime::__set_state(array(
   'date' => '0021-08-21 00:00:00.000000',
   'timezone_type' => 1,
   'timezone' => '+00:49',
))
+01:45:30
