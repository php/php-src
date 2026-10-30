--TEST--
Date constants
--FILE--
<?php
    date_default_timezone_set("Europe/Oslo");
    $constants = array(
        DATE_ATOM,
        DATE_COOKIE,
        DATE_ISO8601,
        DATE_ISO8601_EXPANDED,
        DATE_RFC822,
        DATE_RFC850,
        DATE_RFC1036,
        DATE_RFC1123,
        DATE_RFC7231,
        DATE_RFC2822,
        DATE_RFC3339,
        DATE_RFC3339_EXTENDED,
        DATE_RSS,
        DATE_W3C
    );

    foreach($constants as $const) {
        var_dump(date($const, strtotime("1 Jul 06 14:27:30 +0200")));
        var_dump(date($const, strtotime("2006-05-30T14:32:13+02:00")));
    }

    print "\n";

    var_dump(
        DATE_ATOM             == DateTime::ATOM,
        DATE_COOKIE           == DateTime::COOKIE,
        DATE_ISO8601          == DateTime::ISO8601,
        DATE_ISO8601_EXPANDED == DateTime::ISO8601_EXPANDED,
        DATE_RFC822           == DateTime::RFC822,
        DATE_RFC850           == DateTime::RFC850,
        DATE_RFC1036          == DateTime::RFC1036,
        DATE_RFC1123          == DateTime::RFC1123,
        DATE_RFC2822          == DateTime::RFC2822,
        DATE_RFC7231          == DateTime::RFC7231,
        DATE_RFC3339          == DateTime::RFC3339,
        DATE_RFC3339_EXTENDED == DateTime::RFC3339_EXTENDED,
        DATE_RSS              == DateTime::RSS,
        DATE_W3C              == DateTime::W3C
    );
?>
--EXPECT--
string(25) "2006-07-01T14:27:30+02:00"
string(25) "2006-05-30T14:32:13+02:00"
string(35) "Saturday, 01-Jul-2006 14:27:30 CEST"
string(34) "Tuesday, 30-May-2006 14:32:13 CEST"
string(24) "2006-07-01T14:27:30+0200"
string(24) "2006-05-30T14:32:13+0200"
string(26) "+2006-07-01T14:27:30+02:00"
string(26) "+2006-05-30T14:32:13+02:00"
string(29) "Sat, 01 Jul 06 14:27:30 +0200"
string(29) "Tue, 30 May 06 14:32:13 +0200"
string(33) "Saturday, 01-Jul-06 14:27:30 CEST"
string(32) "Tuesday, 30-May-06 14:32:13 CEST"
string(29) "Sat, 01 Jul 06 14:27:30 +0200"
string(29) "Tue, 30 May 06 14:32:13 +0200"
string(31) "Sat, 01 Jul 2006 14:27:30 +0200"
string(31) "Tue, 30 May 2006 14:32:13 +0200"
string(29) "Sat, 01 Jul 2006 14:27:30 GMT"
string(29) "Tue, 30 May 2006 14:32:13 GMT"
string(31) "Sat, 01 Jul 2006 14:27:30 +0200"
string(31) "Tue, 30 May 2006 14:32:13 +0200"
string(25) "2006-07-01T14:27:30+02:00"
string(25) "2006-05-30T14:32:13+02:00"
string(29) "2006-07-01T14:27:30.000+02:00"
string(29) "2006-05-30T14:32:13.000+02:00"
string(31) "Sat, 01 Jul 2006 14:27:30 +0200"
string(31) "Tue, 30 May 2006 14:32:13 +0200"
string(25) "2006-07-01T14:27:30+02:00"
string(25) "2006-05-30T14:32:13+02:00"

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
