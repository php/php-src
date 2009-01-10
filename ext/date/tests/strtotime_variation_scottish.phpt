--TEST--
Checking whisky time
--FILE--
<?php
	date_default_timezone_set('UTC');
	var_dump(date('H:i:s', strtotime('back of 7')));
	var_dump(date('H:i:s', strtotime('front of 7')));
	var_dump(date('H:i:s', strtotime('back of 19')));
	var_dump(date('H:i:s', strtotime('front of 19')));
?>
--EXPECTF--
%string|unicode%(8) "07:15:00"
%string|unicode%(8) "06:45:00"
%string|unicode%(8) "19:15:00"
%string|unicode%(8) "18:45:00"
