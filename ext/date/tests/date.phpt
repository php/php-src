--TEST--
date() function
--FILE--
<?php
$tmp = "cr";
if (defined('PHP_WINDOWS_VERSION_MAJOR')) {
	date_default_timezone_set('GMT0');
} else {
	putenv ("TZ=GMT0");
}


for($a = 0;$a < strlen($tmp); $a++){
	echo $tmp[$a], ': ', date($tmp[$a], 1043324459)."\n";
}

if (defined('PHP_WINDOWS_VERSION_MAJOR')) {
	date_default_timezone_set("MET");
} else {
	putenv ("TZ=MET");
}
for($a = 0;$a < strlen($tmp); $a++){
	echo $tmp[$a], ': ', date($tmp[$a], 1043324459)."\n";
}
?>
--EXPECT--
c: 2003-01-23T12:20:59+00:00
r: Thu, 23 Jan 2003 12:20:59 +0000
c: 2003-01-23T13:20:59+01:00
r: Thu, 23 Jan 2003 13:20:59 +0100
