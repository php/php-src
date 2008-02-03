--TEST--
Bug #43960 (strtotime() returns timestamp in the future when given a bogus string)
--INI--
date.timezone=UTC
--FILE--
<?php
var_dump(strtotime('i like to eat slices at work'));
?>
--EXPECT--
bool(false)
