--TEST--
Test get_browser() function : error functionality 
--INI--
browscap=ext/standard/tests/misc/browscap.ini
unicode.runtime_encoding = utf-8
unicode.output_encoding = utf-8
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

$browsers = include dirname(__FILE__) . DIRECTORY_SEPARATOR . 'browsernames.php';

/* Needed for the unicode_decode() call later */
unicode_set_error_mode( TO_UNICODE, U_CONV_ERROR_ESCAPE_UNICODE );

echo "*** Testing get_browser() : error functionality ***\n";

/* Unknown browser uses defaults. */
$result = @get_browser( 'foobar', true );
foreach( $result as $k => $val ) {
	$result[$k] = unicode_decode( $val, 'UTF-8' );
}
var_dump( $result );

/* More than expected arguments */
var_dump( get_browser( 'foobar', true, 15 ) );

/* Some wrong parameters, no HTTP_USER_AGENT set */
var_dump( get_browser( null, 'foobar' ) );

?>
===DONE===
--EXPECTF--
*** Testing get_browser() : error functionality ***
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

Warning: get_browser() expects at most 2 parameters, 3 given in %s on line %d
NULL

Warning: get_browser(): HTTP_USER_AGENT variable is not set, cannot determine user agent name in %s on line %d
bool(false)
===DONE===
