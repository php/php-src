--TEST--
tidy_access_count() function - basic test for tidy_access_count()
--CREDITS--
Christian Wenz <wenz@php.net>
--EXTENSIONS--
tidy
--FILE--
<?php
$buffer = '<img src="file.png" /><php>';
$config = array(
  'accessibility-check' => 1);

$tidy = tidy_parse_string($buffer, $config);
$tidy->diagnose();
var_dump(tidy_access_count($tidy));
?>
--EXPECTF--
int(%d)
