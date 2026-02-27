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
--EXPECTF--
Deprecated: PHP Startup: Use of mbstring.internal_encoding is deprecated in Unknown on line 0

Warning: PHP Startup: Unknown encoding "BAD" in ini setting in Unknown on line 0
*** Testing INI mbstring.internal_encoding: invalid encoding specified in INI ***

Deprecated: Function mb_internal_encoding() is deprecated since 8.5, use internal_encoding INI settings instead in %s on line %d
UTF-8
BAD

Deprecated: Function mb_internal_encoding() is deprecated since 8.5, use internal_encoding INI settings instead in %s on line %d

Deprecated: Function mb_internal_encoding() is deprecated since 8.5, use internal_encoding INI settings instead in %s on line %d
UTF-8
BAD
