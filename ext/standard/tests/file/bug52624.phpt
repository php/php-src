--TEST--
Bug #52624 (tempnam() by-pass open_basedir with inexistent directory)
--INI--
open_basedir=.
--FILE--
<?php

echo tempnam("directory_that_not_exists", "prefix_");

?>
--EXPECTF--
Warning: tempnam(): open_basedir restriction in effect. File(%s) is not within the allowed path(s): (%s) in %s on line %d
