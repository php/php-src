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
	array("Europe/Tirane",      17, 17, 17, 1, 4849, 1970),
	array("America/Buenos_Aires", 17, 17, 17, 1, 1734, 1970),
	array("America/Rosario",    17, 17, 17, 1, 1734, 1970),
	array("Europe/Vienna",      17, 17, 17, 1, 3743, 1970),
	array("Asia/Baku",          17, 17, 17, 1, 9490, 1970),
);

foreach ($tests as $test) {
	date_default_timezone_set($test[0]);
	print "{$test[0]}\n";
	array_shift($test);
	$timestamp = call_user_func_array('mktime', $test);

	print "ts     = ". date("l Y-m-d H:i:s T", $timestamp). "\n";
	$strtotime_tstamp = strtotime("first monday", $timestamp);
	print "result = ".date("l Y-m-d H:i:s T", $strtotime_tstamp)."\n";
	print "wanted = Monday            00:00:00\n\n";
}
?>
--EXPECT--
Europe/Andorra
ts     = Monday 2037-10-19 17:17:17 CEST
result = Monday 2037-10-26 00:00:00 CET
wanted = Monday            00:00:00

Asia/Dubai
ts     = Thursday 1970-01-01 17:17:17 GST
result = Monday 1970-01-05 00:00:00 GST
wanted = Monday            00:00:00

Asia/Kabul
ts     = Thursday 1970-01-01 17:17:17 AFT
result = Monday 1970-01-05 00:00:00 AFT
wanted = Monday            00:00:00

America/Antigua
ts     = Thursday 1970-01-01 17:17:17 AST
result = Monday 1970-01-05 00:00:00 AST
wanted = Monday            00:00:00

America/Anguilla
ts     = Thursday 1970-01-01 17:17:17 AST
result = Monday 1970-01-05 00:00:00 AST
wanted = Monday            00:00:00

Europe/Tirane
ts     = Monday 1983-04-11 17:17:17 CET
result = Monday 1983-04-18 01:00:00 CEST
wanted = Monday            00:00:00

Asia/Yerevan
ts     = Monday 2037-10-19 17:17:17 AMST
result = Monday 2037-10-26 00:00:00 AMT
wanted = Monday            00:00:00

America/Curacao
ts     = Thursday 1970-01-01 17:17:17 AST
result = Monday 1970-01-05 00:00:00 AST
wanted = Monday            00:00:00

Africa/Luanda
ts     = Thursday 1970-01-01 17:17:17 WAT
result = Monday 1970-01-05 00:00:00 WAT
wanted = Monday            00:00:00

Antarctica/McMurdo
ts     = Monday 2037-09-28 17:17:17 NZDT
result = Monday 2037-10-05 00:00:00 NZDT
wanted = Monday            00:00:00

Australia/Adelaide
ts     = Friday 1971-01-01 17:17:17 CST
result = Monday 1971-01-04 00:00:00 CST
wanted = Monday            00:00:00

Australia/Darwin
ts     = Monday 1971-03-29 17:17:17 CST
result = Monday 1971-04-05 00:00:00 CST
wanted = Monday            00:00:00

Australia/Perth
ts     = Friday 1971-01-01 17:17:17 WST
result = Monday 1971-01-04 00:00:00 WST
wanted = Monday            00:00:00

America/Aruba
ts     = Monday 1971-03-29 17:17:17 AST
result = Monday 1971-04-05 00:00:00 AST
wanted = Monday            00:00:00

Asia/Baku
ts     = Friday 1971-01-01 17:17:17 BAKT
result = Monday 1971-01-04 00:00:00 BAKT
wanted = Monday            00:00:00

Europe/Sarajevo
ts     = Friday 1971-01-01 17:17:17 CET
result = Monday 1971-01-04 00:00:00 CET
wanted = Monday            00:00:00

America/Barbados
ts     = Friday 1971-01-01 17:17:17 AST
result = Monday 1971-01-04 00:00:00 AST
wanted = Monday            00:00:00

Asia/Dacca
ts     = Friday 1971-01-01 17:17:17 DACT
result = Monday 1971-01-04 00:00:00 DACT
wanted = Monday            00:00:00

Europe/Brussels
ts     = Friday 1971-01-01 17:17:17 CET
result = Monday 1971-01-04 00:00:00 CET
wanted = Monday            00:00:00

Africa/Ouagadougou
ts     = Monday 1971-03-29 17:17:17 GMT
result = Monday 1971-04-05 00:00:00 GMT
wanted = Monday            00:00:00

Europe/Tirane
ts     = Monday 1983-04-11 17:17:17 CET
result = Monday 1983-04-18 01:00:00 CEST
wanted = Monday            00:00:00

America/Buenos_Aires
ts     = Monday 1974-09-30 17:17:17 ART
result = Monday 1974-10-07 00:00:00 ART
wanted = Monday            00:00:00

America/Rosario
ts     = Monday 1974-09-30 17:17:17 ART
result = Monday 1974-10-07 00:00:00 ART
wanted = Monday            00:00:00

Europe/Vienna
ts     = Monday 1980-03-31 17:17:17 CET
result = Monday 1980-04-07 00:00:00 CEST
wanted = Monday            00:00:00

Asia/Baku
ts     = Monday 1995-12-25 17:17:17 AZT
result = Monday 1996-01-01 00:00:00 AZT
wanted = Monday            00:00:00
