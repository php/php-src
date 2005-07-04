--TEST--
Bug #26198 (strtotime handling of "M Y" and "Y M" format)
--FILE--
<?php
	date_default_timezone_set("GMT");
	echo gmdate("F Y (Y-m-d H:i:s T)\n", strtotime("Oct 2001"));
	echo gmdate("M Y (Y-m-d H:i:s T)\n", strtotime("2001 Oct"));
?>
--EXPECT--
October 2001 (2001-10-01 00:00:00 GMT)
Oct 2001 (2001-10-01 00:00:00 GMT)
