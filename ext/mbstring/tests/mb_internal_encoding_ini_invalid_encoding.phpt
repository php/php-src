--TEST--
Test INI mbstring.internal_encoding basic - invalid encoding specified in INI
--EXTENSIONS--
mbstring
--INI--
mbstring.internal_encoding=BAD
--FILE--
<?php

echo "*** Testing INI mbstring.internal_encoding: invalid encoding specified in INI ***\n";

echo mb_internal_encoding()."\n";
echo ini_get('mbstring.internal_encoding')."\n";
mb_internal_encoding('UTF-8');
echo mb_internal_encoding()."\n";
echo ini_get('mbstring.internal_encoding')."\n";

?>
--EXPECT--
Deprecated: PHP Startup: Use of mbstring.internal_encoding is deprecated in Unknown on line 0

Warning: PHP Startup: Unknown encoding "BAD" in ini setting in Unknown on line 0
*** Testing INI mbstring.internal_encoding: invalid encoding specified in INI ***
UTF-8
BAD
UTF-8
BAD
