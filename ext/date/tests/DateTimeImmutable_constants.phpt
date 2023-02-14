--TEST--
DateTimeImmutable constants
--FILE--
<?php

var_dump(
    DATE_ATOM               === DateTimeImmutable::ATOM,
    DATE_COOKIE             === DateTimeImmutable::COOKIE,
    DATE_ISO8601            === DateTimeImmutable::ISO8601,
    DATE_ISO8601_EXPANDED   === DateTimeImmutable::ISO8601_EXPANDED,
    DATE_RFC822             === DateTimeImmutable::RFC822,
    DATE_RFC850             === DateTimeImmutable::RFC850,
    DATE_RFC1036            === DateTimeImmutable::RFC1036,
    DATE_RFC1123            === DateTimeImmutable::RFC1123,
    DATE_RFC7231            === DateTimeImmutable::RFC7231,
    DATE_RFC2822            === DateTimeImmutable::RFC2822,
    DATE_RFC3339            === DateTimeImmutable::RFC3339,
    DATE_RFC3339_EXTENDED   === DateTimeImmutable::RFC3339_EXTENDED,
    DATE_RSS                === DateTimeImmutable::RSS,
    DATE_W3C                === DateTimeImmutable::W3C
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
bool(true)
bool(true)
bool(true)
