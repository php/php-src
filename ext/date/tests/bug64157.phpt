--TEST--
Test for bug #64157: DateTime::createFromFormat() reports confusing error message
--CREDITS--
Boro Sitnikovski <buritomath@yahoo.com>
--INI--
date.timezone = UTC
--FILE--
<?php
DateTime::createFromFormat('s', '0');
$lastErrors = DateTime::getLastErrors();
print_r($lastErrors['errors'][0]);
?>
--EXPECT--
A two digit second could not be found
