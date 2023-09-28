--TEST--
Bug #52624 (tempnam() by-pass open_basedir with inexistent directory)
--INI--
open_basedir=.
--FILE--
<?php

echo tempnam("directory_that_not_exists", "prefix_");

?>
--EXPECTF--
Notice: tempnam('directory_that_...', 'prefix_'): file created in the system's temporary directory in %s on line %d

Warning: tempnam('directory_that_...', 'prefix_'): open_basedir restriction in effect. File(%s) is not within the allowed path(s): (.) in %s on line %d
