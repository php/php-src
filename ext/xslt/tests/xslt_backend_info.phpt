--TEST--
xslt_backend_info: examples for detection of backend features
--SKIPIF--
<?php // vim600: noet sw=4 ts=4 syn=php ai si tw=78
include("skipif.inc");
if(!function_exists('xslt_backend_info')) {
	die("skip\n");
}
// Yeah-right-but-still
if(xslt_backend_name() != "Sablotron") {
	die("skip This test currently only supports Sablotron");
}
if("No information available" == xslt_backend_info()) {
	die("skip Information could not be detected.");
}
?>
--FILE--
<?php
/*
 * Test xslt_backend_info: basically this test is provided as a how-to for
 * x-platform packages, which rely on certain features which may or may
 * not be available depending on what has been linked into the backend.
 */

$tmp = explode("\n", xslt_backend_info());
$info = array();
foreach($tmp AS $line) {
	list($key, $value) = explode(": ", $line, 2);
	$info[strtolower($key)] = $value;
}

if(FALSE === strstr($info['libs'], " -lexpat")) {
	die("You're configuration is missing expat, which conflicts with sanity.");
}

if(FALSE === strstr($info['libs'], " -liconv")) {
	echo("You don't have iconv support\n");
} else {
	echo("You have iconv support\n");
}
if(FALSE === strstr($info['libs'], " -ljs")) {
	echo("You don't have JavaScript support\n");
} else {
	echo("You have JavaScript support\n");
}
?>
--EXPECTREGEX--
You (don't )?have iconv support
You (don't )?have JavaScript support
