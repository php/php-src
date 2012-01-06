--TEST--
Bug 60675: htmlentities(ENT_COMPAT, windows-1251) for ISO-8859-1 encoded scripts
--CREDITS--
Daniel Convissor <danielc@php.net>
--XFAIL--
Bug 60675 exists
--FILE--
<?php

/*
 * This script needs to be encoded in ISO-8859-1 for the test to work.
 * Check the encoding with "file -bi"
 * If the encoding is wrong, change it with:
 *   mv bug60675.phpt bug60675.tmp
 *   iconv -f <output of file -bi> -t ISO-8859-1 bug60675.tmp > bug60675.phpt
 *   rm bug60675.tmp
 */

$in = 'Тестируем';
echo htmlentities($in, ENT_COMPAT, 'windows-1251');

?>
--EXPECT--
&#1058;&#1077;&#1089;&#1090;&#1080;&#1088;&#1091;&#1077;&#1084;
