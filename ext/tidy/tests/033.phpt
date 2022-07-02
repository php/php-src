--TEST--
tidy_warning_count() function - basic test for tidy_warning_count()
--CREDITS--
Christian Wenz <wenz@php.net>
--EXTENSIONS--
tidy
--FILE--
<?php
$buffer = '<img src="file.png" /><php>';

$tidy = tidy_parse_string($buffer);
var_dump(tidy_warning_count($tidy));
?>
--EXPECTF--
int(%d)
