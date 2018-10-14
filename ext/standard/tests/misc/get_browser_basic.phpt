--TEST--
Test get_browser() function : basic functionality
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
/* Prototype  : mixed get_browser([string browser_name [, bool return_array]])
 * Description: Get information about the capabilities of a browser.
 * If browser_name is omitted or null, HTTP_USER_AGENT is used.
 * Returns an object by default; if return_array is true, returns an array.
 *
 * Source code: ext/standard/browscap.c
 * Alias to functions:
 */

$browsers = include dirname(__FILE__) . DIRECTORY_SEPARATOR . 'browsernames.inc';

echo "*** Testing get_browser() : basic functionality ***\n";

for( $x = 0; $x < 20; $x++) {
	var_dump( get_browser( $browsers[$x], true ) );
}

?>
===DONE===
--EXPECT--
*** Testing get_browser() : basic functionality ***
array(35) {
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
array(35) {
  ["browser_name_regex"]=>
  string(83) "~^mozilla/4\.0 \(compatible; msie 6\.0; msie 5\.5; windows nt 5\.0\) opera 7\.0.*$~"
  ["browser_name_pattern"]=>
  string(71) "Mozilla/4.0 (compatible; MSIE 6.0; MSIE 5.5; Windows NT 5.0) Opera 7.0*"
  ["parent"]=>
  string(9) "Opera 7.0"
  ["platform"]=>
  string(7) "Win2000"
  ["win32"]=>
  string(1) "1"
  ["browser"]=>
  string(5) "Opera"
  ["version"]=>
  string(3) "7.0"
  ["majorver"]=>
  string(1) "7"
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
  ["minorver"]=>
  string(1) "0"
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
array(35) {
  ["browser_name_regex"]=>
  string(83) "~^mozilla/4\.0 \(compatible; msie 6\.0; msie 5\.5; windows nt 4\.0\) opera 7\.0.*$~"
  ["browser_name_pattern"]=>
  string(71) "Mozilla/4.0 (compatible; MSIE 6.0; MSIE 5.5; Windows NT 4.0) Opera 7.0*"
  ["parent"]=>
  string(9) "Opera 7.0"
  ["platform"]=>
  string(5) "WinNT"
  ["win32"]=>
  string(1) "1"
  ["browser"]=>
  string(5) "Opera"
  ["version"]=>
  string(3) "7.0"
  ["majorver"]=>
  string(1) "7"
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
  ["minorver"]=>
  string(1) "0"
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
array(35) {
  ["browser_name_regex"]=>
  string(69) "~^mozilla/4\.0 \(compatible; msie 5\.0; windows 2000\) opera 6\.0.*$~"
  ["browser_name_pattern"]=>
  string(59) "Mozilla/4.0 (compatible; MSIE 5.0; Windows 2000) Opera 6.0*"
  ["parent"]=>
  string(9) "Opera 6.0"
  ["platform"]=>
  string(7) "Win2000"
  ["win32"]=>
  string(1) "1"
  ["browser"]=>
  string(5) "Opera"
  ["version"]=>
  string(3) "6.0"
  ["majorver"]=>
  string(1) "6"
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
  string(1) "1"
  ["cssversion"]=>
  string(1) "1"
  ["supportscss"]=>
  string(1) "1"
  ["minorver"]=>
  string(1) "0"
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
array(35) {
  ["browser_name_regex"]=>
  string(68) "~^mozilla/4\.0 \(compatible; msie 5\.0; mac_powerpc\) opera 5\.0.*$~"
  ["browser_name_pattern"]=>
  string(58) "Mozilla/4.0 (compatible; MSIE 5.0; Mac_PowerPC) Opera 5.0*"
  ["parent"]=>
  string(9) "Opera 5.0"
  ["platform"]=>
  string(6) "MacPPC"
  ["browser"]=>
  string(5) "Opera"
  ["version"]=>
  string(3) "5.0"
  ["majorver"]=>
  string(1) "5"
  ["frames"]=>
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
  string(1) "1"
  ["cssversion"]=>
  string(1) "1"
  ["supportscss"]=>
  string(1) "1"
  ["minorver"]=>
  string(1) "0"
  ["alpha"]=>
  string(0) ""
  ["beta"]=>
  string(0) ""
  ["win16"]=>
  string(0) ""
  ["win32"]=>
  string(0) ""
  ["win64"]=>
  string(0) ""
  ["iframes"]=>
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
array(35) {
  ["browser_name_regex"]=>
  string(65) "~^mozilla/4\.0 \(compatible; msie 6\.0; .*windows nt 5\.1.*\).*$~"
  ["browser_name_pattern"]=>
  string(53) "Mozilla/4.0 (compatible; MSIE 6.0; *Windows NT 5.1*)*"
  ["parent"]=>
  string(6) "IE 6.0"
  ["platform"]=>
  string(5) "WinXP"
  ["browser"]=>
  string(2) "IE"
  ["version"]=>
  string(3) "6.0"
  ["majorver"]=>
  string(1) "6"
  ["win32"]=>
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
  ["cdf"]=>
  string(1) "1"
  ["vbscript"]=>
  string(1) "1"
  ["javaapplets"]=>
  string(1) "1"
  ["javascript"]=>
  string(1) "1"
  ["activexcontrols"]=>
  string(1) "1"
  ["css"]=>
  string(1) "2"
  ["cssversion"]=>
  string(1) "2"
  ["supportscss"]=>
  string(1) "1"
  ["minorver"]=>
  string(1) "0"
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
array(35) {
  ["browser_name_regex"]=>
  string(40) "~^opera/9\.0.* \(windows nt 5\.1.*\).*$~"
  ["browser_name_pattern"]=>
  string(29) "Opera/9.0* (Windows NT 5.1*)*"
  ["parent"]=>
  string(9) "Opera 9.0"
  ["platform"]=>
  string(5) "WinXP"
  ["win32"]=>
  string(1) "1"
  ["browser"]=>
  string(5) "Opera"
  ["version"]=>
  string(3) "9.0"
  ["majorver"]=>
  string(1) "9"
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
  ["minorver"]=>
  string(1) "0"
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
array(35) {
  ["browser_name_regex"]=>
  string(47) "~^opera/9\.2.* \(macintosh; .*mac os x;.*\).*$~"
  ["browser_name_pattern"]=>
  string(36) "Opera/9.2* (Macintosh; *Mac OS X;*)*"
  ["parent"]=>
  string(9) "Opera 9.2"
  ["platform"]=>
  string(6) "MacOSX"
  ["browser"]=>
  string(5) "Opera"
  ["version"]=>
  string(3) "9.2"
  ["majorver"]=>
  string(1) "9"
  ["minorver"]=>
  string(1) "2"
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
  ["win32"]=>
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
array(35) {
  ["browser_name_regex"]=>
  string(65) "~^mozilla/4\.0 \(compatible; msie 6\.0; .*windows nt 5\.1.*\).*$~"
  ["browser_name_pattern"]=>
  string(53) "Mozilla/4.0 (compatible; MSIE 6.0; *Windows NT 5.1*)*"
  ["parent"]=>
  string(6) "IE 6.0"
  ["platform"]=>
  string(5) "WinXP"
  ["browser"]=>
  string(2) "IE"
  ["version"]=>
  string(3) "6.0"
  ["majorver"]=>
  string(1) "6"
  ["win32"]=>
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
  ["cdf"]=>
  string(1) "1"
  ["vbscript"]=>
  string(1) "1"
  ["javaapplets"]=>
  string(1) "1"
  ["javascript"]=>
  string(1) "1"
  ["activexcontrols"]=>
  string(1) "1"
  ["css"]=>
  string(1) "2"
  ["cssversion"]=>
  string(1) "2"
  ["supportscss"]=>
  string(1) "1"
  ["minorver"]=>
  string(1) "0"
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
array(35) {
  ["browser_name_regex"]=>
  string(65) "~^mozilla/4\.0 \(compatible; msie 6\.0; .*windows nt 5\.1.*\).*$~"
  ["browser_name_pattern"]=>
  string(53) "Mozilla/4.0 (compatible; MSIE 6.0; *Windows NT 5.1*)*"
  ["parent"]=>
  string(6) "IE 6.0"
  ["platform"]=>
  string(5) "WinXP"
  ["browser"]=>
  string(2) "IE"
  ["version"]=>
  string(3) "6.0"
  ["majorver"]=>
  string(1) "6"
  ["win32"]=>
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
  ["cdf"]=>
  string(1) "1"
  ["vbscript"]=>
  string(1) "1"
  ["javaapplets"]=>
  string(1) "1"
  ["javascript"]=>
  string(1) "1"
  ["activexcontrols"]=>
  string(1) "1"
  ["css"]=>
  string(1) "2"
  ["cssversion"]=>
  string(1) "2"
  ["supportscss"]=>
  string(1) "1"
  ["minorver"]=>
  string(1) "0"
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
array(35) {
  ["browser_name_regex"]=>
  string(59) "~^mozilla/.\.. \(compatible; msie 5\.2.*; .*mac_powerpc.*$~"
  ["browser_name_pattern"]=>
  string(49) "Mozilla/?.? (compatible; MSIE 5.2*; *Mac_PowerPC*"
  ["parent"]=>
  string(6) "IE Mac"
  ["version"]=>
  string(3) "5.2"
  ["majorver"]=>
  string(1) "5"
  ["minorver"]=>
  string(1) "2"
  ["browser"]=>
  string(2) "IE"
  ["platform"]=>
  string(6) "MacPPC"
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
  ["cdf"]=>
  string(1) "1"
  ["javaapplets"]=>
  string(1) "1"
  ["javascript"]=>
  string(1) "1"
  ["css"]=>
  string(1) "1"
  ["cssversion"]=>
  string(1) "1"
  ["supportscss"]=>
  string(1) "1"
  ["alpha"]=>
  string(0) ""
  ["beta"]=>
  string(0) ""
  ["win16"]=>
  string(0) ""
  ["win32"]=>
  string(0) ""
  ["win64"]=>
  string(0) ""
  ["authenticodeupdate"]=>
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
array(35) {
  ["browser_name_regex"]=>
  string(59) "~^mozilla/.\.. \(compatible; msie 5\.0.*; .*mac_powerpc.*$~"
  ["browser_name_pattern"]=>
  string(49) "Mozilla/?.? (compatible; MSIE 5.0*; *Mac_PowerPC*"
  ["parent"]=>
  string(6) "IE Mac"
  ["version"]=>
  string(3) "5.0"
  ["majorver"]=>
  string(1) "5"
  ["minorver"]=>
  string(1) "0"
  ["browser"]=>
  string(2) "IE"
  ["platform"]=>
  string(6) "MacPPC"
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
  ["cdf"]=>
  string(1) "1"
  ["javaapplets"]=>
  string(1) "1"
  ["javascript"]=>
  string(1) "1"
  ["css"]=>
  string(1) "1"
  ["cssversion"]=>
  string(1) "1"
  ["supportscss"]=>
  string(1) "1"
  ["alpha"]=>
  string(0) ""
  ["beta"]=>
  string(0) ""
  ["win16"]=>
  string(0) ""
  ["win32"]=>
  string(0) ""
  ["win64"]=>
  string(0) ""
  ["authenticodeupdate"]=>
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
array(34) {
  ["browser_name_regex"]=>
  string(6) "~^.*$~"
  ["browser_name_pattern"]=>
  string(1) "*"
  ["browser"]=>
  string(15) "Default Browser"
  ["version"]=>
  string(1) "0"
  ["majorver"]=>
  string(1) "0"
  ["minorver"]=>
  string(1) "0"
  ["platform"]=>
  string(7) "unknown"
  ["alpha"]=>
  string(0) ""
  ["beta"]=>
  string(0) ""
  ["win16"]=>
  string(0) ""
  ["win32"]=>
  string(0) ""
  ["win64"]=>
  string(0) ""
  ["frames"]=>
  string(1) "1"
  ["iframes"]=>
  string(0) ""
  ["tables"]=>
  string(1) "1"
  ["cookies"]=>
  string(0) ""
  ["backgroundsounds"]=>
  string(0) ""
  ["authenticodeupdate"]=>
  string(1) "0"
  ["cdf"]=>
  string(0) ""
  ["vbscript"]=>
  string(0) ""
  ["javaapplets"]=>
  string(0) ""
  ["javascript"]=>
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
  ["css"]=>
  string(1) "0"
  ["cssversion"]=>
  string(1) "0"
  ["supportscss"]=>
  string(0) ""
  ["aol"]=>
  string(0) ""
  ["aolversion"]=>
  string(1) "0"
}
array(34) {
  ["browser_name_regex"]=>
  string(6) "~^.*$~"
  ["browser_name_pattern"]=>
  string(1) "*"
  ["browser"]=>
  string(15) "Default Browser"
  ["version"]=>
  string(1) "0"
  ["majorver"]=>
  string(1) "0"
  ["minorver"]=>
  string(1) "0"
  ["platform"]=>
  string(7) "unknown"
  ["alpha"]=>
  string(0) ""
  ["beta"]=>
  string(0) ""
  ["win16"]=>
  string(0) ""
  ["win32"]=>
  string(0) ""
  ["win64"]=>
  string(0) ""
  ["frames"]=>
  string(1) "1"
  ["iframes"]=>
  string(0) ""
  ["tables"]=>
  string(1) "1"
  ["cookies"]=>
  string(0) ""
  ["backgroundsounds"]=>
  string(0) ""
  ["authenticodeupdate"]=>
  string(1) "0"
  ["cdf"]=>
  string(0) ""
  ["vbscript"]=>
  string(0) ""
  ["javaapplets"]=>
  string(0) ""
  ["javascript"]=>
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
  ["css"]=>
  string(1) "0"
  ["cssversion"]=>
  string(1) "0"
  ["supportscss"]=>
  string(0) ""
  ["aol"]=>
  string(0) ""
  ["aolversion"]=>
  string(1) "0"
}
array(34) {
  ["browser_name_regex"]=>
  string(6) "~^.*$~"
  ["browser_name_pattern"]=>
  string(1) "*"
  ["browser"]=>
  string(15) "Default Browser"
  ["version"]=>
  string(1) "0"
  ["majorver"]=>
  string(1) "0"
  ["minorver"]=>
  string(1) "0"
  ["platform"]=>
  string(7) "unknown"
  ["alpha"]=>
  string(0) ""
  ["beta"]=>
  string(0) ""
  ["win16"]=>
  string(0) ""
  ["win32"]=>
  string(0) ""
  ["win64"]=>
  string(0) ""
  ["frames"]=>
  string(1) "1"
  ["iframes"]=>
  string(0) ""
  ["tables"]=>
  string(1) "1"
  ["cookies"]=>
  string(0) ""
  ["backgroundsounds"]=>
  string(0) ""
  ["authenticodeupdate"]=>
  string(1) "0"
  ["cdf"]=>
  string(0) ""
  ["vbscript"]=>
  string(0) ""
  ["javaapplets"]=>
  string(0) ""
  ["javascript"]=>
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
  ["css"]=>
  string(1) "0"
  ["cssversion"]=>
  string(1) "0"
  ["supportscss"]=>
  string(0) ""
  ["aol"]=>
  string(0) ""
  ["aolversion"]=>
  string(1) "0"
}
array(34) {
  ["browser_name_regex"]=>
  string(6) "~^.*$~"
  ["browser_name_pattern"]=>
  string(1) "*"
  ["browser"]=>
  string(15) "Default Browser"
  ["version"]=>
  string(1) "0"
  ["majorver"]=>
  string(1) "0"
  ["minorver"]=>
  string(1) "0"
  ["platform"]=>
  string(7) "unknown"
  ["alpha"]=>
  string(0) ""
  ["beta"]=>
  string(0) ""
  ["win16"]=>
  string(0) ""
  ["win32"]=>
  string(0) ""
  ["win64"]=>
  string(0) ""
  ["frames"]=>
  string(1) "1"
  ["iframes"]=>
  string(0) ""
  ["tables"]=>
  string(1) "1"
  ["cookies"]=>
  string(0) ""
  ["backgroundsounds"]=>
  string(0) ""
  ["authenticodeupdate"]=>
  string(1) "0"
  ["cdf"]=>
  string(0) ""
  ["vbscript"]=>
  string(0) ""
  ["javaapplets"]=>
  string(0) ""
  ["javascript"]=>
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
  ["css"]=>
  string(1) "0"
  ["cssversion"]=>
  string(1) "0"
  ["supportscss"]=>
  string(0) ""
  ["aol"]=>
  string(0) ""
  ["aolversion"]=>
  string(1) "0"
}
array(34) {
  ["browser_name_regex"]=>
  string(6) "~^.*$~"
  ["browser_name_pattern"]=>
  string(1) "*"
  ["browser"]=>
  string(15) "Default Browser"
  ["version"]=>
  string(1) "0"
  ["majorver"]=>
  string(1) "0"
  ["minorver"]=>
  string(1) "0"
  ["platform"]=>
  string(7) "unknown"
  ["alpha"]=>
  string(0) ""
  ["beta"]=>
  string(0) ""
  ["win16"]=>
  string(0) ""
  ["win32"]=>
  string(0) ""
  ["win64"]=>
  string(0) ""
  ["frames"]=>
  string(1) "1"
  ["iframes"]=>
  string(0) ""
  ["tables"]=>
  string(1) "1"
  ["cookies"]=>
  string(0) ""
  ["backgroundsounds"]=>
  string(0) ""
  ["authenticodeupdate"]=>
  string(1) "0"
  ["cdf"]=>
  string(0) ""
  ["vbscript"]=>
  string(0) ""
  ["javaapplets"]=>
  string(0) ""
  ["javascript"]=>
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
  ["css"]=>
  string(1) "0"
  ["cssversion"]=>
  string(1) "0"
  ["supportscss"]=>
  string(0) ""
  ["aol"]=>
  string(0) ""
  ["aolversion"]=>
  string(1) "0"
}
array(35) {
  ["browser_name_regex"]=>
  string(99) "~^mozilla/5\.0 \(windows; .; windows nt 6\.0; .*\) applewebkit/.* \(.*\) version/3\.0.* safari/.*$~"
  ["browser_name_pattern"]=>
  string(83) "Mozilla/5.0 (Windows; ?; Windows NT 6.0; *) AppleWebKit/* (*) Version/3.0* Safari/*"
  ["parent"]=>
  string(10) "Safari 3.0"
  ["platform"]=>
  string(8) "WinVista"
  ["browser"]=>
  string(6) "Safari"
  ["version"]=>
  string(3) "3.0"
  ["majorver"]=>
  string(1) "3"
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
  ["minorver"]=>
  string(1) "0"
  ["alpha"]=>
  string(0) ""
  ["beta"]=>
  string(0) ""
  ["win16"]=>
  string(0) ""
  ["win32"]=>
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
array(35) {
  ["browser_name_regex"]=>
  string(99) "~^mozilla/.* \(iphone; .; .*mac os x.*\) applewebkit/.* \(.*\) version/.*\..* mobile/.* safari/.*$~"
  ["browser_name_pattern"]=>
  string(81) "Mozilla/* (iPhone; ?; *Mac OS X*) AppleWebKit/* (*) Version/*.* Mobile/* Safari/*"
  ["parent"]=>
  string(6) "iPhone"
  ["browser"]=>
  string(6) "Safari"
  ["platform"]=>
  string(6) "MacOSX"
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
  ["wap"]=>
  string(1) "1"
  ["ismobiledevice"]=>
  string(1) "1"
  ["css"]=>
  string(1) "2"
  ["cssversion"]=>
  string(1) "2"
  ["supportscss"]=>
  string(1) "1"
  ["version"]=>
  string(1) "0"
  ["majorver"]=>
  string(1) "0"
  ["minorver"]=>
  string(1) "0"
  ["alpha"]=>
  string(0) ""
  ["beta"]=>
  string(0) ""
  ["win16"]=>
  string(0) ""
  ["win32"]=>
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
  ["issyndicationreader"]=>
  string(0) ""
  ["crawler"]=>
  string(0) ""
  ["aol"]=>
  string(0) ""
  ["aolversion"]=>
  string(1) "0"
}
array(35) {
  ["browser_name_regex"]=>
  string(99) "~^mozilla/5\.0 \(ipod; u; .*mac os x; en\) applewebkit/.* \(.*\) version/3\.0 mobile/.* safari/.*$~"
  ["browser_name_pattern"]=>
  string(84) "Mozilla/5.0 (iPod; U; *Mac OS X; en) AppleWebKit/* (*) Version/3.0 Mobile/* Safari/*"
  ["parent"]=>
  string(4) "iPod"
  ["version"]=>
  string(3) "3.0"
  ["majorver"]=>
  string(1) "3"
  ["minorver"]=>
  string(1) "0"
  ["platform"]=>
  string(6) "MacOSX"
  ["browser"]=>
  string(4) "iPod"
  ["cookies"]=>
  string(1) "1"
  ["alpha"]=>
  string(0) ""
  ["beta"]=>
  string(0) ""
  ["win16"]=>
  string(0) ""
  ["win32"]=>
  string(0) ""
  ["win64"]=>
  string(0) ""
  ["frames"]=>
  string(0) ""
  ["iframes"]=>
  string(0) ""
  ["tables"]=>
  string(0) ""
  ["backgroundsounds"]=>
  string(0) ""
  ["authenticodeupdate"]=>
  string(0) ""
  ["cdf"]=>
  string(0) ""
  ["vbscript"]=>
  string(0) ""
  ["javaapplets"]=>
  string(0) ""
  ["javascript"]=>
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
  ["css"]=>
  string(1) "0"
  ["cssversion"]=>
  string(1) "0"
  ["supportscss"]=>
  string(0) ""
  ["aol"]=>
  string(0) ""
  ["aolversion"]=>
  string(1) "0"
}
===DONE===
