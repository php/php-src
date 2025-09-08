--TEST--
Test INI mbstring.internal_encoding basic - encoding when valid specified
--EXTENSIONS--
mbstring
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
--EXPECTF--
Deprecated: PHP Startup: Use of mbstring.internal_encoding is deprecated in Unknown on line 0
*** Testing INI mbstring.internal_encoding : basic functionality ***

Deprecated: Function mb_internal_encoding() is deprecated since 8.5, use internal_encoding INI settings instead in %s on line %d
ISO-8859-7
ISO-8859-7

Deprecated: Function mb_internal_encoding() is deprecated since 8.5, use internal_encoding INI settings instead in %s on line %d

Deprecated: Function mb_internal_encoding() is deprecated since 8.5, use internal_encoding INI settings instead in %s on line %d
UTF-8
ISO-8859-7
