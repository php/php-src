--TEST--
Bug #33869 (strtotime() doesn't parse "+1days" format)
--FILE--
<?php
	date_default_timezone_set("UTC");
	$tm = mktime(1,1,1,1,1,2005);
	echo date('m/d/Y H:m:s', strtotime('+5days',$tm));
	echo "\n";
	echo date('m/d/Y H:m:s', strtotime('+1month',$tm));
	echo "\n";
	echo date('m/d/Y H:m:s', strtotime('+1year',$tm));
	echo "\n";
	echo date('m/d/Y H:m:s', strtotime('+5 days',$tm));
	echo "\n";
	echo date('m/d/Y H:m:s', strtotime('+1 month',$tm));
	echo "\n";
	echo date('m/d/Y H:m:s', strtotime('+1 year',$tm));
	echo "\n";
?>
--EXPECT--
01/06/2005 01:01:01
02/01/2005 01:02:01
01/01/2006 01:01:01
01/06/2005 01:01:01
02/01/2005 01:02:01
01/01/2006 01:01:01
