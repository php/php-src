--TEST--
Test for bug #64560: timezones missing from DateTimeZone::listIdentifiers()
--CREDITS--
Boro Sitnikovski <buritomath@yahoo.com>
--FILE--
<?php
var_dump(in_array('Australia/Canberra', DateTimeZone::listIdentifiers()));
--EXPECT--
bool(true)
