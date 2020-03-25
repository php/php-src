--TEST--
DateTime constants
--FILE--
<?php

var_dump(
    DATE_ATOM    === DateTime::ATOM,
    DATE_COOKIE  === DateTime::COOKIE,
    DATE_ISO8601 === DateTime::ISO8601,
    DATE_RFC822  === DateTime::RFC822,
    DATE_RFC850  === DateTime::RFC850,
    DATE_RFC1036 === DateTime::RFC1036,
    DATE_RFC1123 === DateTime::RFC1123,
    DATE_RFC2822 === DateTime::RFC2822,
    DATE_RFC3339 === DateTime::RFC3339,
    DATE_RSS     === DateTime::RSS,
    DATE_W3C     === DateTime::W3C
);

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
