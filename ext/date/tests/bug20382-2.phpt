--TEST--
Bug #20382 [2] (strtotime ("Monday", $date) produces wrong result on DST changeover)
--FILE--
<?php

$tests = array(
	array("Europe/Andorra",     17, 17, 17, 1, 24764, 1970),
	array("Asia/Dubai",         17, 17, 17, 1, 1, 1970),
	array("Asia/Kabul",         17, 17, 17, 1, 1, 1970),
	array("America/Antigua",    17, 17, 17, 1, 1, 1970),
	array("America/Anguilla",   17, 17, 17, 1, 1, 1970),
	array("Europe/Tirane",      17, 17, 17, 1, 4849, 1970),
	array("Asia/Yerevan",       17, 17, 17, 1, 24764, 1970),
	array("America/Curacao",    17, 17, 17, 1, 1, 1970),
	array("Africa/Luanda",      17, 17, 17, 1, 1, 1970),
	array("Antarctica/McMurdo", 17, 17, 17, 1, 24743, 1970),
	array("Australia/Adelaide", 17, 17, 17, 1, 1, 1971),
	array("Australia/Darwin",   17, 17, 17, 1, 88, 1971),
	array("Australia/Perth",    17, 17, 17, 1, 1, 1971),
	array("America/Aruba",      17, 17, 17, 1, 88, 1971),
	array("Asia/Baku",          17, 17, 17, 1, 1, 1971),
	array("Europe/Sarajevo",    17, 17, 17, 1, 1, 1971),
	array("America/Barbados",   17, 17, 17, 1, 1, 1971),
	array("Asia/Dacca",         17, 17, 17, 1, 1, 1971),
	array("Europe/Brussels",    17, 17, 17, 1, 1, 1971),
	array("Africa/Ouagadougou", 17, 17, 17, 1, 88, 1971),
);

foreach ($tests as $test) {
	putenv("TZ={$test[0]}");
	array_shift($test);
	$timestamp = call_user_func_array('mktime', $test);

	print "ts     = ". date("l Y-m-d H:i:s T", $timestamp). "\n";
	$strtotime_tstamp = strtotime("Monday", $timestamp);
	print "result = ".date("l Y-m-d H:i:s T", $strtotime_tstamp)."\n";
	print "wanted = Monday            00:00:00\n\n";
}
?>
--EXPECT--
ts     = Monday 2037-10-19 17:17:17 CEST
result = Monday 2037-10-26 00:00:00 CET
wanted = Monday            00:00:00

ts     = Thursday 1970-01-01 17:17:17 GST
result = Monday 1970-01-05 00:00:00 GST
wanted = Monday            00:00:00

ts     = Thursday 1970-01-01 17:17:17 AFT
result = Monday 1970-01-05 00:00:00 AFT
wanted = Monday            00:00:00

ts     = Thursday 1970-01-01 17:17:17 AST
result = Monday 1970-01-05 00:00:00 AST
wanted = Monday            00:00:00

ts     = Thursday 1970-01-01 17:17:17 AST
result = Monday 1970-01-05 00:00:00 AST
wanted = Monday            00:00:00

ts     = Monday 1983-04-11 17:17:17 CET
result = Monday 1983-04-18 01:00:00 CEST
wanted = Monday            00:00:00

ts     = Monday 2037-10-19 17:17:17 AMST
result = Monday 2037-10-26 00:00:00 AMT
wanted = Monday            00:00:00

ts     = Thursday 1970-01-01 17:17:17 AST
result = Monday 1970-01-05 00:00:00 AST
wanted = Monday            00:00:00

ts     = Thursday 1970-01-01 17:17:17 WAT
result = Monday 1970-01-05 00:00:00 WAT
wanted = Monday            00:00:00

ts     = Monday 2037-09-28 17:17:17 NZST
result = Monday 2037-10-05 00:00:00 NZDT
wanted = Monday            00:00:00

ts     = Friday 1971-01-01 17:17:17 CST
result = Monday 1971-01-04 00:00:00 CST
wanted = Monday            00:00:00

ts     = Monday 1971-03-29 17:17:17 CST
result = Monday 1971-04-05 00:00:00 CST
wanted = Monday            00:00:00

ts     = Friday 1971-01-01 17:17:17 WST
result = Monday 1971-01-04 00:00:00 WST
wanted = Monday            00:00:00

ts     = Monday 1971-03-29 17:17:17 AST
result = Monday 1971-04-05 00:00:00 AST
wanted = Monday            00:00:00

ts     = Friday 1971-01-01 17:17:17 BAKT
result = Monday 1971-01-04 00:00:00 BAKT
wanted = Monday            00:00:00

ts     = Friday 1971-01-01 17:17:17 CET
result = Monday 1971-01-04 00:00:00 CET
wanted = Monday            00:00:00

ts     = Friday 1971-01-01 17:17:17 AST
result = Monday 1971-01-04 00:00:00 AST
wanted = Monday            00:00:00

ts     = Friday 1971-01-01 17:17:17 DACT
result = Monday 1971-01-04 00:00:00 DACT
wanted = Monday            00:00:00

ts     = Friday 1971-01-01 17:17:17 CET
result = Monday 1971-01-04 00:00:00 CET
wanted = Monday            00:00:00

ts     = Monday 1971-03-29 17:17:17 GMT
result = Monday 1971-04-05 00:00:00 GMT
wanted = Monday            00:00:00
