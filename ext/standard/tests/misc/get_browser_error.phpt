--TEST--
Test get_browser() function : error functionality
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

echo "*** Testing get_browser() : error functionality ***\n";

/* Unknown browser uses defaults. */
var_dump( get_browser( 'foobar', true ) );

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

Warning: get_browser() expects at most 2 parameters, 3 given in %s on line %d
NULL

Warning: get_browser(): HTTP_USER_AGENT variable is not set, cannot determine user agent name in %s on line %d
bool(false)
===DONE===
