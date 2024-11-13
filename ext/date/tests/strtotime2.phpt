--TEST--
strtotime() on date constants
--FILE--
<?php

date_default_timezone_set("Europe/Oslo");
$time = time();

$constants = array(
    'DATE_ATOM',
    'DATE_COOKIE',
    'DATE_ISO8601',
    'DATE_ISO8601_EXPANDED',
    'DATE_RFC822',
    'DATE_RFC850',
    'DATE_RFC1036',
    'DATE_RFC1123',
    'DATE_RFC2822',
    'DATE_RFC3339',
    'DATE_RFC3339_EXTENDED',
    'DATE_RSS',
    'DATE_W3C'
);


foreach ($constants as $const) {
    echo "$const:\t";
    echo ((strtotime(date(constant($const), $time)) === $time) ? "OK" : "FAIL") . "\n";
}
?>
--EXPECT--
DATE_ATOM:	OK
DATE_COOKIE:	OK
DATE_ISO8601:	OK
DATE_ISO8601_EXPANDED:	OK
DATE_RFC822:	OK
DATE_RFC850:	OK
DATE_RFC1036:	OK
DATE_RFC1123:	OK
DATE_RFC2822:	OK
DATE_RFC3339:	OK
DATE_RFC3339_EXTENDED:	OK
DATE_RSS:	OK
DATE_W3C:	OK
