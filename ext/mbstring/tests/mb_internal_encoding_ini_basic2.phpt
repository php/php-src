--TEST--
Test INI mbstring.internal_encoding basic - encoding when valid specified
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_stripos') or die("skip mb_stripos() is not available in this build");
?>
--INI--
mbstring.internal_encoding=ISO-8859-7
--FILE--
<?php

echo "*** Testing INI mbstring.internal_encoding : basic functionality ***\n";

echo mb_internal_encoding()."\n";
echo ini_get('mbstring.internal_encoding')."\n";
mb_internal_encoding('UTF-8');
echo mb_internal_encoding()."\n";
echo ini_get('mbstring.internal_encoding')."\n";

?>
--EXPECT--
Deprecated: PHP Startup: Use of mbstring.internal_encoding is deprecated in Unknown on line 0
*** Testing INI mbstring.internal_encoding : basic functionality ***
ISO-8859-7
ISO-8859-7
UTF-8
ISO-8859-7
