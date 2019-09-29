--TEST--
Ensures null is returned if versions are compared with invalid operator
--CREDITS--
David Stockton - <dave@davidstockton.com> - i3logix PHP Testfest 2017
--FILE--
<?php
var_dump(version_compare('1.2', '2.1', '??'));
?>
--EXPECT--
NULL
