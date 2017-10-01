--TEST--
Test get_browser() function variation : Return data as object
--INI--
browscap={PWD}/browscap.ini
--SKIPIF--
<?php
	/**
	 * Basic test, it would be pretty much coincidence if there's
	 * a browscap.ini on another place that isn't valid.
	 */
	if(! is_readable( ini_get( 'browscap' ) ) ) {
		die( 'skip: browscap.ini file ' . ini_get('browscap') . " not readable" );
	}
?>
--FILE--
<?php

$agent = "Opera/7.11 (Windows NT 5.1; U) [en]";
var_dump(get_browser($agent));

?>
--EXPECT--
object(stdClass)#1 (35) {
  ["browser_name_regex"]=>
  string(41) "~^opera/7\.1.* \(windows nt 5\.1; .\).*$~"
  ["browser_name_pattern"]=>
  string(31) "Opera/7.1* (Windows NT 5.1; ?)*"
  ["parent"]=>
  string(9) "Opera 7.1"
  ["platform"]=>
  string(5) "WinXP"
  ["win32"]=>
  string(1) "1"
  ["browser"]=>
  string(5) "Opera"
  ["version"]=>
  string(3) "7.1"
  ["majorver"]=>
  string(1) "7"
  ["minorver"]=>
  string(1) "1"
  ["frames"]=>
  string(1) "1"
  ["iframes"]=>
  string(1) "1"
  ["tables"]=>
  string(1) "1"
  ["cookies"]=>
  string(1) "1"
  ["backgroundsounds"]=>
  string(1) "1"
  ["javaapplets"]=>
  string(1) "1"
  ["javascript"]=>
  string(1) "1"
  ["css"]=>
  string(1) "2"
  ["cssversion"]=>
  string(1) "2"
  ["supportscss"]=>
  string(1) "1"
  ["alpha"]=>
  string(0) ""
  ["beta"]=>
  string(0) ""
  ["win16"]=>
  string(0) ""
  ["win64"]=>
  string(0) ""
  ["authenticodeupdate"]=>
  string(0) ""
  ["cdf"]=>
  string(0) ""
  ["vbscript"]=>
  string(0) ""
  ["activexcontrols"]=>
  string(0) ""
  ["stripper"]=>
  string(0) ""
  ["isbanned"]=>
  string(0) ""
  ["wap"]=>
  string(0) ""
  ["ismobiledevice"]=>
  string(0) ""
  ["issyndicationreader"]=>
  string(0) ""
  ["crawler"]=>
  string(0) ""
  ["aol"]=>
  string(0) ""
  ["aolversion"]=>
  string(1) "0"
}
