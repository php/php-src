--TEST--
Bug #34087 (strtotime() doesn't parse "yyyy/m?m/d?d" format)
--FILE--
<?php
	date_default_timezone_set("GMT");
	echo date(DATE_ISO8601, strtotime("2005/1/2"));
	echo "\n";
	echo date(DATE_ISO8601, strtotime("2005/01/02"));
	echo "\n";
	echo date(DATE_ISO8601, strtotime("2005/01/2"));
	echo "\n";
	echo date(DATE_ISO8601, strtotime("2005/1/02"));
	echo "\n";
?>
--EXPECT--
2005-01-02T00:00:00+0000
2005-01-02T00:00:00+0000
2005-01-02T00:00:00+0000
2005-01-02T00:00:00+0000
