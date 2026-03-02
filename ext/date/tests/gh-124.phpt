--TEST--
Test for timelib #124: Problem with large negative timestamps
--INI--
date.timezone=UTC
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) echo "skip this test is for 64-bit only";
if (getenv('SKIP_ASAN')) die('skip triggers undefined behavior');
?>
--FILE--
<?php
var_dump((new DateTime("@-9223372036854775808"))->getTimestamp());
var_dump((new DateTime('-292277022657-01-27 08:29:52 UTC'))->getTimestamp());
var_dump((new DateTime('-292277022657-01-27 08:29:53 UTC'))->getTimestamp());

var_dump((new DateTime("@9223372036854775807"))->getTimestamp());
var_dump((new DateTime('+292277026596-12-04 15:30:07 UTC'))->getTimestamp());
echo (new DateTime('+2512370164-01-01 00:00:00Z'))->format(DATE_RFC3339_EXTENDED);
?>
--EXPECT--
int(-9223372036854775808)
int(-9223372036854775808)
int(-9223372036854775807)
int(9223372036854775807)
int(9223372036854775807)
2512370164-01-01T00:00:00.000+00:00
