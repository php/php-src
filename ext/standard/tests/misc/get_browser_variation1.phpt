--TEST--
Test get_browser() function : variation with extra browscap definitions
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

for( $x = 20; $x < count($browsers); $x++ ) {
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
  unicode(100) "%XA7^mozilla/5\.0 \(macintosh; .; .*mac os x.*\) applewebkit/.* \(.*\) version/3\.1.* safari/.*$%XA7"
  ["browser_name_pattern"]=>
  unicode(78) "Mozilla/5.0 (Macintosh; ?; *Mac OS X*) AppleWebKit/* (*) Version/3.1* Safari/*"
  ["parent"]=>
  unicode(10) "Safari 3.1"
  ["platform"]=>
  unicode(6) "MacOSX"
  ["browser"]=>
  unicode(6) "Safari"
  ["version"]=>
  unicode(3) "3.1"
  ["majorver"]=>
  unicode(1) "3"
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
  unicode(73) "%XA7^mozilla/5\.0 \(macintosh; .*mac os x.*\) gecko/.* camino/1\.5.*$%XA7"
  ["browser_name_pattern"]=>
  unicode(55) "Mozilla/5.0 (Macintosh; *Mac OS X*) Gecko/* Camino/1.5*"
  ["parent"]=>
  unicode(6) "Camino"
  ["version"]=>
  unicode(3) "1.5"
  ["majorver"]=>
  unicode(1) "1"
  ["minorver"]=>
  unicode(1) "5"
  ["platform"]=>
  unicode(6) "MacOSX"
  ["browser"]=>
  unicode(6) "Camino"
  ["frames"]=>
  unicode(1) "1"
  ["iframes"]=>
  unicode(1) "1"
  ["tables"]=>
  unicode(1) "1"
  ["cookies"]=>
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
  ["backgroundsounds"]=>
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
  unicode(35) "%XA7^.*konqueror/3\.1.*linux.*$%XA7"
  ["browser_name_pattern"]=>
  unicode(21) "*Konqueror/3.1*Linux*"
  ["parent"]=>
  unicode(9) "Konqueror"
  ["version"]=>
  unicode(3) "3.1"
  ["majorver"]=>
  unicode(1) "3"
  ["minorver"]=>
  unicode(1) "1"
  ["browser"]=>
  unicode(9) "Konqueror"
  ["platform"]=>
  unicode(5) "Linux"
  ["frames"]=>
  unicode(1) "1"
  ["iframes"]=>
  unicode(1) "1"
  ["tables"]=>
  unicode(1) "1"
  ["cookies"]=>
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
  unicode(35) "%XA7^.*konqueror/3\.1.*linux.*$%XA7"
  ["browser_name_pattern"]=>
  unicode(21) "*Konqueror/3.1*Linux*"
  ["parent"]=>
  unicode(9) "Konqueror"
  ["version"]=>
  unicode(3) "3.1"
  ["majorver"]=>
  unicode(1) "3"
  ["minorver"]=>
  unicode(1) "1"
  ["browser"]=>
  unicode(9) "Konqueror"
  ["platform"]=>
  unicode(5) "Linux"
  ["frames"]=>
  unicode(1) "1"
  ["iframes"]=>
  unicode(1) "1"
  ["tables"]=>
  unicode(1) "1"
  ["cookies"]=>
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
  unicode(27) "%XA7^.*konqueror/2\..*$%XA7"
  ["browser_name_pattern"]=>
  unicode(14) "*Konqueror/2.*"
  ["parent"]=>
  unicode(9) "Konqueror"
  ["iframes"]=>
  unicode(0) ""
  ["browser"]=>
  unicode(9) "Konqueror"
  ["platform"]=>
  unicode(5) "Linux"
  ["frames"]=>
  unicode(1) "1"
  ["tables"]=>
  unicode(1) "1"
  ["cookies"]=>
  unicode(1) "1"
  ["javascript"]=>
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
  unicode(76) "%XA7^mozilla/5\.0 \(windows; .; windows nt 5\.1; .*rv:1\.4.*\) gecko/.*$%XA7"
  ["browser_name_pattern"]=>
  unicode(58) "Mozilla/5.0 (Windows; ?; Windows NT 5.1; *rv:1.4*) Gecko/*"
  ["parent"]=>
  unicode(11) "Mozilla 1.4"
  ["platform"]=>
  unicode(5) "WinXP"
  ["win32"]=>
  unicode(1) "1"
  ["browser"]=>
  unicode(7) "Mozilla"
  ["version"]=>
  unicode(3) "1.4"
  ["majorver"]=>
  unicode(1) "1"
  ["minorver"]=>
  unicode(1) "4"
  ["beta"]=>
  unicode(1) "1"
  ["frames"]=>
  unicode(1) "1"
  ["iframes"]=>
  unicode(1) "1"
  ["tables"]=>
  unicode(1) "1"
  ["cookies"]=>
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
  ["win16"]=>
  unicode(0) ""
  ["win64"]=>
  unicode(0) ""
  ["backgroundsounds"]=>
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
  unicode(63) "%XA7^mozilla/5\.0 \(x11; .*linux.*; .*rv:1\.7.*\) gecko/.*$%XA7"
  ["browser_name_pattern"]=>
  unicode(44) "Mozilla/5.0 (X11; *Linux*; *rv:1.7*) Gecko/*"
  ["parent"]=>
  unicode(11) "Mozilla 1.7"
  ["platform"]=>
  unicode(5) "Linux"
  ["browser"]=>
  unicode(7) "Mozilla"
  ["version"]=>
  unicode(3) "1.7"
  ["majorver"]=>
  unicode(1) "1"
  ["minorver"]=>
  unicode(1) "7"
  ["beta"]=>
  unicode(1) "1"
  ["frames"]=>
  unicode(1) "1"
  ["iframes"]=>
  unicode(1) "1"
  ["tables"]=>
  unicode(1) "1"
  ["cookies"]=>
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
  ["win16"]=>
  unicode(0) ""
  ["win32"]=>
  unicode(0) ""
  ["win64"]=>
  unicode(0) ""
  ["backgroundsounds"]=>
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
  unicode(76) "%XA7^mozilla/5\.0 \(windows; .; windows nt 5\.1; .*rv:1\.9.*\) gecko/.*$%XA7"
  ["browser_name_pattern"]=>
  unicode(58) "Mozilla/5.0 (Windows; ?; Windows NT 5.1; *rv:1.9*) Gecko/*"
  ["parent"]=>
  unicode(11) "Mozilla 1.9"
  ["platform"]=>
  unicode(5) "WinXP"
  ["win32"]=>
  unicode(1) "1"
  ["browser"]=>
  unicode(7) "Mozilla"
  ["version"]=>
  unicode(3) "1.9"
  ["majorver"]=>
  unicode(1) "1"
  ["minorver"]=>
  unicode(1) "9"
  ["alpha"]=>
  unicode(1) "1"
  ["frames"]=>
  unicode(1) "1"
  ["iframes"]=>
  unicode(1) "1"
  ["tables"]=>
  unicode(1) "1"
  ["cookies"]=>
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
  ["beta"]=>
  unicode(0) ""
  ["win16"]=>
  unicode(0) ""
  ["win64"]=>
  unicode(0) ""
  ["backgroundsounds"]=>
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
  unicode(81) "%XA7^mozilla/5\.0 \(x11; .*; linux.*; .*; rv:1\..*\) gecko/.* firebird/0\..*$%XA7"
  ["browser_name_pattern"]=>
  unicode(60) "Mozilla/5.0 (X11; *; Linux*; *; rv:1.*) Gecko/* Firebird/0.*"
  ["parent"]=>
  unicode(8) "Firebird"
  ["browser"]=>
  unicode(8) "Firebird"
  ["frames"]=>
  unicode(1) "1"
  ["iframes"]=>
  unicode(1) "1"
  ["tables"]=>
  unicode(1) "1"
  ["cookies"]=>
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
  ["backgroundsounds"]=>
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
  unicode(63) "%XA7^mozilla/5\.0 \(x11; .*linux.*; .*rv:1\.4.*\) gecko/.*$%XA7"
  ["browser_name_pattern"]=>
  unicode(44) "Mozilla/5.0 (X11; *Linux*; *rv:1.4*) Gecko/*"
  ["parent"]=>
  unicode(11) "Mozilla 1.4"
  ["platform"]=>
  unicode(5) "Linux"
  ["browser"]=>
  unicode(7) "Mozilla"
  ["version"]=>
  unicode(3) "1.4"
  ["majorver"]=>
  unicode(1) "1"
  ["minorver"]=>
  unicode(1) "4"
  ["beta"]=>
  unicode(1) "1"
  ["frames"]=>
  unicode(1) "1"
  ["iframes"]=>
  unicode(1) "1"
  ["tables"]=>
  unicode(1) "1"
  ["cookies"]=>
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
  ["win16"]=>
  unicode(0) ""
  ["win32"]=>
  unicode(0) ""
  ["win64"]=>
  unicode(0) ""
  ["backgroundsounds"]=>
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
  unicode(92) "%XA7^mozilla/5\.0 \(windows; .*; windows nt 5\.1; .*; rv:1\..*\) gecko/.* firefox/0\..*$%XA7"
  ["browser_name_pattern"]=>
  unicode(71) "Mozilla/5.0 (Windows; *; Windows NT 5.1; *; rv:1.*) Gecko/* Firefox/0.*"
  ["parent"]=>
  unicode(7) "Firefox"
  ["win32"]=>
  unicode(1) "1"
  ["browser"]=>
  unicode(7) "Firefox"
  ["frames"]=>
  unicode(1) "1"
  ["iframes"]=>
  unicode(1) "1"
  ["tables"]=>
  unicode(1) "1"
  ["cookies"]=>
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
  ["win64"]=>
  unicode(0) ""
  ["backgroundsounds"]=>
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
  unicode(47) "%XA7^mozilla/5\.0 \(.*rv:1\.8.*\) gecko/.*$%XA7"
  ["browser_name_pattern"]=>
  unicode(30) "Mozilla/5.0 (*rv:1.8*) Gecko/*"
  ["parent"]=>
  unicode(11) "Mozilla 1.8"
  ["browser"]=>
  unicode(7) "Mozilla"
  ["version"]=>
  unicode(3) "1.8"
  ["majorver"]=>
  unicode(1) "1"
  ["minorver"]=>
  unicode(1) "8"
  ["frames"]=>
  unicode(1) "1"
  ["iframes"]=>
  unicode(1) "1"
  ["tables"]=>
  unicode(1) "1"
  ["cookies"]=>
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
  ["backgroundsounds"]=>
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
  unicode(47) "%XA7^mozilla/5\.0 \(.*rv:1\.8.*\) gecko/.*$%XA7"
  ["browser_name_pattern"]=>
  unicode(30) "Mozilla/5.0 (*rv:1.8*) Gecko/*"
  ["parent"]=>
  unicode(11) "Mozilla 1.8"
  ["browser"]=>
  unicode(7) "Mozilla"
  ["version"]=>
  unicode(3) "1.8"
  ["majorver"]=>
  unicode(1) "1"
  ["minorver"]=>
  unicode(1) "8"
  ["frames"]=>
  unicode(1) "1"
  ["iframes"]=>
  unicode(1) "1"
  ["tables"]=>
  unicode(1) "1"
  ["cookies"]=>
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
  ["backgroundsounds"]=>
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
  unicode(82) "%XA7^mozilla/5\.0 \(x11; .*; sunos.*; .*; rv:1\.8.*\) gecko/.* firefox/2\.0.*$%XA7"
  ["browser_name_pattern"]=>
  unicode(61) "Mozilla/5.0 (X11; *; SunOS*; *; rv:1.8*) Gecko/* Firefox/2.0*"
  ["parent"]=>
  unicode(11) "Firefox 2.0"
  ["platform"]=>
  unicode(5) "SunOS"
  ["browser"]=>
  unicode(7) "Firefox"
  ["version"]=>
  unicode(3) "2.0"
  ["majorver"]=>
  unicode(1) "2"
  ["frames"]=>
  unicode(1) "1"
  ["iframes"]=>
  unicode(1) "1"
  ["tables"]=>
  unicode(1) "1"
  ["cookies"]=>
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
  ["backgroundsounds"]=>
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
  unicode(84) "%XA7^mozilla/5\.0 \(x11; .*; .*linux.*; .*; rv:1\.8.*\) gecko/.* firefox/2\.0.*$%XA7"
  ["browser_name_pattern"]=>
  unicode(62) "Mozilla/5.0 (X11; *; *Linux*; *; rv:1.8*) Gecko/* Firefox/2.0*"
  ["parent"]=>
  unicode(11) "Firefox 2.0"
  ["platform"]=>
  unicode(5) "Linux"
  ["browser"]=>
  unicode(7) "Firefox"
  ["version"]=>
  unicode(3) "2.0"
  ["majorver"]=>
  unicode(1) "2"
  ["frames"]=>
  unicode(1) "1"
  ["iframes"]=>
  unicode(1) "1"
  ["tables"]=>
  unicode(1) "1"
  ["cookies"]=>
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
  ["backgroundsounds"]=>
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
===DONE===