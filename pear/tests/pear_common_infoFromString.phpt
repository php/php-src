--TEST--
PEAR_Common::analyzeSourceCode test
--SKIPIF--
<?php
if (!getenv('PHP_PEAR_RUNTESTS')) {
    echo 'skip';
}
if (!function_exists('token_get_all')) {
    echo 'skip';
}
?>
--FILE--
<?php
putenv('PHP_PEAR_SYSCONF_DIR=' . dirname(__FILE__));

require_once "PEAR/Common.php";

$common = &new PEAR_Common;

PEAR::setErrorHandling(PEAR_ERROR_CALLBACK, 'catchit');

function catchit($err)
{
    echo "Caught error: " . $err->getMessage() . "\n";
}

echo "Test invalid XML\n";

$common->infoFromString('\\goober');

echo "Test valid XML, not a package.xml\n";

$common->infoFromString('<?xml version="1.0" encoding="ISO-8859-1" ?>' .
    "\n<grobbage></grobbage>");

echo "Test valid package.xml, invalid version number\n";

$common->infoFromString('<?xml version="1.0" encoding="ISO-8859-1" ?>' .
    '<package version="10000000"></package>');

echo "Test empty package.xml\n";

$ret = $common->infoFromString('<?xml version="1.0" encoding="ISO-8859-1" ?>' .
    '<package version="1.0"></package>');

var_dump($ret);

?>
--GET--
--POST--
--EXPECT--
Test invalid XML
Caught error: XML error: not well-formed (invalid token) at line 1
Test valid XML, not a package.xml
Caught error: Invalid Package File, no <package> tag
Test valid package.xml, invalid version number
Caught error: No handlers for package.xml version 10000000
Test empty package.xml
array(2) {
  ["provides"]=>
  array(0) {
  }
  ["filelist"]=>
  &array(0) {
  }
}