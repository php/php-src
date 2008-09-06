--TEST--
Test get_browser() function : basic functionality 
--INI--
browscap=ext/standard/tests/misc/browscap.ini
unicode.runtime_encoding = utf-8
unicode.output_encoding = utf-8
--SKIPIF--
<?php
	/**
	 * Basic check, it would be pretty much coincidence if there's
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

$browsers = include dirname(__FILE__) . DIRECTORY_SEPARATOR . 'browsernames.php';

/* Needed for the unicode_decode() call later */
unicode_set_error_mode( TO_UNICODE, U_CONV_ERROR_ESCAPE_UNICODE );

echo "*** Testing get_browser() : basic functionality ***\n";

for( $x = 0; $x < 20; $x++ ) {
	$result = @get_browser( $browsers[$x], true );
	foreach( $result as $k => $val ) {
		$result[$k] = unicode_decode( $val, 'UTF-8' );
	}
	var_dump( $result );
}

?>
===DONE===
--EXPECTF--
*** Testing get_browser() : basic functionality ***
array(35) {
  ["browser_name_regex"]=>
  unicode(47) "%XA7^opera/7\.1.* \(windows nt 5\.1; .\).*$%XA7"
  ["browser_name_pattern"]=>
  unicode(31) "Opera/7.1* (Windows NT 5.1; ?)*"
  ["parent"]=>
  unicode(9) "Opera 7.1"
  ["platform"]=>
  unicode(5) "WinXP"
  ["win32"]=>
  unicode(1) "1"
  ["browser"]=>
  unicode(5) "Opera"
  ["version"]=>
  unicode(3) "7.1"
  ["majorver"]=>
  unicode(1) "7"
  ["minorver"]=>
  unicode(1) "1"
  ["frames"]=>
  unicode(1) "1"
  ["iframes"]=>
  unicode(1) "1"
  ["tables"]=>
  unicode(1) "1"
  ["cookies"]=>
  unicode(1) "1"
  ["backgroundsounds"]=>
  unicode(1) "1"
  ["javaapplets"]=>
  unicode(1) "1"
  ["javascript"]=>
  unicode(1) "1"
  ["css"]=>
  unicode(1) "2"
  ["cssversion"]=>
  unicode(1) "2"
  ["supportscss"]=>
  unicode(1) "1"
  ["alpha"]=>
  unicode(0) ""
  ["beta"]=>
  unicode(0) ""
  ["win16"]=>
  unicode(0) ""
  ["win64"]=>
  unicode(0) ""
  ["authenticodeupdate"]=>
  unicode(0) ""
  ["cdf"]=>
  unicode(0) ""
  ["vbscript"]=>
  unicode(0) ""
  ["activexcontrols"]=>
  unicode(0) ""
  ["stripper"]=>
  unicode(0) ""
  ["isbanned"]=>
  unicode(0) ""
  ["wap"]=>
  unicode(0) ""
  ["ismobiledevice"]=>
  unicode(0) ""
  ["issyndicationreader"]=>
  unicode(0) ""
  ["crawler"]=>
  unicode(0) ""
  ["aol"]=>
  unicode(0) ""
  ["aolversion"]=>
  unicode(1) "0"
}
array(35) {
  ["browser_name_regex"]=>
  unicode(89) "%XA7^mozilla/4\.0 \(compatible; msie 6\.0; msie 5\.5; windows nt 5\.0\) opera 7\.0.*$%XA7"
  ["browser_name_pattern"]=>
  unicode(71) "Mozilla/4.0 (compatible; MSIE 6.0; MSIE 5.5; Windows NT 5.0) Opera 7.0*"
  ["parent"]=>
  unicode(9) "Opera 7.0"
  ["platform"]=>
  unicode(7) "Win2000"
  ["win32"]=>
  unicode(1) "1"
  ["browser"]=>
  unicode(5) "Opera"
  ["version"]=>
  unicode(3) "7.0"
  ["majorver"]=>
  unicode(1) "7"
  ["frames"]=>
  unicode(1) "1"
  ["iframes"]=>
  unicode(1) "1"
  ["tables"]=>
  unicode(1) "1"
  ["cookies"]=>
  unicode(1) "1"
  ["backgroundsounds"]=>
  unicode(1) "1"
  ["javaapplets"]=>
  unicode(1) "1"
  ["javascript"]=>
  unicode(1) "1"
  ["css"]=>
  unicode(1) "2"
  ["cssversion"]=>
  unicode(1) "2"
  ["supportscss"]=>
  unicode(1) "1"
  ["minorver"]=>
  unicode(1) "0"
  ["alpha"]=>
  unicode(0) ""
  ["beta"]=>
  unicode(0) ""
  ["win16"]=>
  unicode(0) ""
  ["win64"]=>
  unicode(0) ""
  ["authenticodeupdate"]=>
  unicode(0) ""
  ["cdf"]=>
  unicode(0) ""
  ["vbscript"]=>
  unicode(0) ""
  ["activexcontrols"]=>
  unicode(0) ""
  ["stripper"]=>
  unicode(0) ""
  ["isbanned"]=>
  unicode(0) ""
  ["wap"]=>
  unicode(0) ""
  ["ismobiledevice"]=>
  unicode(0) ""
  ["issyndicationreader"]=>
  unicode(0) ""
  ["crawler"]=>
  unicode(0) ""
  ["aol"]=>
  unicode(0) ""
  ["aolversion"]=>
  unicode(1) "0"
}
array(35) {
  ["browser_name_regex"]=>
  unicode(89) "%XA7^mozilla/4\.0 \(compatible; msie 6\.0; msie 5\.5; windows nt 4\.0\) opera 7\.0.*$%XA7"
  ["browser_name_pattern"]=>
  unicode(71) "Mozilla/4.0 (compatible; MSIE 6.0; MSIE 5.5; Windows NT 4.0) Opera 7.0*"
  ["parent"]=>
  unicode(9) "Opera 7.0"
  ["platform"]=>
  unicode(5) "WinNT"
  ["win32"]=>
  unicode(1) "1"
  ["browser"]=>
  unicode(5) "Opera"
  ["version"]=>
  unicode(3) "7.0"
  ["majorver"]=>
  unicode(1) "7"
  ["frames"]=>
  unicode(1) "1"
  ["iframes"]=>
  unicode(1) "1"
  ["tables"]=>
  unicode(1) "1"
  ["cookies"]=>
  unicode(1) "1"
  ["backgroundsounds"]=>
  unicode(1) "1"
  ["javaapplets"]=>
  unicode(1) "1"
  ["javascript"]=>
  unicode(1) "1"
  ["css"]=>
  unicode(1) "2"
  ["cssversion"]=>
  unicode(1) "2"
  ["supportscss"]=>
  unicode(1) "1"
  ["minorver"]=>
  unicode(1) "0"
  ["alpha"]=>
  unicode(0) ""
  ["beta"]=>
  unicode(0) ""
  ["win16"]=>
  unicode(0) ""
  ["win64"]=>
  unicode(0) ""
  ["authenticodeupdate"]=>
  unicode(0) ""
  ["cdf"]=>
  unicode(0) ""
  ["vbscript"]=>
  unicode(0) ""
  ["activexcontrols"]=>
  unicode(0) ""
  ["stripper"]=>
  unicode(0) ""
  ["isbanned"]=>
  unicode(0) ""
  ["wap"]=>
  unicode(0) ""
  ["ismobiledevice"]=>
  unicode(0) ""
  ["issyndicationreader"]=>
  unicode(0) ""
  ["crawler"]=>
  unicode(0) ""
  ["aol"]=>
  unicode(0) ""
  ["aolversion"]=>
  unicode(1) "0"
}
array(35) {
  ["browser_name_regex"]=>
  unicode(75) "%XA7^mozilla/4\.0 \(compatible; msie 5\.0; windows 2000\) opera 6\.0.*$%XA7"
  ["browser_name_pattern"]=>
  unicode(59) "Mozilla/4.0 (compatible; MSIE 5.0; Windows 2000) Opera 6.0*"
  ["parent"]=>
  unicode(9) "Opera 6.0"
  ["platform"]=>
  unicode(7) "Win2000"
  ["win32"]=>
  unicode(1) "1"
  ["browser"]=>
  unicode(5) "Opera"
  ["version"]=>
  unicode(3) "6.0"
  ["majorver"]=>
  unicode(1) "6"
  ["frames"]=>
  unicode(1) "1"
  ["iframes"]=>
  unicode(1) "1"
  ["tables"]=>
  unicode(1) "1"
  ["cookies"]=>
  unicode(1) "1"
  ["backgroundsounds"]=>
  unicode(1) "1"
  ["javaapplets"]=>
  unicode(1) "1"
  ["javascript"]=>
  unicode(1) "1"
  ["css"]=>
  unicode(1) "1"
  ["cssversion"]=>
  unicode(1) "1"
  ["supportscss"]=>
  unicode(1) "1"
  ["minorver"]=>
  unicode(1) "0"
  ["alpha"]=>
  unicode(0) ""
  ["beta"]=>
  unicode(0) ""
  ["win16"]=>
  unicode(0) ""
  ["win64"]=>
  unicode(0) ""
  ["authenticodeupdate"]=>
  unicode(0) ""
  ["cdf"]=>
  unicode(0) ""
  ["vbscript"]=>
  unicode(0) ""
  ["activexcontrols"]=>
  unicode(0) ""
  ["stripper"]=>
  unicode(0) ""
  ["isbanned"]=>
  unicode(0) ""
  ["wap"]=>
  unicode(0) ""
  ["ismobiledevice"]=>
  unicode(0) ""
  ["issyndicationreader"]=>
  unicode(0) ""
  ["crawler"]=>
  unicode(0) ""
  ["aol"]=>
  unicode(0) ""
  ["aolversion"]=>
  unicode(1) "0"
}
array(35) {
  ["browser_name_regex"]=>
  unicode(74) "%XA7^mozilla/4\.0 \(compatible; msie 5\.0; mac_powerpc\) opera 5\.0.*$%XA7"
  ["browser_name_pattern"]=>
  unicode(58) "Mozilla/4.0 (compatible; MSIE 5.0; Mac_PowerPC) Opera 5.0*"
  ["parent"]=>
  unicode(9) "Opera 5.0"
  ["platform"]=>
  unicode(6) "MacPPC"
  ["browser"]=>
  unicode(5) "Opera"
  ["version"]=>
  unicode(3) "5.0"
  ["majorver"]=>
  unicode(1) "5"
  ["frames"]=>
  unicode(1) "1"
  ["tables"]=>
  unicode(1) "1"
  ["cookies"]=>
  unicode(1) "1"
  ["backgroundsounds"]=>
  unicode(1) "1"
  ["javaapplets"]=>
  unicode(1) "1"
  ["javascript"]=>
  unicode(1) "1"
  ["css"]=>
  unicode(1) "1"
  ["cssversion"]=>
  unicode(1) "1"
  ["supportscss"]=>
  unicode(1) "1"
  ["minorver"]=>
  unicode(1) "0"
  ["alpha"]=>
  unicode(0) ""
  ["beta"]=>
  unicode(0) ""
  ["win16"]=>
  unicode(0) ""
  ["win32"]=>
  unicode(0) ""
  ["win64"]=>
  unicode(0) ""
  ["iframes"]=>
  unicode(0) ""
  ["authenticodeupdate"]=>
  unicode(0) ""
  ["cdf"]=>
  unicode(0) ""
  ["vbscript"]=>
  unicode(0) ""
  ["activexcontrols"]=>
  unicode(0) ""
  ["stripper"]=>
  unicode(0) ""
  ["isbanned"]=>
  unicode(0) ""
  ["wap"]=>
  unicode(0) ""
  ["ismobiledevice"]=>
  unicode(0) ""
  ["issyndicationreader"]=>
  unicode(0) ""
  ["crawler"]=>
  unicode(0) ""
  ["aol"]=>
  unicode(0) ""
  ["aolversion"]=>
  unicode(1) "0"
}
array(35) {
  ["browser_name_regex"]=>
  unicode(71) "%XA7^mozilla/4\.0 \(compatible; msie 6\.0; .*windows nt 5\.1.*\).*$%XA7"
  ["browser_name_pattern"]=>
  unicode(53) "Mozilla/4.0 (compatible; MSIE 6.0; *Windows NT 5.1*)*"
  ["parent"]=>
  unicode(6) "IE 6.0"
  ["platform"]=>
  unicode(5) "WinXP"
  ["browser"]=>
  unicode(2) "IE"
  ["version"]=>
  unicode(3) "6.0"
  ["majorver"]=>
  unicode(1) "6"
  ["win32"]=>
  unicode(1) "1"
  ["frames"]=>
  unicode(1) "1"
  ["iframes"]=>
  unicode(1) "1"
  ["tables"]=>
  unicode(1) "1"
  ["cookies"]=>
  unicode(1) "1"
  ["backgroundsounds"]=>
  unicode(1) "1"
  ["cdf"]=>
  unicode(1) "1"
  ["vbscript"]=>
  unicode(1) "1"
  ["javaapplets"]=>
  unicode(1) "1"
  ["javascript"]=>
  unicode(1) "1"
  ["activexcontrols"]=>
  unicode(1) "1"
  ["css"]=>
  unicode(1) "2"
  ["cssversion"]=>
  unicode(1) "2"
  ["supportscss"]=>
  unicode(1) "1"
  ["minorver"]=>
  unicode(1) "0"
  ["alpha"]=>
  unicode(0) ""
  ["beta"]=>
  unicode(0) ""
  ["win16"]=>
  unicode(0) ""
  ["win64"]=>
  unicode(0) ""
  ["authenticodeupdate"]=>
  unicode(0) ""
  ["stripper"]=>
  unicode(0) ""
  ["isbanned"]=>
  unicode(0) ""
  ["wap"]=>
  unicode(0) ""
  ["ismobiledevice"]=>
  unicode(0) ""
  ["issyndicationreader"]=>
  unicode(0) ""
  ["crawler"]=>
  unicode(0) ""
  ["aol"]=>
  unicode(0) ""
  ["aolversion"]=>
  unicode(1) "0"
}
array(35) {
  ["browser_name_regex"]=>
  unicode(46) "%XA7^opera/9\.0.* \(windows nt 5\.1.*\).*$%XA7"
  ["browser_name_pattern"]=>
  unicode(29) "Opera/9.0* (Windows NT 5.1*)*"
  ["parent"]=>
  unicode(9) "Opera 9.0"
  ["platform"]=>
  unicode(5) "WinXP"
  ["win32"]=>
  unicode(1) "1"
  ["browser"]=>
  unicode(5) "Opera"
  ["version"]=>
  unicode(3) "9.0"
  ["majorver"]=>
  unicode(1) "9"
  ["frames"]=>
  unicode(1) "1"
  ["iframes"]=>
  unicode(1) "1"
  ["tables"]=>
  unicode(1) "1"
  ["cookies"]=>
  unicode(1) "1"
  ["backgroundsounds"]=>
  unicode(1) "1"
  ["javaapplets"]=>
  unicode(1) "1"
  ["javascript"]=>
  unicode(1) "1"
  ["css"]=>
  unicode(1) "2"
  ["cssversion"]=>
  unicode(1) "2"
  ["supportscss"]=>
  unicode(1) "1"
  ["minorver"]=>
  unicode(1) "0"
  ["alpha"]=>
  unicode(0) ""
  ["beta"]=>
  unicode(0) ""
  ["win16"]=>
  unicode(0) ""
  ["win64"]=>
  unicode(0) ""
  ["authenticodeupdate"]=>
  unicode(0) ""
  ["cdf"]=>
  unicode(0) ""
  ["vbscript"]=>
  unicode(0) ""
  ["activexcontrols"]=>
  unicode(0) ""
  ["stripper"]=>
  unicode(0) ""
  ["isbanned"]=>
  unicode(0) ""
  ["wap"]=>
  unicode(0) ""
  ["ismobiledevice"]=>
  unicode(0) ""
  ["issyndicationreader"]=>
  unicode(0) ""
  ["crawler"]=>
  unicode(0) ""
  ["aol"]=>
  unicode(0) ""
  ["aolversion"]=>
  unicode(1) "0"
}
array(35) {
  ["browser_name_regex"]=>
  unicode(53) "%XA7^opera/9\.2.* \(macintosh; .*mac os x;.*\).*$%XA7"
  ["browser_name_pattern"]=>
  unicode(36) "Opera/9.2* (Macintosh; *Mac OS X;*)*"
  ["parent"]=>
  unicode(9) "Opera 9.2"
  ["platform"]=>
  unicode(6) "MacOSX"
  ["browser"]=>
  unicode(5) "Opera"
  ["version"]=>
  unicode(3) "9.2"
  ["majorver"]=>
  unicode(1) "9"
  ["minorver"]=>
  unicode(1) "2"
  ["frames"]=>
  unicode(1) "1"
  ["iframes"]=>
  unicode(1) "1"
  ["tables"]=>
  unicode(1) "1"
  ["cookies"]=>
  unicode(1) "1"
  ["backgroundsounds"]=>
  unicode(1) "1"
  ["javaapplets"]=>
  unicode(1) "1"
  ["javascript"]=>
  unicode(1) "1"
  ["css"]=>
  unicode(1) "2"
  ["cssversion"]=>
  unicode(1) "2"
  ["supportscss"]=>
  unicode(1) "1"
  ["alpha"]=>
  unicode(0) ""
  ["beta"]=>
  unicode(0) ""
  ["win16"]=>
  unicode(0) ""
  ["win32"]=>
  unicode(0) ""
  ["win64"]=>
  unicode(0) ""
  ["authenticodeupdate"]=>
  unicode(0) ""
  ["cdf"]=>
  unicode(0) ""
  ["vbscript"]=>
  unicode(0) ""
  ["activexcontrols"]=>
  unicode(0) ""
  ["stripper"]=>
  unicode(0) ""
  ["isbanned"]=>
  unicode(0) ""
  ["wap"]=>
  unicode(0) ""
  ["ismobiledevice"]=>
  unicode(0) ""
  ["issyndicationreader"]=>
  unicode(0) ""
  ["crawler"]=>
  unicode(0) ""
  ["aol"]=>
  unicode(0) ""
  ["aolversion"]=>
  unicode(1) "0"
}
array(35) {
  ["browser_name_regex"]=>
  unicode(71) "%XA7^mozilla/4\.0 \(compatible; msie 6\.0; .*windows nt 5\.1.*\).*$%XA7"
  ["browser_name_pattern"]=>
  unicode(53) "Mozilla/4.0 (compatible; MSIE 6.0; *Windows NT 5.1*)*"
  ["parent"]=>
  unicode(6) "IE 6.0"
  ["platform"]=>
  unicode(5) "WinXP"
  ["browser"]=>
  unicode(2) "IE"
  ["version"]=>
  unicode(3) "6.0"
  ["majorver"]=>
  unicode(1) "6"
  ["win32"]=>
  unicode(1) "1"
  ["frames"]=>
  unicode(1) "1"
  ["iframes"]=>
  unicode(1) "1"
  ["tables"]=>
  unicode(1) "1"
  ["cookies"]=>
  unicode(1) "1"
  ["backgroundsounds"]=>
  unicode(1) "1"
  ["cdf"]=>
  unicode(1) "1"
  ["vbscript"]=>
  unicode(1) "1"
  ["javaapplets"]=>
  unicode(1) "1"
  ["javascript"]=>
  unicode(1) "1"
  ["activexcontrols"]=>
  unicode(1) "1"
  ["css"]=>
  unicode(1) "2"
  ["cssversion"]=>
  unicode(1) "2"
  ["supportscss"]=>
  unicode(1) "1"
  ["minorver"]=>
  unicode(1) "0"
  ["alpha"]=>
  unicode(0) ""
  ["beta"]=>
  unicode(0) ""
  ["win16"]=>
  unicode(0) ""
  ["win64"]=>
  unicode(0) ""
  ["authenticodeupdate"]=>
  unicode(0) ""
  ["stripper"]=>
  unicode(0) ""
  ["isbanned"]=>
  unicode(0) ""
  ["wap"]=>
  unicode(0) ""
  ["ismobiledevice"]=>
  unicode(0) ""
  ["issyndicationreader"]=>
  unicode(0) ""
  ["crawler"]=>
  unicode(0) ""
  ["aol"]=>
  unicode(0) ""
  ["aolversion"]=>
  unicode(1) "0"
}
array(35) {
  ["browser_name_regex"]=>
  unicode(71) "%XA7^mozilla/4\.0 \(compatible; msie 6\.0; .*windows nt 5\.1.*\).*$%XA7"
  ["browser_name_pattern"]=>
  unicode(53) "Mozilla/4.0 (compatible; MSIE 6.0; *Windows NT 5.1*)*"
  ["parent"]=>
  unicode(6) "IE 6.0"
  ["platform"]=>
  unicode(5) "WinXP"
  ["browser"]=>
  unicode(2) "IE"
  ["version"]=>
  unicode(3) "6.0"
  ["majorver"]=>
  unicode(1) "6"
  ["win32"]=>
  unicode(1) "1"
  ["frames"]=>
  unicode(1) "1"
  ["iframes"]=>
  unicode(1) "1"
  ["tables"]=>
  unicode(1) "1"
  ["cookies"]=>
  unicode(1) "1"
  ["backgroundsounds"]=>
  unicode(1) "1"
  ["cdf"]=>
  unicode(1) "1"
  ["vbscript"]=>
  unicode(1) "1"
  ["javaapplets"]=>
  unicode(1) "1"
  ["javascript"]=>
  unicode(1) "1"
  ["activexcontrols"]=>
  unicode(1) "1"
  ["css"]=>
  unicode(1) "2"
  ["cssversion"]=>
  unicode(1) "2"
  ["supportscss"]=>
  unicode(1) "1"
  ["minorver"]=>
  unicode(1) "0"
  ["alpha"]=>
  unicode(0) ""
  ["beta"]=>
  unicode(0) ""
  ["win16"]=>
  unicode(0) ""
  ["win64"]=>
  unicode(0) ""
  ["authenticodeupdate"]=>
  unicode(0) ""
  ["stripper"]=>
  unicode(0) ""
  ["isbanned"]=>
  unicode(0) ""
  ["wap"]=>
  unicode(0) ""
  ["ismobiledevice"]=>
  unicode(0) ""
  ["issyndicationreader"]=>
  unicode(0) ""
  ["crawler"]=>
  unicode(0) ""
  ["aol"]=>
  unicode(0) ""
  ["aolversion"]=>
  unicode(1) "0"
}
array(35) {
  ["browser_name_regex"]=>
  unicode(65) "%XA7^mozilla/.\.. \(compatible; msie 5\.2.*; .*mac_powerpc.*$%XA7"
  ["browser_name_pattern"]=>
  unicode(49) "Mozilla/?.? (compatible; MSIE 5.2*; *Mac_PowerPC*"
  ["parent"]=>
  unicode(6) "IE Mac"
  ["version"]=>
  unicode(3) "5.2"
  ["majorver"]=>
  unicode(1) "5"
  ["minorver"]=>
  unicode(1) "2"
  ["browser"]=>
  unicode(2) "IE"
  ["platform"]=>
  unicode(6) "MacPPC"
  ["frames"]=>
  unicode(1) "1"
  ["iframes"]=>
  unicode(1) "1"
  ["tables"]=>
  unicode(1) "1"
  ["cookies"]=>
  unicode(1) "1"
  ["backgroundsounds"]=>
  unicode(1) "1"
  ["cdf"]=>
  unicode(1) "1"
  ["javaapplets"]=>
  unicode(1) "1"
  ["javascript"]=>
  unicode(1) "1"
  ["css"]=>
  unicode(1) "1"
  ["cssversion"]=>
  unicode(1) "1"
  ["supportscss"]=>
  unicode(1) "1"
  ["alpha"]=>
  unicode(0) ""
  ["beta"]=>
  unicode(0) ""
  ["win16"]=>
  unicode(0) ""
  ["win32"]=>
  unicode(0) ""
  ["win64"]=>
  unicode(0) ""
  ["authenticodeupdate"]=>
  unicode(0) ""
  ["vbscript"]=>
  unicode(0) ""
  ["activexcontrols"]=>
  unicode(0) ""
  ["stripper"]=>
  unicode(0) ""
  ["isbanned"]=>
  unicode(0) ""
  ["wap"]=>
  unicode(0) ""
  ["ismobiledevice"]=>
  unicode(0) ""
  ["issyndicationreader"]=>
  unicode(0) ""
  ["crawler"]=>
  unicode(0) ""
  ["aol"]=>
  unicode(0) ""
  ["aolversion"]=>
  unicode(1) "0"
}
array(35) {
  ["browser_name_regex"]=>
  unicode(65) "%XA7^mozilla/.\.. \(compatible; msie 5\.0.*; .*mac_powerpc.*$%XA7"
  ["browser_name_pattern"]=>
  unicode(49) "Mozilla/?.? (compatible; MSIE 5.0*; *Mac_PowerPC*"
  ["parent"]=>
  unicode(6) "IE Mac"
  ["version"]=>
  unicode(3) "5.0"
  ["majorver"]=>
  unicode(1) "5"
  ["minorver"]=>
  unicode(1) "0"
  ["browser"]=>
  unicode(2) "IE"
  ["platform"]=>
  unicode(6) "MacPPC"
  ["frames"]=>
  unicode(1) "1"
  ["iframes"]=>
  unicode(1) "1"
  ["tables"]=>
  unicode(1) "1"
  ["cookies"]=>
  unicode(1) "1"
  ["backgroundsounds"]=>
  unicode(1) "1"
  ["cdf"]=>
  unicode(1) "1"
  ["javaapplets"]=>
  unicode(1) "1"
  ["javascript"]=>
  unicode(1) "1"
  ["css"]=>
  unicode(1) "1"
  ["cssversion"]=>
  unicode(1) "1"
  ["supportscss"]=>
  unicode(1) "1"
  ["alpha"]=>
  unicode(0) ""
  ["beta"]=>
  unicode(0) ""
  ["win16"]=>
  unicode(0) ""
  ["win32"]=>
  unicode(0) ""
  ["win64"]=>
  unicode(0) ""
  ["authenticodeupdate"]=>
  unicode(0) ""
  ["vbscript"]=>
  unicode(0) ""
  ["activexcontrols"]=>
  unicode(0) ""
  ["stripper"]=>
  unicode(0) ""
  ["isbanned"]=>
  unicode(0) ""
  ["wap"]=>
  unicode(0) ""
  ["ismobiledevice"]=>
  unicode(0) ""
  ["issyndicationreader"]=>
  unicode(0) ""
  ["crawler"]=>
  unicode(0) ""
  ["aol"]=>
  unicode(0) ""
  ["aolversion"]=>
  unicode(1) "0"
}
array(34) {
  ["browser_name_regex"]=>
  unicode(12) "%XA7^.*$%XA7"
  ["browser_name_pattern"]=>
  unicode(1) "*"
  ["browser"]=>
  unicode(15) "Default Browser"
  ["version"]=>
  unicode(1) "0"
  ["majorver"]=>
  unicode(1) "0"
  ["minorver"]=>
  unicode(1) "0"
  ["platform"]=>
  unicode(7) "unknown"
  ["alpha"]=>
  unicode(0) ""
  ["beta"]=>
  unicode(0) ""
  ["win16"]=>
  unicode(0) ""
  ["win32"]=>
  unicode(0) ""
  ["win64"]=>
  unicode(0) ""
  ["frames"]=>
  unicode(1) "1"
  ["iframes"]=>
  unicode(0) ""
  ["tables"]=>
  unicode(1) "1"
  ["cookies"]=>
  unicode(0) ""
  ["backgroundsounds"]=>
  unicode(0) ""
  ["authenticodeupdate"]=>
  unicode(1) "0"
  ["cdf"]=>
  unicode(0) ""
  ["vbscript"]=>
  unicode(0) ""
  ["javaapplets"]=>
  unicode(0) ""
  ["javascript"]=>
  unicode(0) ""
  ["activexcontrols"]=>
  unicode(0) ""
  ["stripper"]=>
  unicode(0) ""
  ["isbanned"]=>
  unicode(0) ""
  ["wap"]=>
  unicode(0) ""
  ["ismobiledevice"]=>
  unicode(0) ""
  ["issyndicationreader"]=>
  unicode(0) ""
  ["crawler"]=>
  unicode(0) ""
  ["css"]=>
  unicode(1) "0"
  ["cssversion"]=>
  unicode(1) "0"
  ["supportscss"]=>
  unicode(0) ""
  ["aol"]=>
  unicode(0) ""
  ["aolversion"]=>
  unicode(1) "0"
}
array(34) {
  ["browser_name_regex"]=>
  unicode(12) "%XA7^.*$%XA7"
  ["browser_name_pattern"]=>
  unicode(1) "*"
  ["browser"]=>
  unicode(15) "Default Browser"
  ["version"]=>
  unicode(1) "0"
  ["majorver"]=>
  unicode(1) "0"
  ["minorver"]=>
  unicode(1) "0"
  ["platform"]=>
  unicode(7) "unknown"
  ["alpha"]=>
  unicode(0) ""
  ["beta"]=>
  unicode(0) ""
  ["win16"]=>
  unicode(0) ""
  ["win32"]=>
  unicode(0) ""
  ["win64"]=>
  unicode(0) ""
  ["frames"]=>
  unicode(1) "1"
  ["iframes"]=>
  unicode(0) ""
  ["tables"]=>
  unicode(1) "1"
  ["cookies"]=>
  unicode(0) ""
  ["backgroundsounds"]=>
  unicode(0) ""
  ["authenticodeupdate"]=>
  unicode(1) "0"
  ["cdf"]=>
  unicode(0) ""
  ["vbscript"]=>
  unicode(0) ""
  ["javaapplets"]=>
  unicode(0) ""
  ["javascript"]=>
  unicode(0) ""
  ["activexcontrols"]=>
  unicode(0) ""
  ["stripper"]=>
  unicode(0) ""
  ["isbanned"]=>
  unicode(0) ""
  ["wap"]=>
  unicode(0) ""
  ["ismobiledevice"]=>
  unicode(0) ""
  ["issyndicationreader"]=>
  unicode(0) ""
  ["crawler"]=>
  unicode(0) ""
  ["css"]=>
  unicode(1) "0"
  ["cssversion"]=>
  unicode(1) "0"
  ["supportscss"]=>
  unicode(0) ""
  ["aol"]=>
  unicode(0) ""
  ["aolversion"]=>
  unicode(1) "0"
}
array(34) {
  ["browser_name_regex"]=>
  unicode(12) "%XA7^.*$%XA7"
  ["browser_name_pattern"]=>
  unicode(1) "*"
  ["browser"]=>
  unicode(15) "Default Browser"
  ["version"]=>
  unicode(1) "0"
  ["majorver"]=>
  unicode(1) "0"
  ["minorver"]=>
  unicode(1) "0"
  ["platform"]=>
  unicode(7) "unknown"
  ["alpha"]=>
  unicode(0) ""
  ["beta"]=>
  unicode(0) ""
  ["win16"]=>
  unicode(0) ""
  ["win32"]=>
  unicode(0) ""
  ["win64"]=>
  unicode(0) ""
  ["frames"]=>
  unicode(1) "1"
  ["iframes"]=>
  unicode(0) ""
  ["tables"]=>
  unicode(1) "1"
  ["cookies"]=>
  unicode(0) ""
  ["backgroundsounds"]=>
  unicode(0) ""
  ["authenticodeupdate"]=>
  unicode(1) "0"
  ["cdf"]=>
  unicode(0) ""
  ["vbscript"]=>
  unicode(0) ""
  ["javaapplets"]=>
  unicode(0) ""
  ["javascript"]=>
  unicode(0) ""
  ["activexcontrols"]=>
  unicode(0) ""
  ["stripper"]=>
  unicode(0) ""
  ["isbanned"]=>
  unicode(0) ""
  ["wap"]=>
  unicode(0) ""
  ["ismobiledevice"]=>
  unicode(0) ""
  ["issyndicationreader"]=>
  unicode(0) ""
  ["crawler"]=>
  unicode(0) ""
  ["css"]=>
  unicode(1) "0"
  ["cssversion"]=>
  unicode(1) "0"
  ["supportscss"]=>
  unicode(0) ""
  ["aol"]=>
  unicode(0) ""
  ["aolversion"]=>
  unicode(1) "0"
}
array(34) {
  ["browser_name_regex"]=>
  unicode(12) "%XA7^.*$%XA7"
  ["browser_name_pattern"]=>
  unicode(1) "*"
  ["browser"]=>
  unicode(15) "Default Browser"
  ["version"]=>
  unicode(1) "0"
  ["majorver"]=>
  unicode(1) "0"
  ["minorver"]=>
  unicode(1) "0"
  ["platform"]=>
  unicode(7) "unknown"
  ["alpha"]=>
  unicode(0) ""
  ["beta"]=>
  unicode(0) ""
  ["win16"]=>
  unicode(0) ""
  ["win32"]=>
  unicode(0) ""
  ["win64"]=>
  unicode(0) ""
  ["frames"]=>
  unicode(1) "1"
  ["iframes"]=>
  unicode(0) ""
  ["tables"]=>
  unicode(1) "1"
  ["cookies"]=>
  unicode(0) ""
  ["backgroundsounds"]=>
  unicode(0) ""
  ["authenticodeupdate"]=>
  unicode(1) "0"
  ["cdf"]=>
  unicode(0) ""
  ["vbscript"]=>
  unicode(0) ""
  ["javaapplets"]=>
  unicode(0) ""
  ["javascript"]=>
  unicode(0) ""
  ["activexcontrols"]=>
  unicode(0) ""
  ["stripper"]=>
  unicode(0) ""
  ["isbanned"]=>
  unicode(0) ""
  ["wap"]=>
  unicode(0) ""
  ["ismobiledevice"]=>
  unicode(0) ""
  ["issyndicationreader"]=>
  unicode(0) ""
  ["crawler"]=>
  unicode(0) ""
  ["css"]=>
  unicode(1) "0"
  ["cssversion"]=>
  unicode(1) "0"
  ["supportscss"]=>
  unicode(0) ""
  ["aol"]=>
  unicode(0) ""
  ["aolversion"]=>
  unicode(1) "0"
}
array(34) {
  ["browser_name_regex"]=>
  unicode(12) "%XA7^.*$%XA7"
  ["browser_name_pattern"]=>
  unicode(1) "*"
  ["browser"]=>
  unicode(15) "Default Browser"
  ["version"]=>
  unicode(1) "0"
  ["majorver"]=>
  unicode(1) "0"
  ["minorver"]=>
  unicode(1) "0"
  ["platform"]=>
  unicode(7) "unknown"
  ["alpha"]=>
  unicode(0) ""
  ["beta"]=>
  unicode(0) ""
  ["win16"]=>
  unicode(0) ""
  ["win32"]=>
  unicode(0) ""
  ["win64"]=>
  unicode(0) ""
  ["frames"]=>
  unicode(1) "1"
  ["iframes"]=>
  unicode(0) ""
  ["tables"]=>
  unicode(1) "1"
  ["cookies"]=>
  unicode(0) ""
  ["backgroundsounds"]=>
  unicode(0) ""
  ["authenticodeupdate"]=>
  unicode(1) "0"
  ["cdf"]=>
  unicode(0) ""
  ["vbscript"]=>
  unicode(0) ""
  ["javaapplets"]=>
  unicode(0) ""
  ["javascript"]=>
  unicode(0) ""
  ["activexcontrols"]=>
  unicode(0) ""
  ["stripper"]=>
  unicode(0) ""
  ["isbanned"]=>
  unicode(0) ""
  ["wap"]=>
  unicode(0) ""
  ["ismobiledevice"]=>
  unicode(0) ""
  ["issyndicationreader"]=>
  unicode(0) ""
  ["crawler"]=>
  unicode(0) ""
  ["css"]=>
  unicode(1) "0"
  ["cssversion"]=>
  unicode(1) "0"
  ["supportscss"]=>
  unicode(0) ""
  ["aol"]=>
  unicode(0) ""
  ["aolversion"]=>
  unicode(1) "0"
}
array(35) {
  ["browser_name_regex"]=>
  unicode(105) "%XA7^mozilla/5\.0 \(windows; .; windows nt 6\.0; .*\) applewebkit/.* \(.*\) version/3\.0.* safari/.*$%XA7"
  ["browser_name_pattern"]=>
  unicode(83) "Mozilla/5.0 (Windows; ?; Windows NT 6.0; *) AppleWebKit/* (*) Version/3.0* Safari/*"
  ["parent"]=>
  unicode(10) "Safari 3.0"
  ["platform"]=>
  unicode(8) "WinVista"
  ["browser"]=>
  unicode(6) "Safari"
  ["version"]=>
  unicode(3) "3.0"
  ["majorver"]=>
  unicode(1) "3"
  ["frames"]=>
  unicode(1) "1"
  ["iframes"]=>
  unicode(1) "1"
  ["tables"]=>
  unicode(1) "1"
  ["cookies"]=>
  unicode(1) "1"
  ["backgroundsounds"]=>
  unicode(1) "1"
  ["javaapplets"]=>
  unicode(1) "1"
  ["javascript"]=>
  unicode(1) "1"
  ["css"]=>
  unicode(1) "2"
  ["cssversion"]=>
  unicode(1) "2"
  ["supportscss"]=>
  unicode(1) "1"
  ["minorver"]=>
  unicode(1) "0"
  ["alpha"]=>
  unicode(0) ""
  ["beta"]=>
  unicode(0) ""
  ["win16"]=>
  unicode(0) ""
  ["win32"]=>
  unicode(0) ""
  ["win64"]=>
  unicode(0) ""
  ["authenticodeupdate"]=>
  unicode(0) ""
  ["cdf"]=>
  unicode(0) ""
  ["vbscript"]=>
  unicode(0) ""
  ["activexcontrols"]=>
  unicode(0) ""
  ["stripper"]=>
  unicode(0) ""
  ["isbanned"]=>
  unicode(0) ""
  ["wap"]=>
  unicode(0) ""
  ["ismobiledevice"]=>
  unicode(0) ""
  ["issyndicationreader"]=>
  unicode(0) ""
  ["crawler"]=>
  unicode(0) ""
  ["aol"]=>
  unicode(0) ""
  ["aolversion"]=>
  unicode(1) "0"
}
array(35) {
  ["browser_name_regex"]=>
  unicode(105) "%XA7^mozilla/.* \(iphone; .; .*mac os x.*\) applewebkit/.* \(.*\) version/.*\..* mobile/.* safari/.*$%XA7"
  ["browser_name_pattern"]=>
  unicode(81) "Mozilla/* (iPhone; ?; *Mac OS X*) AppleWebKit/* (*) Version/*.* Mobile/* Safari/*"
  ["parent"]=>
  unicode(6) "iPhone"
  ["browser"]=>
  unicode(6) "Safari"
  ["platform"]=>
  unicode(6) "MacOSX"
  ["frames"]=>
  unicode(1) "1"
  ["iframes"]=>
  unicode(1) "1"
  ["tables"]=>
  unicode(1) "1"
  ["cookies"]=>
  unicode(1) "1"
  ["backgroundsounds"]=>
  unicode(1) "1"
  ["javaapplets"]=>
  unicode(1) "1"
  ["javascript"]=>
  unicode(1) "1"
  ["wap"]=>
  unicode(1) "1"
  ["ismobiledevice"]=>
  unicode(1) "1"
  ["css"]=>
  unicode(1) "2"
  ["cssversion"]=>
  unicode(1) "2"
  ["supportscss"]=>
  unicode(1) "1"
  ["version"]=>
  unicode(1) "0"
  ["majorver"]=>
  unicode(1) "0"
  ["minorver"]=>
  unicode(1) "0"
  ["alpha"]=>
  unicode(0) ""
  ["beta"]=>
  unicode(0) ""
  ["win16"]=>
  unicode(0) ""
  ["win32"]=>
  unicode(0) ""
  ["win64"]=>
  unicode(0) ""
  ["authenticodeupdate"]=>
  unicode(0) ""
  ["cdf"]=>
  unicode(0) ""
  ["vbscript"]=>
  unicode(0) ""
  ["activexcontrols"]=>
  unicode(0) ""
  ["stripper"]=>
  unicode(0) ""
  ["isbanned"]=>
  unicode(0) ""
  ["issyndicationreader"]=>
  unicode(0) ""
  ["crawler"]=>
  unicode(0) ""
  ["aol"]=>
  unicode(0) ""
  ["aolversion"]=>
  unicode(1) "0"
}
array(35) {
  ["browser_name_regex"]=>
  unicode(105) "%XA7^mozilla/5\.0 \(ipod; u; .*mac os x; en\) applewebkit/.* \(.*\) version/3\.0 mobile/.* safari/.*$%XA7"
  ["browser_name_pattern"]=>
  unicode(84) "Mozilla/5.0 (iPod; U; *Mac OS X; en) AppleWebKit/* (*) Version/3.0 Mobile/* Safari/*"
  ["parent"]=>
  unicode(4) "iPod"
  ["version"]=>
  unicode(3) "3.0"
  ["majorver"]=>
  unicode(1) "3"
  ["minorver"]=>
  unicode(1) "0"
  ["platform"]=>
  unicode(6) "MacOSX"
  ["browser"]=>
  unicode(4) "iPod"
  ["cookies"]=>
  unicode(1) "1"
  ["alpha"]=>
  unicode(0) ""
  ["beta"]=>
  unicode(0) ""
  ["win16"]=>
  unicode(0) ""
  ["win32"]=>
  unicode(0) ""
  ["win64"]=>
  unicode(0) ""
  ["frames"]=>
  unicode(0) ""
  ["iframes"]=>
  unicode(0) ""
  ["tables"]=>
  unicode(0) ""
  ["backgroundsounds"]=>
  unicode(0) ""
  ["authenticodeupdate"]=>
  unicode(0) ""
  ["cdf"]=>
  unicode(0) ""
  ["vbscript"]=>
  unicode(0) ""
  ["javaapplets"]=>
  unicode(0) ""
  ["javascript"]=>
  unicode(0) ""
  ["activexcontrols"]=>
  unicode(0) ""
  ["stripper"]=>
  unicode(0) ""
  ["isbanned"]=>
  unicode(0) ""
  ["wap"]=>
  unicode(0) ""
  ["ismobiledevice"]=>
  unicode(0) ""
  ["issyndicationreader"]=>
  unicode(0) ""
  ["crawler"]=>
  unicode(0) ""
  ["css"]=>
  unicode(1) "0"
  ["cssversion"]=>
  unicode(1) "0"
  ["supportscss"]=>
  unicode(0) ""
  ["aol"]=>
  unicode(0) ""
  ["aolversion"]=>
  unicode(1) "0"
}
===DONE===
