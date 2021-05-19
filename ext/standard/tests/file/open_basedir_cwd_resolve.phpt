--TEST--
CWD on open_basedir should not imply that everything is accessible
--FILE--
<?php

$cwd = getcwd();
ini_set('open_basedir', $cwd);
var_dump(file_get_contents('/some/path/outside/open/basedir'));

?>
--EXPECTF--
Warning: file_get_contents(): open_basedir restriction in effect. File(/some/path/outside/open/basedir) is not within the allowed path(s): (%s) in %s on line %d

Warning: file_get_contents(/some/path/outside/open/basedir): failed to open stream: Operation not permitted in %s on line %d
bool(false)
