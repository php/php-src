--TEST--
mb_eregi() and invalid arguments
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (!function_exists("mb_eregi")) print "skip mb_eregi() not available";
?>
--FILE--
<?php

mb_regex_encoding('utf-8');

var_dump(mb_eregi('z', 'XYZ'));
var_dump(mb_eregi('xyzp', 'XYZ'));
var_dump(mb_eregi('ö', 'Öäü'));
?>
--EXPECTF--
Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d

Deprecated: Function mb_eregi() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
bool(true)

Deprecated: Function mb_eregi() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
bool(false)

Deprecated: Function mb_eregi() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
bool(true)
