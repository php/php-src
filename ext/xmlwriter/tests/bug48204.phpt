--TEST--
xmlwriter_open_uri with PHP_MAXPATHLEN + 1
--SKIPIF--
<?php if (!extension_loaded("xmlwriter")) print "skip"; ?>
<?php if (!defined('PHP_MAXPATHLEN') && !substr(PHP_OS, 0, 3) == "WIN" && !stristr(PHP_OS, 'linux')) print "skip unknown PHP_MAXPATHLEN"; ?>
--FILE--
<?php 
if (!defined('PHP_MAXPATHLEN')) {
	if (substr(PHP_OS, 0, 3) == "WIN") {
		define('PHP_MAXPATHLEN', 260);
	} else if (stristr(PHP_OS, 'linux')) {
		define('PHP_MAXPATHLEN', 4096);
	} else {
		define('PHP_MAXPATHLEN', 1024); // Darwin, AIX for instance
	}
}
$path = str_repeat('a', PHP_MAXPATHLEN + 1);
var_dump(xmlwriter_open_uri('file:///' . $path));
?>
--CREDIT--
Koen Kuipers koenk82@gmail.com
Theo van der Zee
#Test Fest Utrecht 09-05-2009
--EXPECTF--

Warning: xmlwriter_open_uri(): Unable to resolve file path in %s on line %d
bool(false)
