--TEST--
tidy_error_count() function - basic test for tidy_error_count()
--CREDITS--
Christian Wenz <wenz@php.net>
--EXTENSIONS--
tidy
--FILE--
<?php
$buffer = '<img src="file.png" /><php>';

$tidy = tidy_parse_string($buffer);
var_dump(tidy_error_count($tidy));
?>
--EXPECTF--
int(%d)
