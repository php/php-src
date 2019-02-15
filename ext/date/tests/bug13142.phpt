--TEST--
Bug #13142 (strtotime handling of "M d H:i:s Y" format)
--INI--
date.timezone=US/Eastern
--SKIPIF--
<?php
if (defined('PHP_WINDOWS_VERSION_MAJOR')) {
	die("skip. set TZ env is not supported at runtime.");
}
if (!@putenv("TZ=US/Eastern") || getenv("TZ") != 'US/Eastern') {
	die("skip unable to change TZ environment variable\n");
}
?>
--FILE--
<?php

if (date('T') == 'GMT') {
	putenv("TZ=EST5EDT4,M4.1.0,M10.5.0");
}
echo date("r\n", strtotime("Sep 04 16:39:45 2001"));
echo date("r\n", strtotime("Sep 04 2001 16:39:45"));
?>
--EXPECT--
Tue, 04 Sep 2001 16:39:45 -0400
Tue, 04 Sep 2001 16:39:45 -0400
