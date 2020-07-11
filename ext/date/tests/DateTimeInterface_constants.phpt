--TEST--
DateTimeInterface constants
--FILE--
<?php

var_dump(
    DATE_ATOM    === DateTimeInterface::ATOM,
    DATE_COOKIE  === DateTimeInterface::COOKIE,
    DATE_ISO8601 === DateTimeInterface::ISO8601,
    DATE_RFC822  === DateTimeInterface::RFC822,
    DATE_RFC850  === DateTimeInterface::RFC850,
    DATE_RFC1036 === DateTimeInterface::RFC1036,
    DATE_RFC1123 === DateTimeInterface::RFC1123,
    DATE_RFC2822 === DateTimeInterface::RFC2822,
    DATE_RFC3339 === DateTimeInterface::RFC3339,
    DATE_RSS     === DateTimeInterface::RSS,
    DATE_W3C     === DateTimeInterface::W3C
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
