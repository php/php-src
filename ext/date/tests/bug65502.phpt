--TEST--
Test for bug #65502: DateTimeImmutable::createFromFormat returns DateTime
--CREDITS--
Boro Sitnikovski <buritomath@yahoo.com>
--INI--
date.timezone = UTC
--FILE--
<?php
echo get_class(DateTimeImmutable::createFromFormat('j-M-Y', '12-Sep-2013'));
?>
--EXPECT--
DateTimeImmutable
